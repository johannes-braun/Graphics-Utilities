#include "geometry.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <jpu/log.hpp>

namespace res
{
    void handle_node(geometry_file& file, aiNode* node, const glm::mat4& transform)
    {
        const glm::mat4 node_trafo = transform * transpose(reinterpret_cast<glm::mat4&>(node->mTransformation));
        
        for(uint32_t i = 0; i < node->mNumMeshes; ++i)
        {
            file.meshes.get_by_index(node->mMeshes[i]).transform = static_cast<res::transform>(node_trafo);
        }

        for(uint32_t i=0; i < node->mNumChildren; ++i)
        {
            handle_node(file, node->mChildren[i], node_trafo);
        }
    }

    geometry_file load_geometry(const std::experimental::filesystem::path& path)
    {
        geometry_file result;
        if (!exists(path))
        {
            log_e << "File cannot be opened: " << path;
            return result;
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.string(),
            aiProcess_GenSmoothNormals |
            aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality |
            aiProcess_LimitBoneWeights |
            aiProcess_RemoveRedundantMaterials |
            aiProcess_Triangulate |
            aiProcess_GenUVCoords |
            aiProcess_SortByPType |
            aiProcess_FindDegenerates |
            aiProcess_FindInvalidData);

        result.meshes.reserve(scene->mNumMeshes);
        result.materials.reserve(scene->mNumMaterials);

        for(uint32_t i = 0; i < scene->mNumMaterials; ++i)
        {
            const auto ai_material = scene->mMaterials[i];
            aiString name; 
            [[maybe_unused]] aiReturn r = ai_material->Get(AI_MATKEY_NAME, name);
            [[maybe_unused]] material& current_material = result.materials.push(name.C_Str(), material());
        }

        const auto to_vec3 = [](const aiVector3D& vec) { return glm::vec3(vec.x, vec.y, vec.z); };
        for (uint32_t m = 0; m < scene->mNumMeshes; ++m)
        {
            const auto ai_mesh = scene->mMeshes[m];
            mesh& current_mesh = result.meshes.push(ai_mesh->mName.C_Str(), mesh());
            current_mesh.indices.resize(ai_mesh->mNumFaces * 3);
#pragma omp parallel for schedule(static)
            for (auto i = 0; i < static_cast<int>(ai_mesh->mNumFaces); ++i)
            {
                if (ai_mesh->mFaces[0].mNumIndices != 3)
                    continue;
                current_mesh.indices[3 * i] = ai_mesh->mFaces[i].mIndices[0];
                current_mesh.indices[3 * i + 1] = ai_mesh->mFaces[i].mIndices[1];
                current_mesh.indices[3 * i + 2] = ai_mesh->mFaces[i].mIndices[2];
            }

            current_mesh.vertices.resize(ai_mesh->mNumVertices);
#pragma omp parallel for schedule(static)
            for (auto i = 0; i < static_cast<int>(ai_mesh->mNumVertices); ++i)
            {
                current_mesh.vertices[i] = vertex(
                    to_vec3(ai_mesh->mVertices[i]),
                    ai_mesh->HasTextureCoords(0) ? glm::vec2(to_vec3(ai_mesh->mTextureCoords[0][i])) : glm::vec2(0),
                    to_vec3(ai_mesh->mNormals[i])
                );
            }
            
            current_mesh.material = result.materials.data() + ai_mesh->mMaterialIndex;
        }

        handle_node(result, scene->mRootNode, glm::mat4(1.f));

        return result;
    }
}
