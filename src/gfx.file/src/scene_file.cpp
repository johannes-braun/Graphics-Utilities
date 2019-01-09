#include "file.hpp"
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/vector3.h>
#include <gfx.core/log.hpp>
#include <glm/glm.hpp>

namespace gfx {
void handle_node(scene_file& file, aiNode* node, const aiScene* scene, const glm::mat4& transform) noexcept
{
    const glm::mat4 node_trafo = transform * transpose(reinterpret_cast<glm::mat4&>(node->mTransformation));

    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
    { file.mesh.geometries.at(node->mMeshes[i]).transformation = static_cast<gfx::transform>(node_trafo); }

    for (uint32_t i = 0; i < node->mNumChildren; ++i) { handle_node(file, node->mChildren[i], scene, node_trafo); }
}

class progress_t : public Assimp::ProgressHandler
{
    std::function<bool(float)> on_progress;
    float                      last = 0;

public:
    progress_t(const std::function<bool(float)>& on_progress) : on_progress(on_progress) {}
    bool Update(float percentage) override
    {
        if (percentage > last && on_progress)
        {
            last = percentage;
            return on_progress(0.5f * percentage);
        }
        return true;
    }
};

scene_file::scene_file(const files::path& path, float scale, const std::function<bool(float)>& on_progress) : file(path)
{
    if (!exists(file::path))
    {
        elog << "Scene \"" << path << "\" could not be loaded.";
        return;
    }
    Assimp::Importer importer;
    progress_t       handler(on_progress);
    importer.SetProgressHandler(&handler);

    auto              n = std::chrono::steady_clock::now();
    file_mapping      mapping(*this, file_access::r, size());
    file_mapping_view mapping_view(mapping, 0, size());
    const aiScene*    scene = importer.ReadFile(file::path.string().c_str(), aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices
                                                                              | aiProcess_ImproveCacheLocality | aiProcess_LimitBoneWeights
                                                                              | aiProcess_RemoveRedundantMaterials | aiProcess_Triangulate
                                                                              | aiProcess_GenUVCoords | aiProcess_SortByPType
                                                                              | aiProcess_FindDegenerates | aiProcess_FindInvalidData);
    importer.SetProgressHandler(nullptr);
    ilog << (std::chrono::steady_clock::now() - n).count();
    materials.resize(scene->mNumMaterials);
    
    std::atomic_int current = 0;
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < static_cast<int>(scene->mNumMaterials); ++i)
    {
        const auto                ai_material = scene->mMaterials[i];
        aiString                  name;
        [[maybe_unused]] aiReturn r                = ai_material->Get(AI_MATKEY_NAME, name);
        material&                 current_material = materials[i];

        current_material.name = std::string(name.data, name.length);
        ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor4D&>(current_material.color_diffuse));
        ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, reinterpret_cast<aiColor4D&>(current_material.color_emissive));
        ai_material->Get(AI_MATKEY_COLOR_REFLECTIVE, reinterpret_cast<aiColor4D&>(current_material.color_reflective));
        ai_material->Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor4D&>(current_material.color_specular));
        ai_material->Get(AI_MATKEY_COLOR_TRANSPARENT, reinterpret_cast<aiColor4D&>(current_material.color_transparent));

        const auto load_if = [&](auto& into, auto components, auto... matkey) {
            aiString p;
            ai_material->Get(matkey..., p);
            std::filesystem::path texpath = file::path.parent_path() / p.C_Str();
            if (p.length != 0 && !std::filesystem::is_directory(texpath) && std::filesystem::exists(texpath))
            {
                ilog << "Loading texture from " << texpath;
                into = image_file(texpath, bits::b8, components);
            }
        };


        load_if(current_material.texture_diffuse, 4, AI_MATKEY_TEXTURE_DIFFUSE(0));
        load_if(current_material.texture_bump, 1, AI_MATKEY_TEXTURE_HEIGHT(0));
        on_progress(0.5f + 0.25f * (++current / double(scene->mNumMaterials)));
    }
    current            = 0;
    const auto to_vec3 = [](const aiVector3D& vec) { return glm::vec3(vec.x, vec.y, vec.z); };
    for (int m = 0; m < static_cast<int>(scene->mNumMeshes); ++m)
    {
        const auto ai_mesh = scene->mMeshes[m];
        mesh3d     current_mesh;
        current_mesh.indices.resize(ai_mesh->mNumFaces * 3);
#pragma omp parallel for schedule(static)
        for (auto i = 0; i < static_cast<int>(ai_mesh->mNumFaces); ++i)
        {
            if (ai_mesh->mFaces[0].mNumIndices != 3) continue;
            current_mesh.indices[3 * i]     = ai_mesh->mFaces[i].mIndices[0];
            current_mesh.indices[3 * i + 1] = ai_mesh->mFaces[i].mIndices[1];
            current_mesh.indices[3 * i + 2] = ai_mesh->mFaces[i].mIndices[2];
        }

        current_mesh.vertices.resize(ai_mesh->mNumVertices);
        for (auto i = 0; i < static_cast<int>(ai_mesh->mNumVertices); ++i)
        {
            current_mesh.vertices[i] =
                vertex3d(to_vec3(ai_mesh->mVertices[i]),
                         ai_mesh->HasTextureCoords(0) ? glm::vec2(to_vec3(ai_mesh->mTextureCoords[0][i])) : glm::vec2(0),
                         to_vec3(ai_mesh->mNormals[i]));
        }

        auto& geo        = current_mesh.geometries.emplace_back();
        geo.index_count  = static_cast<u32>(current_mesh.indices.size());
        geo.vertex_count = static_cast<u32>(current_mesh.vertices.size());
        current_mesh.compute_bounds();
        mesh += current_mesh;
        mesh_names.emplace_back(ai_mesh->mName.C_Str());
        mesh_material_indices[u32(mesh.geometries.size() - 1)] = ai_mesh->mMaterialIndex;

        on_progress(0.5f + 0.25f + 0.25f * (++current / double(scene->mNumMeshes)));
    }

    handle_node(*this, scene->mRootNode, scene, glm::scale(glm::vec3(scale)));
    on_progress(1.f);
}
}    // namespace gfx
