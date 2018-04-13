//#pragma once
//
//#include <filesystem>
//#include <AL/al.h>
//
//namespace res
//{
//    namespace { struct free_deleter { void operator()(void* d) const { free(d); }}; }
//
//    struct audio_file
//    {
//        uint16_t channels;
//        uint32_t sample_rate;
//        size_t data_size;
//        ALuint format;
//        std::unique_ptr<void, free_deleter> data{nullptr};
//    };
//
//    namespace filesystem = std::experimental::filesystem;
//
//    audio_file load_audio(const filesystem::path& file);
//}
