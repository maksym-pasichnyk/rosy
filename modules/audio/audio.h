#pragma once

#include <string_view>

#include <AL/al.h>
#include <AL/alc.h>

#include <vector>

namespace rosy::audio {
    enum class Channel {
        Mono = 1,
        Stereo = 2
    };

    struct Data {
        friend struct Audio;

        inline Channel getChannel() const {
            return m_channel;
        }
        inline uint32_t getBPS() const {
            return m_bps;
        }
        inline uint32_t getSampleRate() const {
            return m_samplerate;
        }
        inline const char* bytes() const {
            return m_bytes.data();
        }
        inline size_t size() const {
            return m_bytes.size();
        }
    private:
        std::vector<char> m_bytes;
        Channel m_channel;
        uint32_t m_bps;
        uint32_t m_samplerate;
    };

    enum class SourceType {
//        Static,
        Stream
    };
    struct Source {
        friend struct Audio;

        explicit Source(const Data& data);
        ~Source();

        void play();
        void pause();
        void stop();
        void setLoop(const bool loop);

    private:
        ALuint source;
        ALuint buffer;
    };

    enum class AudioFormat : ALenum {
        Mono_8 = AL_FORMAT_MONO8,
        Mono_16 = AL_FORMAT_MONO16,
        Stereo_8 = AL_FORMAT_STEREO8,
        Stereo_16 = AL_FORMAT_STEREO16
    };

    struct Audio {
        friend struct Source;

        Audio();
        ~Audio();

        static Data loadData(const std::string_view filename);

        static AudioFormat GetFormat(const Channel channel, const size_t bps) {
            if (channel == Channel::Mono) {
                return AudioFormat(bps == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16);
            }
            return AudioFormat(bps == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16);
        }

        std::unique_ptr<Source> newSource(const std::string_view filename, const SourceType type);

    private:
        ALCdevice *device;
        ALCcontext* context;
//        ALuint sources[32];
    };

    std::unique_ptr<Source> newSource(const std::string_view filename, const SourceType type);
}