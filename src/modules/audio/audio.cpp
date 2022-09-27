#include <memory>

#include "audio.hpp"

#include <array>
#include <vector>
#include <fstream>
#include <filesystem>

#pragma pack(push, 1)
struct WAVHeader {
    std::array<char, 4> chunk_id;
    uint32_t chunk_size;
    std::array<char, 4> format;
};
struct WAVFormat {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_race;
    uint16_t block_align;
    uint16_t bits_per_sample;

};
#pragma pack(pop)

namespace rosy::audio {
    namespace {
        Arc<Audio> s_audio;
    }

    Audio::Audio() {
        device = alcOpenDevice("OpenAL Soft");
        context = alcCreateContext(device, nullptr);
        alcMakeContextCurrent(context);

//        alGenSources(32, sources);
    }

    Audio::~Audio() {
//        alDeleteSources(32, sources);

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);
    }

    auto Audio::load_data(const std::filesystem::path& path) -> std::shared_ptr<Data> {
        std::ifstream in(path, std::ios::binary);

        WAVHeader header{};
        WAVFormat format{};
        in.read(reinterpret_cast<char *>(&header), sizeof(WAVHeader));

        std::array<char, 4> chunk_id{};
        uint32_t chunk_size;
        while (in) {
            in.read(chunk_id.data(), 4);
            in.read(reinterpret_cast<char *>(&chunk_size), 4);
            std::vector<char> chunk_data(chunk_size);
            in.read(chunk_data.data(), chunk_size);

            if (memcmp(chunk_id.data(), "fmt ", 4) == 0) {
                memcpy(&format, chunk_data.data(), sizeof(WAVFormat));
            }
            if (memcmp(chunk_id.data(), "data", 4) == 0) {
                Data data{};
                data.m_bytes = std::move(chunk_data);
                data.m_channel = Channel(format.num_channels);
                data.m_sample_rate = format.sample_rate;
                data.m_bps = format.bits_per_sample;

                return std::make_shared<Data>(std::move(data));
            }
        }

        return {};
    }

    Source::Source(const Data &data) {
        alGenSources(1, &source);
        alGenBuffers(1, &buffer);
        alBufferData(buffer, static_cast<ALenum>(Audio::get_format(data.get_channel(), data.get_bps())), data.bytes().data(), data.size(), data.get_sample_rate());
        alSourcei(source, AL_BUFFER, buffer);
    }

    Source::~Source() {
        stop();
        alDeleteSources(1, &source);
        alDeleteBuffers(1, &buffer);
    }

    void Source::play() {
        alSourcePlay(source);
    }

    void Source::pause() {
        alSourcePause(source);
    }

    void Source::stop() {
        alSourceStop(source);
    }

    void Source::set_loop(bool loop) {
        alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    }

    void Audio::init() {
        s_audio = Arc<Audio>::alloc();
    }

    auto Audio::get() -> Arc<Audio> const& {
        if (!s_audio) {
            throw std::runtime_error("Audio module is not initialized");
        }
        return s_audio;
    }

    auto Audio::new_source(const std::filesystem::path& path, SourceType type) -> std::shared_ptr<Source> {
        auto data = Audio::load_data(path);
        return std::make_shared<Source>(*data);
    }

    auto new_source(const std::filesystem::path& path, SourceType type) -> std::shared_ptr<Source> {
        return Audio::get()->new_source(path, type);
    }
}
