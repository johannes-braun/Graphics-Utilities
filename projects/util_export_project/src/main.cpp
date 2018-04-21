#include <gfx/file.hpp>
#include <gfx/log.hpp>
#include <pugixml.hpp>

int main()
{
    const char* project_folder_cstr = tinyfd_selectFolderDialog("Select Project Folder", "./");
    if (!project_folder_cstr)
    {
        log_w << "No project folder selected. Exiting.";
        return 0;
    }

    const gfx::files::path project_folder = project_folder_cstr;

    if (!exists(project_folder / "bin" / (project_folder.filename().string() + ".exe")))
    {
        log_e << "Project not built yet or is no valid project. Please run CMake and build a project before exporting it.";
        return 0;
    }

    pugi::xml_document document;
    if(exists(project_folder / "export.xml"))
        document.load_file((project_folder / "export.xml").c_str());

    const pugi::xml_node root = document.child("export");
    const pugi::xml_node globals = root.child("globals");
    const pugi::xml_node dlls = root.child("dll");

    const char* target_folder_cstr = tinyfd_selectFolderDialog("Select Target Folder", "./");
    if (!target_folder_cstr)
    {
        log_e << "No target folder selected. Exiting.";
        return 0;
    }

    gfx::files::path target_folder = target_folder_cstr;
    target_folder /= project_folder.filename();
    if(!create_directory(target_folder) && !exists(target_folder))
    {
        log_e << "Failed to create target directory. Exiting.";
        return 0;
    }

    const gfx::files::path globals_root = SOURCE_DIRECTORY "/global";
    for(const auto& global : globals)
    {
        if(std::strcmp(global.name(), "all") == 0)
        {
            log_i << "Exporting all global resources...";
            log_i << "Exporting " << globals_root / "shd";
            gfx::files::copy(globals_root / "shd", target_folder / "shd", gfx::files::copy_options::recursive);
            log_i << "Exporting " << globals_root / "res";
            gfx::files::copy(globals_root / "res", target_folder / "res", gfx::files::copy_options::recursive);
            break;
        }

        const gfx::files::path target = global.attribute("path").as_string();
        if (exists(globals_root / target))
        {
            log_i << "Exporting " << target;
            if (is_directory(globals_root / target))
                create_directories(target_folder / target);
            else
                create_directories(target_folder / target.parent_path());

            gfx::files::copy(globals_root / target, target_folder / target);
        }
        else
        {
            log_w << "Global resource not found or used: " << (globals_root / target);
        }
    }

    const gfx::files::path bin = target_folder / "bin";
    create_directory(bin);
    for(const auto& dll : dlls)
    {
        if (std::strcmp(dll.name(), "all") == 0)
        {
            log_i << "Exporting all dlls...";
            for(const auto& f : gfx::files::directory_iterator(project_folder / "bin"))
                if (f.path().extension() == ".dll")
                {
                    gfx::files::copy(f.path(), bin / f.path().filename());
                    log_i << "Exporting " << f.path();
                }
            break;
        }

        const gfx::files::path dll_name = dll.attribute("name").as_string();
        if (exists(project_folder / "bin" / dll_name))
        {
            log_i << "Exporting " << dll_name;
            gfx::files::copy(project_folder / "bin" / dll_name, bin / dll_name);
        }
        else
        {
            log_w << "DLL not found or used: " << dll_name;
        }
    }

    const gfx::files::path executable = project_folder.filename().string() + ".exe";
    log_i << "Exporting " << executable;
    gfx::files::copy(project_folder / "bin" / executable, bin / executable);

    if (!is_empty(project_folder / "res"))
    {
        log_i << "Exporting " << (project_folder / "res");
        gfx::files::copy(project_folder / "res", target_folder / "res", gfx::files::copy_options::recursive);
    }
    if (!is_empty(project_folder / "shd"))
    {
        log_i << "Exporting " << (project_folder / "shd");
        gfx::files::copy(project_folder / "shd", target_folder / "shd", gfx::files::copy_options::recursive);
    }

    log_i << "Export successful.";
    log_h << "Press [ENTER] to quit...";
    std::cin.ignore();
}
