#include <SDL2/SDL_audio.h>
#include <SDL2/SDL.h>
#include <module.h>
#include <memory>
#include "audio.h"

#include <fstream>

#include <AL/al.h>
#include <vector>
#include <cassert>
#include <filesystem>

#pragma pack(push, 1)
struct WAVHeader {
    char chunkId[4];
    uint32_t chunkSize;
    char format[4];
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

namespace {
    module<rosy::audio::Audio> instance;
}

namespace rosy::audio {
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

    Data Audio::loadData(const std::string_view filename) {
        std::ifstream in(std::string(filename), std::ios::binary);

        WAVHeader header{};
        WAVFormat format{};
        in.read(reinterpret_cast<char *>(&header), sizeof(WAVHeader));

        rosy::audio::Data data{};

        char chunk_id[4];
        uint32_t chunk_size;
        while (in) {
            in.read(chunk_id, 4);
            in.read(reinterpret_cast<char *>(&chunk_size), 4);
            std::vector<char> chunk_data(chunk_size);
            in.read(chunk_data.data(), chunk_size);

            if (memcmp(chunk_id, "fmt ", 4) == 0) {
                assert(sizeof(WAVFormat) == chunk_size);
                memcpy(&format, chunk_data.data(), sizeof(WAVFormat));
            }
            if (memcmp(chunk_id, "data", 4) == 0) {
                data.m_bytes = std::move(chunk_data);
                data.m_channel = Channel(format.num_channels);
                data.m_samplerate = format.sample_rate;
                data.m_bps = format.bits_per_sample;
                return std::move(data);
            }
        }

        return {};
    }

    Source::Source(const Data &data) {
        alGenSources(1, &source);

        alGenBuffers(1, &buffer);
        alBufferData(buffer, ALenum(Audio::GetFormat(data.getChannel(), data.getBPS())), data.bytes(), data.size(), data.getSampleRate());

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

    void Source::setLoop(const bool loop) {
        alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    }

    std::unique_ptr<Source> Audio::newSource(const std::string_view filename, const SourceType type) {
        return std::make_unique<Source>(Audio::loadData(filename));
    }

    std::unique_ptr<Source> newSource(const std::string_view filename, const SourceType type) {
        return instance->newSource(filename, type);
    }
}
