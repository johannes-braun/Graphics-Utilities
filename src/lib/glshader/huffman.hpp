#pragma once

#include <array>
#include <vector>
#include <queue>
#include <sstream>

namespace huffman
{
    std::basic_string<uint8_t> encode(const std::basic_string<uint8_t>& in);
    std::basic_string<uint8_t> encode(const std::vector<uint8_t>& in);
    std::basic_string<uint8_t> encode(const uint8_t* in, size_t in_length);
    std::basic_string<uint8_t> decode(const std::basic_string<uint8_t>& in);
}