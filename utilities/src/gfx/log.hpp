#pragma once
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <tuple>
#include <utility>

#include <array>
#include <atomic>
#include <mutex>
#include <sstream>
#include <string>

#if __has_include(<Windows.h>)
// Used for setting the ENABLE_VIRTUAL_TERMINAL_PROCESSING flag in the Windows 10 CMD.
struct WinCMD
{
};
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

//#include <iostream>
//#include <experimental/filesystem>
//
//#if __has_include(<Windows.h>)
//// Used for setting the ENABLE_VIRTUAL_TERMINAL_PROCESSING flag in the Windows 10 CMD.
// struct WinCMD {};
//#define DEFINE_CONSOLEV2_PROPERTIES
//#ifndef NOMINMAX
//#define NOMINMAX
//#endif
//#undef APIENTRY
//#include <Windows.h>
//
// inline WinCMD mkwincmd()
//{
//	const HANDLE h_std = GetStdHandle(STD_OUTPUT_HANDLE);
//	DWORD md = 0;
//	GetConsoleMode(h_std, &md);
//	SetConsoleMode(h_std, md | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
//	return {};
//}
//#endif
//
// namespace gfx::logging
//{
//	enum class severity
//	{
//		verbose = 0,
//		debug = 1,
//		info = 2,
//		hint = 3,
//		warning = 4,
//		error = 5
//	};
//
//	constexpr bool operator<(severity a, severity b)
//	{
//		return static_cast<int>(a) < static_cast<int>(b);
//	}
//
//	constexpr bool operator>(severity a, severity b)
//	{
//		return static_cast<int>(a) > static_cast<int>(b);
//	}
//
//	constexpr bool operator<=(severity a, severity b)
//	{
//		return static_cast<int>(a) <= static_cast<int>(b);
//	}
//
//	constexpr bool operator>=(severity a, severity b)
//	{
//		return static_cast<int>(a) >= static_cast<int>(b);
//	}
//
//	// Wraps 3 chars into a uint32_t tag to be saved in log_data.
//	// This class is actually just for the convenience of the operator[] for log_text.
//	class tag {
//	public:
//		constexpr tag(const char* tag) noexcept : _tag(0 | (tag[0] << 0) | (tag[1] << 8) | (tag[2] << 16)) {}
//		constexpr tag(const uint32_t tag) noexcept : _tag(tag){}
//		constexpr char operator[](const int i) const { return (_tag >> (static_cast<uint32_t>(i) * 8)) & 0xff; }
//		constexpr operator uint32_t() const { return _tag; }
//
//	private:
//		uint32_t _tag;
//	};
//
//	template<uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t Bright>
//	constexpr int logColor() noexcept
//	{
//		static_assert(Red <= 1 && Green <= 1 && Blue <= 1 && Bright <= 1, "You can only use binary color formats with values 0
// or 1."); 		return 30 + 1 * Red + 2 * Green + 4 * Blue + 60 * Bright;
//	}
//
//	template<uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t Bright, uint32_t Tg> struct log_data
//	{
//		constexpr static auto coloring = logColor<Red, Green, Blue, Bright>();
//		constexpr static tag short_form = Tg;
//	};
//
//	template<severity S> struct log_info;
//	template<> struct log_info<severity::verbose>	: log_data<1, 1, 1, 1, tag("VRB")> {};
//	template<> struct log_info<severity::debug>	: log_data<0, 0, 1, 1, tag("DBG")> {};
//	template<> struct log_info<severity::info>		: log_data<0, 1, 0, 0, tag("INF")> {};
//	template<> struct log_info<severity::hint>		: log_data<1, 0, 1, 1, tag("HNT")> {};
//	template<> struct log_info<severity::warning>	: log_data<1, 1, 0, 0, tag("WRN")> {};
//	template<> struct log_info<severity::error>	: log_data<1, 0, 0, 1, tag("ERR")> {};
//
//	// Creates a colored log prefix text for a given Severity using the according log_data struct.
//	template<severity S> constexpr std::array<char, 23> log_text{
//		'\033', '[',
//		'0' + (log_info<S>::coloring + 10) / 100,
//		'0' + ((log_info<S>::coloring + 10) % 100 / 10),
//		'0' + (log_info<S>::coloring + 10) % 10,
//		'm', ' ', '\033', '[', '1', ';', '9', '8', 'm',
//        log_info<S>::short_form[0],
//        log_info<S>::short_form[1],
//        log_info<S>::short_form[2], ' ',
//
//        '\033', '[', '0', 'm', ' '};
//
//	// A LogChannel is being created every time logging::capture is called.
//	// It shall not be saved anywhere and is used to only print completed Log streams to mutex locked stdout.
//	struct log_channel
//	{
//        log_channel() = default;
//		log_channel(log_channel&&) = default;
//		log_channel(const log_channel&) = delete;
//		void operator=(const log_channel&) = delete;
//		void operator=(log_channel&&) = delete;
//
//		// Destructor is called on the end of the line.
//		~log_channel() { print(); }
//
//		template<typename T> log_channel& operator<<(T&& t)
//		{
//			_stream << t;
//			return *this;
//		}
//
//		void print() const
//		{
//#define JPU_LOG_THREADSAFE
//#if defined(JPU_LOG_THREADSAFE)
//			static std::mutex stdout_mutex;
//            std::unique_lock<std::mutex> lock(stdout_mutex);
//#endif
//			std::cout << _stream.str();
//		}
//
//	private:
//		std::stringstream _stream;
//	};
//
//	// Start a log channel with a colored prefix and file/line info.
//	template<severity S> log_channel capture(const char* file, const int line)
//	{
//#if __has_include(<Windows.h>)
//		// Set color settings once.
//		static WinCMD win_cmd = mkwincmd();
//#endif
//
//        std::stringstream prefix;
//        if (line != -1)
//        {
//            auto fname = std::experimental::filesystem::path(file).filename();
//            auto stem = fname.stem().string();
//            const std::string end(stem.end() - 3, stem.end());
//            if (stem.size() > 15)
//            {
//                stem.resize(15);
//                stem[stem.length() - 1] = '.';
//                stem[stem.length() - 2] = '.';
//                stem[stem.length() - 3] = '.';
//                stem += end;
//            }
//            prefix << logging::log_text<S>.data() << "\033[90m[ " << stem << fname.extension() << ":" << line << " ]\033[0m ";
//        }
//        else
//        {
//            prefix << logging::log_text<S>.data() << "\033[90m[ " << file << " ]\033[0m ";
//        }
//		auto str = prefix.str();
//		log_channel channel;
//		channel << '\n' << str << std::string(58 - str.size(), ' ') << " ";
//		return channel;
//	}
//} // namespace gfx::logging
//
//// Log Settings
// constexpr gfx::logging::severity min_severity =
//#if defined(NDEBUG)
//	gfx::logging::severity::info;
//#else
//	gfx::logging::severity::verbose;
//#endif
//
//// Log definitions
//#define log_out(S) if constexpr((S) >= min_severity) gfx::logging::capture<S>(__FILE__, __LINE__)
//#define tlog_out(tag, S) if constexpr((S) >= min_severity) gfx::logging::capture<S>(tag, -1)
//
//#define log_v log_out(gfx::logging::severity::verbose)
//#define log_d log_out(gfx::logging::severity::debug)
//#define log_i log_out(gfx::logging::severity::info)
//#define log_h log_out(gfx::logging::severity::hint)
//#define log_w log_out(gfx::logging::severity::warning)
//#define log_e log_out(gfx::logging::severity::error)
//
//#define tlog_v(t) tlog_out(t, gfx::logging::severity::verbose)
//#define tlog_d(t) tlog_out(t, gfx::logging::severity::debug)
//#define tlog_i(t) tlog_out(t, gfx::logging::severity::info)
//#define tlog_h(t) tlog_out(t, gfx::logging::severity::hint)
//#define tlog_w(t) tlog_out(t, gfx::logging::severity::warning)
//#define tlog_e(t) tlog_out(t, gfx::logging::severity::error)

