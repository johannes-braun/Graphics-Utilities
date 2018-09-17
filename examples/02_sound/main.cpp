#include <gfx/gfx.hpp>

#include <optional>

#include "soloud.h"
#include "soloud_file.h"
#include "soloud_speech.h"
#include "soloud_wav.h"

namespace gfx {
inline namespace v1 {
namespace sl = SoLoud;

class sound_wave;
class sound;
class audio_core
{
public:
	friend class sound;

    audio_core() { _core.init(); }
    ~audio_core() { _core.deinit(); }

    sound play(sound_wave& wave, float volume = -1, float pan = 0, bool start_paused = false);
	sound play_delta(std::chrono::nanoseconds time, sound_wave& wave, float volume = -1, float pan = 0);
	void  stop(sound_wave& wave);
	void  stop_all();

    void set_master_volume(float volume)
    {
		_core.setGlobalVolume(volume);
    }
private:
    sl::Soloud _core;
};

class sound_wave
{
public:
    friend class audio_core;

    sound_wave(audio_file file) : _file(file)
    {
        audio_file& opt_file = *_file;
        _wave.loadRawWave(opt_file.data.data(), opt_file.data.size(), opt_file.sample_rate, opt_file.channels, false, false);
    }

private:
    sl::Wav                   _wave;
    std::optional<audio_file> _file = std::nullopt;
};

class sound
{
public:
    friend class audio_core;
	static constexpr std::chrono::nanoseconds loop_never = std::chrono::nanoseconds{ -1 };

    sound() = default;

    void stop() const
    {
		assert(_core);
		_core->_core.stop(_id);
    }

    void set_volume(float volume) const
    {
		_core->_core.setVolume(_id, volume);
    }

    void loop(std::chrono::nanoseconds time = std::chrono::nanoseconds(0)) const
    {
		if (time.count() == -1)
			_core->_core.setLooping(_id, false);
		else
		{
			_core->_core.setLooping(_id, true);
			_core->_core.setLoopPoint(_id, time.count() / 1'000'000'000.0);
		}
    }

private:
    sound(u32 id, audio_core* core) : _id(id), _core(core) {}
    u32         _id   = 0;
    audio_core* _core = nullptr;
};

sound audio_core::play(sound_wave& wave, float volume, float pan, bool start_paused)
{
    return sound(_core.play(wave._wave, volume, pan, start_paused), this);
}
sound audio_core::play_delta(std::chrono::nanoseconds time, sound_wave& wave, float volume, float pan)
{
	return sound(_core.playClocked(time.count() / 1'000'000'000.0, wave._wave, volume, pan), this);
}

void audio_core::stop(sound_wave& wave)
{
	_core.stopAudioSource(wave._wave);
}

void audio_core::stop_all()
{
	_core.stopAll();
}

}    // namespace v1
}    // namespace gfx

int main(int argc, char* argv[])
{
	using namespace std::chrono_literals;

    gfx::audio_core background_music;
	background_music.set_master_volume(0.5f);

    gfx::audio_file music("oggtest.ogg");
    gfx::sound_wave music_wave(music);

	gfx::audio_file shoot("audio/shoot.ogg");
	gfx::sound_wave shoot_wave(shoot);

    auto sound = background_music.play(music_wave);
	sound.loop();


	std::chrono::steady_clock::time_point base;
	while (true)
	{
		auto tp = std::chrono::steady_clock::now();

		if (tp - base > 40ms)
		{
			background_music.play_delta(16ms, shoot_wave, -1.f, 1.f);
			background_music.play_delta(16ms, shoot_wave, -1.f, -1.f);
			base = tp;
		}

		while (std::chrono::steady_clock::now() - tp < 16ms);
	}
}
