#pragma once

namespace game
{
    enum state
    {
        do_quit = -1,
        loop_menu = 0,
        loop_ingame = 1,
        prepare_splash = 2,
        loop_splash = 3,
    };

    bool splash();
    bool run_splash();
}