namespace gfx
{
template <typename T> struct enable_styling : std::false_type
{
};
template <typename T> constexpr bool enable_styling_v = enable_styling<T>::value;

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
    none       = 0,
    bold       = 1,
    dim        = 2,
    underlined = 4,
    blink      = 5,
    inverted   = 7,
    hidden     = 8,
};

inline static std::atomic_bool lock = true;

namespace log_impl
{
    inline class log_thread
    {
    public:
        ~log_thread()
        {
            std::unique_lock lockx(_mtx);
            lock = false;
        }

        log_thread()
                : _thread([this]() {
                    while(true)
                    {
                        if(_printers.empty())
                        {
                            using namespace std::chrono_literals;
                            std::this_thread::sleep_for(50ms);
                            continue;
                        }

                        // flush all
                        std::unique_lock lockx(_mtx);
                        while(!_printers.empty())
                        {
                            if(!lock)
                                return;
                            _printers.front()();
                            _printers.pop();
                        }
                    }
                })
        {
            _thread.detach();
        }

        void enqueue(std::function<void()> printer)
        {
            std::unique_lock lock(_mtx);
            _printers.push(printer);
        }

    private:
        std::queue<std::function<void()>> _printers;
        std::thread                       _thread;
        std::mutex                        _mtx;
    } logging_thread;

