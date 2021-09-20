#pragma once

#include <span>
#include <memory>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>
#include <filesystem>

namespace rosy::audio {
    enum class Channel {
        Mono = 1,
        Stereo = 2
    };

    enum class SourceType {
        Static,
        Stream
    };

    enum class AudioFormat : ALenum {
        Mono_8 = AL_FORMAT_MONO8,
        Mono_16 = AL_FORMAT_MONO16,
        Stereo_8 = AL_FORMAT_STEREO8,
        Stereo_16 = AL_FORMAT_STEREO16
    };

    struct Data {
        friend struct Audio;

        auto get_channel() const -> Channel {
            return m_channel;
        }
        auto get_bps() const -> uint32_t {
            return m_bps;
        }
        auto get_sample_rate() const -> uint32_t {
            return m_sample_rate;
        }
        auto bytes() const -> std::span<const char> {
            return m_bytes;
        }
        auto size() const -> size_t {
            return m_bytes.size();
        }
    private:
        std::vector<char> m_bytes;
        Channel m_channel;
        uint32_t m_bps;
        uint32_t m_sample_rate;
    };

    struct Source {
        friend struct Audio;

        explicit Source(const Data& data);
        ~Source();

        void play();
        void pause();
        void stop();
        void set_loop(bool loop);

    private:
        ALuint source;
        ALuint buffer;
    };

    struct Audio {
        friend struct Source;

        Audio();
        ~Audio();

        static auto load_data(const std::filesystem::path& path) -> std::shared_ptr<Data>;

        static auto get_format(Channel channel, size_t bps) -> AudioFormat {
            if (channel == Channel::Mono) {
                return bps == 8 ? AudioFormat::Mono_8 : AudioFormat ::Mono_16;
            }
            return bps == 8 ? AudioFormat::Stereo_8 : AudioFormat::Mono_16;
        }

        auto new_source(const std::filesystem::path& path, SourceType type) -> std::shared_ptr<Source>;

    private:
        ALCdevice *device;
        ALCcontext* context;
//        ALuint sources[32];
    };

    extern auto new_source(const std::filesystem::path& path, SourceType type) -> std::shared_ptr<Source>;
}