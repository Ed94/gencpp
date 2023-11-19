#if INTELLISENSE_DIRECTIVES
#include "win32.hpp"
#include "raylib/include/raylib.h"
#endif

int __stdcall WinMain( HINSTANCE instance, HINSTANCE prev_instance, char* commandline, int num_cmd_show)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    rl::init_window(screenWidth, screenHeight, "raylib [core] example - basic window");

    rl::set_target_fps(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!rl::window_should_close())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        rl::begin_drawing();

            rl::clear_background(RL_RAYWHITE);

            rl::draw_text("Congrats! You created your first window!", 190, 200, 20, RL_LIGHTGRAY);

        rl::end_drawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    rl::close_window();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

	return 0;
}

