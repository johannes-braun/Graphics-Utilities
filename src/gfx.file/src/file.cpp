#include "file.hpp"
#include "loaders/tinyfiledialogs.h"
#include <gfx.core/log.hpp>
#include <fstream>

#include "file_config.hpp"

#if __has_include(<windows.h>)
#include <io.h>
#include <windows.h>
#endif

namespace gfx {
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

size_t file::size() const
{
    std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

file_mapping::file_mapping(file f, file_access access, size_t size) : _file(f), _access(access)
{
#if _WIN32
    const auto a = [access] {
        switch (access)
        {
        case file_access::r: return PAGE_READONLY;
        case file_access::rw: return PAGE_READWRITE;
        default: return 0;
        }
    }();
    const auto flags = [access] {
        switch (access)
        {
        case file_access::r: return L"r";
        case file_access::rw: return L"rw";
        default: return L"";
        }
    }();
    std::wstring v = _file.path;

    for (auto& wc : v)
        if (wc == '\\')
            wc = '/';
    _wfopen_s(reinterpret_cast<FILE**>(&_file_handle), v.c_str(), flags);
    const auto file_handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(static_cast<FILE*>(_file_handle))));
    _mapping_handle =
        CreateFileMappingW(file_handle, nullptr, a, u32((size >> 32) & 0xffffffff), u32(size & 0xffffffff), v.c_str());
#else

#endif
}

file_mapping::~file_mapping()
{
#if _WIN32
    if(_mapping_handle)
        CloseHandle(static_cast<HANDLE>(_mapping_handle));
    if(_file_handle)
        fclose(static_cast<FILE*>(_file_handle)); 
#else

#endif
}

file_mapping::file_mapping(file_mapping&& other) noexcept
{
    _file = std::move(other._file);
    _file_handle = other._file_handle;
    _mapping_handle = other._mapping_handle;
    _access = other._access;

    other._file_handle = nullptr;
    other._mapping_handle = nullptr;
}

file_mapping& file_mapping::operator=(file_mapping&& other) noexcept
{
    _file = std::move(other._file);
    _file_handle = other._file_handle;
    _mapping_handle = other._mapping_handle;
    _access = other._access;

    other._file_handle = nullptr;
    other._mapping_handle = nullptr;
    return *this;
}

#if _WIN32
    DWORD get_access(file_access a)
{
    switch(a)
    {
    case file_access::r: return FILE_MAP_READ;
    case file_access::rw: return FILE_MAP_ALL_ACCESS;
    default: return 0;
    }
}
#endif

file_mapping_view::file_mapping_view(file_mapping& mapping, size_t offset, size_t size)
    : _offset(offset), _size(size), _handle(mapping._mapping_handle), _access(mapping._access)
{
#if _WIN32
    const auto access = get_access(_access);
    _data = MapViewOfFile(static_cast<HANDLE>(_handle), access, u32((_offset >> 32) & 0xffffffff), u32(_offset & 0xffffffff), _size);
    auto err = GetLastError();
#else

#endif
}

file_mapping_view::~file_mapping_view()
{
#if _WIN32
    if (_data)
        UnmapViewOfFile(_data);
#else

#endif
}

    file_mapping_view::file_mapping_view(file_mapping_view&& other) noexcept
{
        _data = other._data;
        _offset = other._offset;
        _size = other._size;
        _handle = other._handle;
        _access = other._access;

        other._handle = nullptr;
        other._data = nullptr;
        other._size = 0;
        other._offset = 0;
}

file_mapping_view& file_mapping_view::operator=(file_mapping_view&& other) noexcept
{
    _data = other._data;
    _offset = other._offset;
    _size = other._size;
    _handle = other._handle;
    _access = other._access;

    other._handle = nullptr;
    other._data = nullptr;
    other._size = 0;
    other._offset = 0;
    return *this;
}

    file_mapping_view::file_mapping_view(const file_mapping_view& other)
{
#if _WIN32
        _offset = other._offset;
        _size = other._size;
        _handle = other._handle;
        _access = other._access;
        const auto access = get_access(_access);
        _data = MapViewOfFile(static_cast<HANDLE>(other._handle), access, u32((_offset >> 32) & 0xffffffff), u32(_offset & 0xffffffff), _size);
#else

#endif
}

file_mapping_view& file_mapping_view::operator=(const file_mapping_view& other)
{
#if _WIN32
    _offset = other._offset;
    _size = other._size;
    _handle = other._handle;
    _access = other._access;
    const auto access = get_access(_access);
    _data = MapViewOfFile(static_cast<HANDLE>(other._handle), access, u32((_offset >> 32) & 0xffffffff), u32(_offset & 0xffffffff), _size);
#else

#endif
    return *this;
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
    while (multiselect && *multiselect != '\0')
    {
        if (*multiselect == '|')
        {
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
    if (path)
    {
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
    if (path)
    {
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
    if (path)
    {
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
}    // namespace gfx