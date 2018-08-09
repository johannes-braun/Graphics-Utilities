#include "tinyfiledialogs.h"
#include "file.hpp"
#include <gfx/log.hpp>

#include <gfx/file/file_config.hpp>

namespace gfx
{
file::file(const files::path& path) : path(path)
{
    if (exists(path)) return;

    const auto global_res_directory = files::path(relative_path) / local_res_directory;
    if (exists(local_res_directory / path))
        this->path = local_res_directory / path;
    else if (exists(global_res_directory / path))
        this->path = global_res_directory / path;
    else
        elog << "File not found: " << path;
}

file::operator const files::path&() const noexcept
{
    return path;
}

std::optional<file> file::open_dialog(const std::string& title, const files::path& default_path)
{
    const char* path = tinyfd_openFileDialog(title.c_str(), default_path.string().c_str(), 0, nullptr, nullptr, false);
    return path ? file(path) : std::optional<file>();
}

std::optional<file> file::open_dialog(const std::string& title, const files::path& default_path, const std::vector<std::string>& filters)
{
    std::vector<const char*> f(filters.size());
    for (int i = 0; i < f.size(); ++i) f[i] = filters[i].c_str();
    const char* path = tinyfd_openFileDialog(title.c_str(), default_path.string().c_str(), int(f.size()), f.data(), nullptr, false);
    return path ? file(path) : std::optional<file>();
}

std::optional<file> file::open_dialog(const std::string& title, const files::path& default_path, const std::vector<std::string>& filters,
                                      const std::string& filter_description)
{
    std::vector<const char*> f(filters.size());
    for (int i = 0; i < f.size(); ++i) f[i] = filters[i].c_str();
    const char* path =
        tinyfd_openFileDialog(title.c_str(), default_path.string().c_str(), int(f.size()), f.data(), filter_description.c_str(), false);
    return path ? file(path) : std::optional<file>();
}

std::vector<file> separate(const char* multiselect)
{
    std::vector<file> result;
    const char*       current = multiselect;
    while (multiselect && *multiselect != '\0') {
        if (*multiselect == '|') {
            result.push_back(file(std::string{current, multiselect}));
            current = multiselect + 1;
        }
        ++multiselect;
    }
    if (multiselect) result.push_back(file(std::string{current, multiselect}));
    return result;
}

std::vector<file> file::open_dialog_multi(const std::string& title, const files::path& default_path)
{
    const char* path = tinyfd_openFileDialog(title.c_str(), default_path.string().c_str(), 0, nullptr, nullptr, true);
    return separate(path);
}

std::vector<file> file::open_dialog_multi(const std::string& title, const files::path& default_path,
                                          const std::vector<std::string>& filters)
{
    std::vector<const char*> f(filters.size());
    for (int i = 0; i < f.size(); ++i) f[i] = filters[i].c_str();
    const char* path = tinyfd_openFileDialog(title.c_str(), default_path.string().c_str(), int(f.size()), f.data(), nullptr, true);
    return separate(path);
}

std::vector<file> file::open_dialog_multi(const std::string& title, const files::path& default_path,
                                          const std::vector<std::string>& filters, const std::string& filter_description)
{
    std::vector<const char*> f(filters.size());
    for (int i = 0; i < f.size(); ++i) f[i] = filters[i].c_str();
    const char* path =
        tinyfd_openFileDialog(title.c_str(), default_path.string().c_str(), int(f.size()), f.data(), filter_description.c_str(), true);
    return separate(path);
}

std::optional<file> file::save_dialog(const std::string& title, const files::path& default_path)
{
    const char* path = tinyfd_saveFileDialog(title.c_str(), default_path.string().c_str(), 0, nullptr, nullptr);
    if (path) {
        file result;
        result.path = path;
        return result;
    }
    return {};
}

std::optional<file> file::save_dialog(const std::string& title, const files::path& default_path, const std::vector<std::string>& filters)
{
    std::vector<const char*> f(filters.size());
    for (int i = 0; i < f.size(); ++i) f[i] = filters[i].c_str();
    const char* path = tinyfd_saveFileDialog(title.c_str(), default_path.string().c_str(), int(f.size()), f.data(), nullptr);
    if (path) {
        file result;
        result.path = path;
        return result;
    }
    return {};
}

std::optional<file> file::save_dialog(const std::string& title, const files::path& default_path, const std::vector<std::string>& filters,
                                      const std::string& filter_description)
{
    std::vector<const char*> f(filters.size());
    for (int i = 0; i < f.size(); ++i) f[i] = filters[i].c_str();
    const char* path =
        tinyfd_saveFileDialog(title.c_str(), default_path.string().c_str(), int(f.size()), f.data(), filter_description.c_str());
    if (path) {
        file result;
        result.path = path;
        return result;
    }
    return {};
}

std::optional<files::path> file::folder_dialog(const std::string& title, const files::path& default_path)
{
    const char* path = tinyfd_selectFolderDialog(title.c_str(), default_path.string().c_str());
    if (path) return file(path);
    return {};
}

void file::popup(const std::string& title, const std::string& message, popup_icon icon)
{
    const char* ic = [&]() {
        switch (icon)
        {
        case popup_icon::error: return "error";
        case popup_icon::info: return "info";
        case popup_icon::warning: return "warning";
        }
        return "";
    }();

    tinyfd_notifyPopup(title.c_str(), message.c_str(), ic);
}

msg_result file::message(const std::string& title, const std::string& message, msg_type type, msg_icon icon, int default_button)
{
    const char* ic = [&]() {
        switch (icon)
        {
        case msg_icon::error: return "error";
        case msg_icon::info: return "info";
        case msg_icon::warning: return "warning";
        case msg_icon::question: return "question";
        }
        return "";
    }();

    const char* ty = [&]() {
        switch (type)
        {
        case msg_type::ok: return "ok";
        case msg_type::ok_cancel: return "okcancel";
        case msg_type::yes_no: return "yesno";
        case msg_type::yes_no_cancel: return "yesnocancel";
        }
        return "";
    }();

    switch (tinyfd_messageBox(title.c_str(), message.c_str(), ty, ic, default_button))
    {
    case 0:
        if (type == msg_type::yes_no)
            return msg_result::no;
        else
            return msg_result::cancel;
    case 1:
        if (type == msg_type::ok || type == msg_type::ok_cancel)
            return msg_result::ok;
        else
            return msg_result::yes;
    case 2: return msg_result::no;
    default: return msg_result::cancel;
    };
}
}