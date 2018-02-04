#include "audio.hpp"
#include <fstream>
#include <stb/stb_vorbis.h>

namespace res
{
    struct wave_file
    {
        struct wave_header
        {
            // riff header
            char riff_tag[4];
            uint32_t size;
            char wave_tag[4];

            // fmt section
            char fmt_tag[4];
            uint32_t header_size;
            uint16_t format;
            uint16_t channels;
            uint32_t sample_rate;
            uint32_t bytes_per_second;
            uint16_t block_alignment;
            uint16_t bits_per_sample;

            // data section;
            char data_tag[4];
            uint32_t data_size;
        } header;
        std::unique_ptr<void, free_deleter> data{nullptr};
    };

    wave_file load_wav(const filesystem::path& file)
    {
        const auto file_size = filesystem::file_size(file);
        if(file_size < sizeof(wave_file::wave_header))
            throw std::invalid_argument("File invalid!");

		std::ifstream wav_file(file, std::ios::binary | std::ios::in);
        
        wave_file output;
        wav_file.read(reinterpret_cast<char*>(&output.header), sizeof(wave_file::wave_header));

        if(memcmp(output.header.riff_tag, "RIFF", 4) +
            memcmp(output.header.wave_tag, "WAVE", 4) +
            memcmp(output.header.fmt_tag, "fmt ", 4) + 
            memcmp(output.header.data_tag, "data", 4) != 0)
            throw std::invalid_argument("Invalid Wave file!");
        
        output.data = std::unique_ptr<void, free_deleter>(malloc(output.header.data_size));
        wav_file.read(reinterpret_cast<char*>(output.data.get()), output.header.data_size);
        return output;
    }

    audio_file load_audio(const filesystem::path& file)
    {
        const auto ext = file.extension().string();
        audio_file output;
        if(ext == ".wav")
        {
            auto wav = load_wav(file);
            output.data = std::move(wav.data);
            output.data_size = wav.header.data_size;
            output.channels = wav.header.channels;
            output.sample_rate = wav.header.sample_rate;
            output.format = 0x1100 + (wav.header.bits_per_sample >> 3) - 1 + wav.header.channels*2-2;
            return output;
        }
        if(ext == ".ogg")
        {
            short* data;
            int c, s;
            const auto data_len = stb_vorbis_decode_filename(file.string().c_str(), &c, &s, &data);
            output.data_size = data_len * c * sizeof(short);
            output.data = std::unique_ptr<void, free_deleter>(data);
            output.channels = c;
            output.sample_rate = s;
            output.format = 0x1100 + (16 >> 3) - 1 + c*2-2;
            return output;
        }
        throw std::invalid_argument("Unsupported file format!");
    }
}