    template <typename... Streams> class logger
    {
        class log_printer
        {
            friend class logger<Streams...>;
            log_printer(logger& parent)
                    : _parent(parent)
                    , _do_style{enable_styling_v<Streams>...}
            {
                *this << style::none;
            }

            logger&                                           _parent;
            std::array<std::stringstream, sizeof...(Streams)> _outputs;
            std::array<bool, sizeof...(Streams)>              _do_style;

            enum class state
            {
                def,
                style
            } _cstate = state::def;

            template <typename Stream> void print_one(Stream& stream, std::size_t index)
            {
                if(_cstate == state::style)
                    logging_thread.enqueue([str = _outputs[index].str(), &strm = stream]() { strm << str << 'm' << std::endl; });
                else
                    logging_thread.enqueue([str = _outputs[index].str(), &strm = stream]() { strm << str << std::endl; });
            }

            template <typename Tuple, std::size_t... Indices> void print_each(Tuple&& tuple, std::index_sequence<Indices...>)
            {
                (print_one(std::get<Indices>(std::forward<Tuple&&>(tuple)), Indices), ...);
            }

        public:
            log_printer(const log_printer& p) = delete;
            log_printer(log_printer&& p)      = default;

            ~log_printer()
            {
                if(_outputs[0].rdbuf()->in_avail())
                    print_each(_parent._streams, std::make_index_sequence<sizeof...(Streams)>{});
            }

            template <typename T> log_printer& operator<<(T&& value)
            {
                for(int i = 0; i < sizeof...(Streams); ++i)
                {
                    using dec = std::decay_t<T>;
                    if constexpr(std::is_same_v<dec, color_fg>)
                    {
                        if(_do_style[i])
                        {
                            _outputs[i] << (_cstate == state::def ? "\033[" : ";");
                            _outputs[i] << "38;5;" << static_cast<uint32_t>(value);
                        }
                        _cstate = state::style;
                    }
                    else if constexpr(std::is_same_v<dec, color_bg>)
                    {
                        if(_do_style[i])
                        {
                            _outputs[i] << (_cstate == state::def ? "\033[" : ";");
                            _outputs[i] << "48;5;" << static_cast<uint32_t>(value);
                        }
                        _cstate = state::style;
                    }
                    else if constexpr(std::is_same_v<dec, style>)
                    {
                        if(_do_style[i])
                        {
                            _outputs[i] << (_cstate == state::def ? "\033[" : ";");
                            _outputs[i] << static_cast<uint32_t>(value);
                        }
                        _cstate = state::style;
                    }
                    else
                    {
                        if(_cstate == state::style)
                            _outputs[i] << "m";
                        _outputs[i] << value;
                        _cstate = state::def;
                    }
                }
                return *this;
            }
        };

    public:
        logger(std::function<void(log_printer&)> prefix_fun, Streams&... streams)
                : _streams(streams...)
                , _prefix_fun(prefix_fun)
        {
        }

        template <typename T> log_printer operator<<(T&& value)
        {
            log_printer out(*this);
            if(_prefix_fun)
                _prefix_fun(out);
            out << style::none << " " << value;
            return out;
        }

    private:
        std::tuple<Streams&...>           _streams;
        std::function<void(log_printer&)> _prefix_fun;
    };
    template <typename F, typename... S> logger(F, S...)->logger<S...>;

} // namespace log_impl
template <> struct enable_styling<std::ostream> : std::true_type
{
};

struct highlighter
{
    highlighter(const std::string& label, uint32_t color)
            : _color(color)
            , _label(label)
    {
    }

    template <typename T> void operator()(T& l) const
    {
        l << style::bold << color_fg(230) << color_bg(_color) << _label << color_bg(250) << ' ' << color_bg(251) << color_fg(_color) << ':'
          << ' ';
    }

private:
    uint32_t    _color;
    std::string _label;
};

enum class logger_label : uint64_t
{
};

constexpr logger_label log_label(const char* l)
{
    uint64_t out = 0;
    for(uint32_t i = 0; i < 7; ++i)
    {
        out |= uint64_t(uint8_t(l[i])) << (i << 3);
    }
    return logger_label{out};
}

static std::string expand_label(logger_label l)
{
    auto s = std::string(" ") + reinterpret_cast<const char*>(&l);
    s.resize(8);
    return s + " ";
}

static std::ofstream log_all("output.log");
static std::ofstream log_err("errors.log");

template <logger_label L, uint32_t C> log_impl::logger clogc{highlighter(expand_label(L), C), std::cout};

inline static log_impl::logger clogd{highlighter(expand_label(log_label("debug")), 27), std::cout};
inline static log_impl::logger clogh{highlighter(expand_label(log_label("hint")), 199), std::cout};
inline static log_impl::logger clogi{highlighter(expand_label(log_label("info")), 40), std::cout, log_all};
inline static log_impl::logger clogw{highlighter(expand_label(log_label("warning")), 208), std::cout, log_all};
inline static log_impl::logger cloge{highlighter(expand_label(log_label("error")), 196), std::cout, log_all, log_err};
} // namespace gfx