#include "file.hpp"
#include <gfx/log.hpp>

#include "stb_vorbis.h"
#include <fstream>

namespace gfx {
struct wave_file
{
    struct wave_header
    {
        // riff header
        char     riff_tag[4];
        uint32_t size;
        char     wave_tag[4];

        // fmt section
        char     fmt_tag[4];
        uint32_t header_size;
        uint16_t format;
        uint16_t channels;
        uint32_t sample_rate;
        uint32_t bytes_per_second;
        uint16_t block_alignment;
        uint16_t bits_per_sample;

        // data section;
        char     data_tag[4];
        uint32_t data_size;
    } header;
    std::vector<uint8_t> data;
};

wave_file load_wav(const files::path& file)
{
    const auto file_size = files::file_size(file);
    if (file_size < sizeof(wave_file::wave_header)) return {};

    std::ifstream wav_file(file, std::ios::binary | std::ios::in);

    wave_file output;
    wav_file.read(reinterpret_cast<char*>(&output.header), sizeof(wave_file::wave_header));

    if (memcmp(output.header.riff_tag, "RIFF", 4) + memcmp(output.header.wave_tag, "WAVE", 4) + memcmp(output.header.fmt_tag, "fmt ", 4)
            + memcmp(output.header.data_tag, "data", 4)
        != 0)
        throw std::invalid_argument("Invalid Wave file!");

    output.data.resize(output.header.data_size);
    wav_file.read(reinterpret_cast<char*>(output.data.data()), output.header.data_size);
    return output;
}

audio_file::audio_file(const files::path& path) : file(path)
{
    const auto ext = path.extension().string();
    if (ext == ".wav")
    {
        auto wav = load_wav(file::path);

        int samples = (wav.header.data_size / (wav.header.bits_per_sample / 8)) / wav.header.channels;

        data_size   = wav.header.data_size;
        channels    = wav.header.channels;
        sample_rate = wav.header.sample_rate;
        bits        = wav.header.bits_per_sample;
        format      = 0x1100 + (wav.header.bits_per_sample >> 3) - 1 + wav.header.channels * 2 - 2;

        // data = std::move(wav.data);
        data.resize(samples * channels);

        const auto fill_data = [&](auto ty, auto uty) {
            using type          = decltype(ty);
            using unsigned_type = decltype(uty);

            for (int i = 0; i < samples; ++i)
            {
                unsigned_type* w = reinterpret_cast<unsigned_type*>(wav.data.data());
                data[i]          = static_cast<type>(*(w + i * wav.header.channels)) / static_cast<float>(std::numeric_limits<type>::max());

                if (channels == 2)
                {
                    data[i + samples] =
                        static_cast<type>(*(w + (i * wav.header.channels + 1))) / static_cast<float>(std::numeric_limits<type>::max());
                }
            }
        };
        const auto fill16 = [&] { fill_data(short(), unsigned short()); };
        const auto fill8  = [&] { fill_data(char(), unsigned char()); };

        if (bits == 8)
            fill8();
        else if (bits == 16)
            fill16();
        return;
    }
    if (ext == ".ogg")
    {
        short*     us;
        int        c, s;
        const auto data_len = stb_vorbis_decode_filename(file::path.string().c_str(), &c, &s, &us);
        data_size           = data_len * c * sizeof(short);
        channels            = c;
        sample_rate         = s;
        bits                = 16;
        format              = 0x1100 + (16 >> 3) - 1 + c * 2 - 2;

        const auto samples = data_len;
        data.resize(samples * channels);

        using type          = short;
        using unsigned_type = unsigned short;

        for (int i = 0; i < samples; ++i)
        {
            data[i] = static_cast<type>(*(us + i * channels)) / static_cast<float>(std::numeric_limits<type>::max());
            if (channels == 2)
            { data[i + samples] = static_cast<type>(*(us + (i * channels + 1))) / static_cast<float>(std::numeric_limits<type>::max()); }
        }

        return;
    }
    elog << "Unsupported file format: " << ext;
}

void* audio_file::bytes() const noexcept
{
    return _raw;
}
}    // namespace gfx
