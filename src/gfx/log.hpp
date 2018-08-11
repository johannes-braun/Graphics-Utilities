#pragma once
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <thread>
#include <tuple>
#include <utility>

#include <array>
#include <atomic>
#include <mutex>
#include <sstream>
#include <string>

#if GFX_RELEASE
#include "file/file.hpp"
#endif

#if __has_include(<Windows.h>)
// Used for setting the ENABLE_VIRTUAL_TERMINAL_PROCESSING flag in the Windows 10 CMD.
struct WinCMD
{};
#define DEFINE_CONSOLEV2_PROPERTIES
#ifndef NOMINMAX
#define NOMINMAX
#endif
#undef APIENTRY
#include <Windows.h>

inline WinCMD mkwincmd()
{
    const HANDLE h_std = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD        md    = 0;
    GetConsoleMode(h_std, &md);
    SetConsoleMode(h_std, md | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    return {};
}

static WinCMD vtp = mkwincmd();
#endif

namespace gfx
{
template<typename T>
struct enable_styling : std::false_type
{
};
template<typename T>
constexpr bool enable_styling_v = enable_styling<T>::value;

enum class color_fg : uint32_t
{
    none          = 0,
    red           = 1,
    green         = 2,
    yellow        = 3,
    blue          = 4,
    magenta       = 5,
    cyan          = 6,
    light_gray    = 7,
    dark_gray     = 8,
    light_red     = 9,
    light_green   = 10,
    light_yellow  = 11,
    light_blue    = 12,
    light_magenta = 13,
    light_cyan    = 14,
    white         = 15,
    black         = 16,
};

enum class color_bg : uint32_t
{
    none          = 0,
    red           = 1,
    green         = 2,
    yellow        = 3,
    blue          = 4,
    magenta       = 5,
    cyan          = 6,
    light_gray    = 7,
    dark_gray     = 8,
    light_red     = 9,
    light_green   = 10,
    light_yellow  = 11,
    light_blue    = 12,
    light_magenta = 13,
    light_cyan    = 14,
    white         = 15,
    black         = 16,
};

enum class style
{
    none             = 0,
    bold             = 1,
    dim              = 2,
    italic           = 3,
    underlined       = 4,
    blink            = 5,
    overline         = 6,
    inverted         = 7,
    hidden           = 8,
    strikeout        = 9,
    reset_all        = none,
    reset_attributes = 20,
    reset_foreground = 39,
    reset_background = 49,
};

inline static std::atomic_bool lock                = true;
inline static std::atomic_bool log_thread_finished = false;

enum class logger_label : uint64_t
{
};

size_t constexpr cstrlen(const char* str)
{
    return *str ? 1 + cstrlen(str + 1) : 0;
}

constexpr logger_label log_label(const char* l)
{
    uint64_t out = 0;
    auto     len = cstrlen(l);
    for (uint32_t i = 0; i < 7; ++i) {
        if (i < len) out |= uint64_t(uint8_t(l[i])) << (i << 3);
    }
    return logger_label{out};
}

static std::string expand_label(logger_label l)
{
    auto s = std::string(" ") + reinterpret_cast<const char*>(&l);
    s.resize(8, ' ');
    return s + " ";
}

namespace log_impl
{
class log_thread
{
public:
    ~log_thread()
    {
        if (lock) {
            lock = false;
            _thread.join();
        }
    }

    void quit()
    {
        if (lock) {
            lock = false;
            _thread.join();
        }
    }

    log_thread()
          : _thread([this]() {
              while (true) {
                  if (lock && _printers.empty()) {
                      using namespace std::chrono_literals;
                      std::this_thread::sleep_for(1ms);
                  }
                  else
                  {
                      // flush all
                      while (!_printers.empty()) {
                          std::unique_lock lockx(mtx);
                          _printers.front()();
                          _printers.pop();
                      }
                      if (!lock) {
                          break;
                      }
                  }
              }
              log_thread_finished = true;
          })
    {}

    void enqueue(std::function<void()> printer) { _printers.push(printer); }

    std::mutex mtx;

private:
    std::queue<std::function<void()>> _printers;
    std::thread                       _thread;
};
inline log_thread& logging_thread()
{
    static log_thread t;
    return t;
};
}    // namespace log_impl

template<typename... Streams>
class logger
{
    class log_printer
    {
        friend class logger<Streams...>;
        log_printer(logger& parent) : _parent(parent), _do_style{enable_styling_v<Streams>...} {}

        logger&                                           _parent;
        std::array<std::stringstream, sizeof...(Streams)> _outputs;
        std::array<bool, sizeof...(Streams)>              _do_style;

        enum class state
        {
            def,
            style
        } _cstate = state::def;

        template<typename Stream>
        void print_one(Stream& stream, std::size_t index)
        {
            if (_cstate == state::style && enable_styling_v<Stream>)
                log_impl::logging_thread().enqueue([ str = _outputs[index].str(), &strm = stream ]() { strm << str << 'm' << "\n"; });
            else
                log_impl::logging_thread().enqueue([ str = _outputs[index].str(), &strm = stream ]() { strm << str << "\n"; });
        }

        template<typename Tuple, std::size_t... Indices>
        void print_each(Tuple&& tuple, std::index_sequence<Indices...>)
        {
            std::unique_lock lock(log_impl::logging_thread().mtx);
            (print_one(std::get<Indices>(std::forward<Tuple&&>(tuple)), Indices), ...);
        }

        std::stringstream _str_base;

    public:
        log_printer(const log_printer& p) = delete;
        log_printer(log_printer&& p)      = default;

        ~log_printer()
        {
            if (_outputs[0].rdbuf()->in_avail()) {
                *this << style::none;
                print_each(_parent._streams, std::make_index_sequence<sizeof...(Streams)>{});

				#if GFX_RELEASE
                if (_parent._message_box) {
                    auto res = file::message("Log Notification", _str_base.str() + "\n\nWould you like to close the application now?", msg_type::yes_no, msg_icon::error);
                    if (res == msg_result::yes) exit(-1); 
                }
				#endif
            }
        }

        template<typename T>
        log_printer& operator<<(T&& value)
        {
            using dec        = std::decay_t<T>;
            state next_state = _cstate;
            for (int i = 0; i < sizeof...(Streams); ++i) {
                if constexpr (std::is_same_v<dec, color_fg>) {
                    if (_do_style[i]) {
                        _outputs[i] << (_cstate == state::def ? "\033[" : ";");
                        _outputs[i] << "38;5;" << static_cast<uint32_t>(value);
                    }
                    next_state = state::style;
                }
                else if constexpr (std::is_same_v<dec, color_bg>)
                {
                    if (_do_style[i]) {
                        _outputs[i] << (_cstate == state::def ? "\033[" : ";");
                        _outputs[i] << "48;5;" << static_cast<uint32_t>(value);
                    }
                    next_state = state::style;
                }
                else if constexpr (std::is_same_v<dec, style>)
                {
                    if (_do_style[i]) {
                        _outputs[i] << (_cstate == state::def ? "\033[" : ";");
                        _outputs[i] << static_cast<uint32_t>(value);
                    }
                    next_state = state::style;
                }
                else
                {
                    if (_cstate == state::style && _do_style[i]) _outputs[i] << "m";
                    _outputs[i] << value;
                    next_state = state::def;
                }
            }
            if constexpr (!std::is_same_v<dec, color_fg> && !std::is_same_v<dec, color_bg> && !std::is_same_v<dec, style>)
                if (_parent._message_box) _str_base << value;
            _cstate = next_state;
            return *this;
        }
    };

public:
    logger(std::function<void(log_printer&, const std::string&)> prefix_fun, bool message_box, Streams&... streams)
          : _streams(streams...), _prefix_fun(prefix_fun), _message_box(message_box)
    {}

    template<typename T>
    log_printer operator<<(T&& value)
    {
        log_printer out(*this);
        if (_prefix_fun) _prefix_fun(out, "");
        out << style::none << " " << value;
        return out;
    }

    log_printer operator()(const std::string& str)
    {
        log_printer out(*this);
        if (_prefix_fun) _prefix_fun(out, expand_label(log_label(str.c_str())));
        out << style::none << " ";
        return out;
    }

private:
    std::tuple<Streams&...>                               _streams;
    std::function<void(log_printer&, const std::string&)> _prefix_fun;
    bool                                                  _message_box;
};
template<typename F, typename... S>
logger(F, bool, S&...)->logger<S...>;

struct highlighter
{
    highlighter(const std::string& label, uint32_t color) : _color(color), _label(label) {}

    template<typename T>
    void operator()(T& l, const std::string& s) const
    {
        l << style::bold << color_fg(230) << color_bg(_color) << (s.empty() ? _label : s) << color_bg(246) << ' ' << style::underlined
          << style::bold << color_bg(251) << color_fg(246) << " > ";
    }

private:
    uint32_t    _color;
    std::string _label;
};

template<>
struct enable_styling<std::ostream> : std::true_type
{
};

inline std::ofstream& log_err()
{
    static std::ofstream o{"errors.log"};
    return o;
}

inline std::ofstream& log_all()
{
    static std::ofstream o{"output.log"};
    return o;
}

template<color_bg C, logger_label L = log_label("user")>
logger cclog{highlighter(expand_label(L), uint32_t(C)), false, std::cout};
template<color_bg C, logger_label L = log_label("user")>
logger calog{highlighter(expand_label(L), uint32_t(C)), false, std::cout, log_all()};
template<color_bg C, logger_label L = log_label("user")>
inline logger celog{highlighter(expand_label(L), uint32_t(C)), true, std::cout, log_all(), log_err()};

inline static auto& dlog = cclog<color_bg(31), log_label("debug")>;
inline static auto& hlog = cclog<color_bg(59), log_label("hint")>;
inline static auto& ilog = calog<color_bg(35), log_label("info")>;
inline static auto& wlog = calog<color_bg(172), log_label("warning")>;
inline static auto& elog = celog<color_bg(124), log_label("error")>;

#define log_info gfx::color_fg(239) << "[" << __FILE__ << ":" << __LINE__ << " in " << __func__ << "] " << gfx::style::none
}    // namespace gfx
