#pragma once

#include <array>
#include <sstream>
#include <string_view>
#include <string>
#include <mutex>
#include <iostream>
#include <experimental/filesystem>

#if __has_include(<Windows.h>)
// Used for setting the ENABLE_VIRTUAL_TERMINAL_PROCESSING flag in the Windows 10 CMD.
struct WinCMD {};
#define DEFINE_CONSOLEV2_PROPERTIES
#ifndef NOMINMAX
#define NOMINMAX
#endif
#undef APIENTRY
#include <Windows.h>

inline WinCMD mkwincmd()
{
	// Usable on Windows 10 post-Anniversary-Update. Let's keep the code modern.
	const HANDLE h_std = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD md = 0;
	GetConsoleMode(h_std, &md);
	SetConsoleMode(h_std, md | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	return {};
}
#endif 

namespace jpu::logging
{
	enum class severity
	{
		verbose = 0,
		debug = 1,
		info = 2,
		hint = 3,
		warning = 4,
		error = 5
	};

	constexpr bool operator<(severity a, severity b)
	{
		return static_cast<int>(a) < static_cast<int>(b);
	}

	constexpr bool operator>(severity a, severity b)
	{
		return static_cast<int>(a) > static_cast<int>(b);
	}

	constexpr bool operator<=(severity a, severity b)
	{
		return static_cast<int>(a) <= static_cast<int>(b);
	}

	constexpr bool operator>=(severity a, severity b)
	{
		return static_cast<int>(a) >= static_cast<int>(b);
	}

	// Wraps 3 chars into a uint32_t tag to be saved in log_data.
	// This class is actually just for the convenience of the operator[] for log_text.
	class Tag {
	public:
		constexpr Tag(const char* tag) : m_tag(0 | (tag[0] << 0) | (tag[1] << 8) | (tag[2] << 16)) {}
		constexpr Tag(const uint32_t tag) : m_tag(tag){}
		constexpr char operator[](const int i) const { return (m_tag >> (i * 8)) & 0xff; }
		constexpr operator uint32_t() const { return m_tag; }

	private:
		uint32_t m_tag;
	};

	template<uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t Bright>
	constexpr int logColor()
	{
		static_assert(Red <= 1 && Green <= 1 && Blue <= 1 && Bright <= 1, "You can only use binary color formats with values 0 or 1.");
		return 30 + 1 * Red + 2 * Green + 4 * Blue + 60 * Bright;
	}

	template<uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t Bright, uint32_t Tg> struct log_data
	{
		constexpr static auto coloring = logColor<Red, Green, Blue, Bright>();
		constexpr static Tag short_form = Tg;
	};

	template<severity S> struct log_info;
	template<> struct log_info<severity::verbose>	: log_data<1, 1, 1, 1, Tag("VRB")> {};
	template<> struct log_info<severity::debug>	: log_data<0, 0, 1, 1, Tag("DBG")> {};
	template<> struct log_info<severity::info>		: log_data<0, 1, 0, 1, Tag("INF")> {};
	template<> struct log_info<severity::hint>		: log_data<1, 0, 1, 1, Tag("HNT")> {};
	template<> struct log_info<severity::warning>	: log_data<1, 1, 0, 1, Tag("WRN")> {};
	template<> struct log_info<severity::error>	: log_data<1, 0, 0, 1, Tag("ERR")> {};

	// Creates a colored log prefix text for a given Severity using the according log_data struct.
	template<severity S> constexpr std::array<char, 33> log_text{
		'\033', '[',
		'0' + (log_info<S>::coloring + 10) / 100,
		'0' + ((log_info<S>::coloring + 10) % 100 / 10),
		'0' + (log_info<S>::coloring + 10) % 10,
		'm', ' ',

		'\033', '[', '4', '9', 'm',

		'\033', '[',
		'0' + (log_info<S>::coloring) / 100,
		'0' + ((log_info<S>::coloring) % 100 / 10),
		'0' + (log_info<S>::coloring) % 10,
		'm',

		' ', '[', 
		log_info<S>::short_form[0],
		log_info<S>::short_form[1],
		log_info<S>::short_form[2], ']', ' ',

		'\033', '[', '0', 'm', 'i', 'n', ' ',
		'\0' };

	// A LogChannel is being created every time logging::capture is called.
	// It shall not be saved anywhere and is used to only print completed Log streams to mutex locked stdout.
	struct log_channel
	{
		log_channel() {}
		log_channel(log_channel&&) = default;
		log_channel(const log_channel&) = delete;
		void operator=(const log_channel&) = delete;
		void operator=(log_channel&&) = delete;

		// Destructor is called on the end of the line.
		~log_channel() { print(); }

		template<typename T> log_channel& operator<<(T&& t)
		{
			m_stream << t;
			return *this;
		}

		void print() const
		{
			static std::mutex stdout_mutex;
			std::unique_lock<std::mutex> lock(stdout_mutex);
			std::cout << m_stream.str();
		}

	private:
		std::stringstream m_stream;
	};

	// Start a log channel with a colored prefix and file/line info.
	template<severity S> log_channel capture(const char* file, const int line)
	{
#if __has_include(<Windows.h>)
		// Set color settings once.
		static WinCMD win_cmd = mkwincmd();
#endif

		std::stringstream prefix;
		prefix << logging::log_text<S>.data() << "\033[90m" << std::experimental::filesystem::path(file).filename() << " \033[0m:\033[90m " << line << "\033[0m ";
		auto str = prefix.str();

		log_channel channel;
		channel << '\n' << str << std::string(85 - str.size(), '-') << " ";
		return channel;
	}
}

// Log Settings
constexpr jpu::logging::severity min_severity =
#if defined(NDEBUG)
	jpu::logging::severity::info;
#else
	jpu::logging::severity::verbose;
#endif

// Log definitions
#define log_out(S) if constexpr(S >= min_severity) jpu::logging::capture<S>(__FILE__, __LINE__)
#define log_v log_out(jpu::logging::severity::verbose)
#define log_d log_out(jpu::logging::severity::debug)
#define log_i log_out(jpu::logging::severity::info)
#define log_h log_out(jpu::logging::severity::hint)
#define log_w log_out(jpu::logging::severity::warning)
#define log_e log_out(jpu::logging::severity::error)
