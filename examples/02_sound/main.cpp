#include <gfx/gfx.hpp>

#include <stdlib.h>

#include "soloud.h"
#include "soloud_file.h"
#include "soloud_speech.h"
#include "soloud_wav.h"

namespace sl = SoLoud;

struct audio : public sl::Soloud
{
    audio() { init(); }
    ~audio() { deinit(); }
};

// Entry point
int main(int argc, char* argv[])
{
    // Define a couple of variables
    audio           sound_core;
    gfx::audio_file piano("oggtest.ogg");
    sl::Wav         wave;
    wave.loadRawWave(piano.data.data(), piano.data.size(), piano.sample_rate, piano.channels, false, false);

    sl::Speech speech;
    speech.setText("Kaffee Kaffee Kaffee Kaffee Kaffee Kaffee Kaffee Kaffee Kaffee Kaffee Kaffee Kaffee ");

    sound_core.play(wave);
    sound_core.play(speech);

    while (true)
        ;
}
