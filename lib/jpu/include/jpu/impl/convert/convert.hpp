#pragma once

#include <array>
#include <sstream>
#include <vector>

namespace jpu
{
    std::wstring string_to_wstring(const std::string &input)
	{
		//make text unsigned (const char* to const unsigned char*) as it's needed for conversion.
		const unsigned char* ch_text = reinterpret_cast<const unsigned char*>(input.data());
		return std::wstring(&ch_text[0], &ch_text[input.size()]);
	}

    template<typename T>
	std::vector<T> string_to_vector(const std::string &input, char separator = ' ')
	{
		std::vector<T> output;
		std::stringstream stream(input);
		T current;

		while (stream >> current)
		{
			output.push_back(current);

			if (stream.peek() == separator)
				stream.ignore();
		}

		return output;
	}

	template<typename T, size_t Size>
	std::array<T, Size> string_to_array(const std::string &input, char separator = ' ')
	{
		std::array<T, Size> output;
		std::stringstream stream(input);

		size_t current = 0;
		while (current < Size && stream >> output[current++])
		{
			if (stream.peek() == separator)
				stream.ignore();
		}

		return output;
	}
}
