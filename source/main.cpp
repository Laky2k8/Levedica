#include <grrlib.h>
#include <gccore.h>
#include <fat.h>
#include <wiiuse/wpad.h>
#include <gccore.h>
#include <ogc/lwp_watchdog.h>   // Needed for gettime and ticks_to_millisecs
#include <stdlib.h>
#include <unistd.h>             // Needed for sleep() and chdir()
#include <dirent.h>             // Needed for DIR, opendir(), closedir()
#include <print>
#include <string>

#include "defs.h"

#include "BMfont5_png.h"
#include "cursor_png.h"

#include "map/state.hpp"
#include "map/country.hpp"

#include "maputils.h"
#include "csv.h"

//#include "imagemaniptest.h"


#define CURSOR_HOTSPOT_X 5.0f
#define CURSOR_HOTSPOT_Y 46.0f

vec2 map_pos(-150, -75);
vec2 map_scale(1, 1);

static u8 CalculateFrameRate(void);
std::string read_file(const char *path);

bool can_open_root_fs()
{
    DIR *root = opendir("/");
    if(root)
    {
        closedir(root);
        return true;
    }
    return false;
}

void die(const char *msg)
{
    perror(msg);
    sleep(5);
    fatUnmount(0);
    exit(0);
}

int main() {
    u8 FPS = 0;

    ir_t pointer1; // Wii Remote Pointer 1
    static f32 cursor_x = 320.0f;
    static f32 cursor_y = 240.0f;

    // Debugginh to Dolphin
    SYS_STDIO_Report(true);

    std::vector<uint16_t> selectedStates;


    /*const guVector triangle[] = {{400,200,0.0f}, {500,400,0.0f}, {300,400,0.0f}};
    const u32 trianglecolor[] = {GRRLIB_GREEN, GRRLIB_RED, GRRLIB_BLUE};*/

    GRRLIB_Init();

    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(WPAD_CHAN_0, 640 + 64, 480 - 48);

    GRRLIB_texImg *tex_BMfont5 = GRRLIB_LoadTexture(BMfont5_png);

    GRRLIB_texImg *tex_pointer = GRRLIB_LoadTexture(cursor_png);

    GRRLIB_InitTileSet(tex_BMfont5, 8, 16, 0);
    //GRRLIB_InitTileSet(tex_pointer, 64, 75, 0);

    // Init SD card
    if(!fatInitDefault()) die("Unable to init FAT filesystem, exiting. \n");
    if(!can_open_root_fs()) die("Unable to open root filesystem, exiting.\n");
    if(chdir("/")) die("Could not change to root directory, exiting.\n");

    std::string map_definitions = read_file("sd:/map.csv");

    std::vector<State> states;
    try
    {
        states = parse_state_csv(map_definitions);
    }
    catch(const std::exception& e)
    {
        die(e.what());
    }

    printf((states.at(0).getID() + " - Name: " + states.at(0).getName()).c_str());
    

    //printf("CSV Contents: %s\n", map_definitions.c_str());

    // Map image
    GRRLIB_texImg *tex_map_raw = GRRLIB_LoadTextureFromFile("sd:/map.png");
    if(tex_map_raw == NULL)
    {
        die("Could not load the map image!");
    }
    else
    {
        printf("Map image loaded.");
    }

    GRRLIB_texImg *tex_map_image = prerenderMap(tex_map_raw, GRRLIB_GRAY, GRRLIB_RED, GRRLIB_WHITE, GRRLIB_BLACK, 1, selectedStates);


    while(SYS_MainLoop()) 
    {
        WPAD_ScanPads();

        // If [HOME] was pressed on the first Wiimote, break out of the loop
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)  break;

        const u32 buttonsDown = WPAD_ButtonsDown(0);
        const u32 buttonsHeld = WPAD_ButtonsHeld(0);

        WPAD_IR(WPAD_CHAN_0, &pointer1);

        GRRLIB_FillScreen(GRRLIB_NAVY);    // Clear the screen
        WPAD_Rumble(WPAD_CHAN_0, 0);

        GRRLIB_DrawImg((int)map_pos.x, (int)map_pos.y, tex_map_image, 0, map_scale.x, map_scale.y, GRRLIB_WHITE);

        GRRLIB_Printf(5, 25, tex_BMfont5, GRRLIB_WHITE, 1, (std::string(TITLE) + " " + std::string(VERSION_NUM)).c_str());


        GRRLIB_Printf(500, 27, tex_BMfont5, GRRLIB_WHITE, 1, "Current FPS: %d", FPS);

        //GRRLIB_Printf(5, 100, tex_BMfont5, GRRLIB_WHITE, 1, "CSV Contents: %s", map_definitions.c_str());

        if(buttonsDown & WPAD_BUTTON_HOME) {
            break;
        }
        if(buttonsHeld & WPAD_BUTTON_LEFT) {
            GRRLIB_Printf(475, 75, tex_BMfont5, GRRLIB_WHITE, 1, "LEFT");
        }
        if(buttonsHeld & WPAD_BUTTON_RIGHT) {
            GRRLIB_Printf(525, 75, tex_BMfont5, GRRLIB_WHITE, 1, "RIGHT");
        }
        if(buttonsHeld & WPAD_BUTTON_UP) {
            GRRLIB_Printf(500, 50, tex_BMfont5, GRRLIB_WHITE, 1, "UP");
        }
        if(buttonsHeld & WPAD_BUTTON_DOWN) {
            GRRLIB_Printf(500, 100, tex_BMfont5, GRRLIB_WHITE, 1, "DOWN");
        }
        if(buttonsDown & WPAD_BUTTON_MINUS) {
            GRRLIB_Printf(500, 150, tex_BMfont5, GRRLIB_WHITE, 1, "MINUS");
        }
        if(buttonsDown & WPAD_BUTTON_PLUS) {
            GRRLIB_Printf(500, 175, tex_BMfont5, GRRLIB_WHITE, 1, "PLUS");
        }
        if(buttonsHeld & WPAD_BUTTON_1 && buttonsHeld & WPAD_BUTTON_2) {
            WPAD_Rumble(WPAD_CHAN_0, 1); // Rumble on
            GRRLIB_ScrShot("sd:/grrlib.png");
            WPAD_Rumble(WPAD_CHAN_0, 0); // Rumble off
        }

        if(pointer1.valid)
        {
            cursor_x = pointer1.x - (tex_pointer->w / 2.0f);
            cursor_y = pointer1.y + CURSOR_HOTSPOT_Y - (tex_pointer->h / 2.0f);
            GRRLIB_DrawImg(cursor_x, cursor_y, tex_pointer, 0, 1, 1, GRRLIB_WHITE);

            GRRLIB_Plot(pointer1.x,     pointer1.y,     GRRLIB_YELLOW);
            GRRLIB_Plot(pointer1.x + 1, pointer1.y,     GRRLIB_YELLOW);
            GRRLIB_Plot(pointer1.x,     pointer1.y + 1, GRRLIB_YELLOW);
            GRRLIB_Plot(pointer1.x + 1, pointer1.y + 1, GRRLIB_YELLOW);

        }

        if(buttonsHeld & WPAD_BUTTON_A)
        {
            vec2 click_pos = screen_pos_to_map(vec2(pointer1.x, pointer1.y), map_pos, map_scale, tex_map_raw->w, tex_map_raw->h);

            u32 pixel_color;
            uint16_t state_id;
            State selected_state;
            if (click_pos.x >= 0 && click_pos.y >= 0)
            {
                pixel_color = GRRLIB_GetPixelFromtexImg((int)click_pos.x, (int)click_pos.y, tex_map_raw);

                u8 r = (pixel_color >> 24) & 0xFF;
                u8 g = (pixel_color >> 16) & 0xFF;
                u8 b = (pixel_color >> 8)  & 0xFF;

                state_id = rgb_to_16bit(r, g, b);

                // find the state with the corresponding ID
                auto it = std::find_if(states.begin(), states.end(), [state_id](const State& state) 
                {
                        return state.getID() == state_id;
                });

                if (it != states.end()) 
                {
                    selected_state = *it;
                }

                // temp state painting, TODO replace with actual country system lmao
                auto state_it = std::find(selectedStates.begin(), selectedStates.end(), state_id);
                if (state_it == selectedStates.end()) 
                {
                    selectedStates.push_back(state_id);

                    // rerender
                    GX_DrawDone();
                    GRRLIB_FreeTexture(tex_map_image);
                    tex_map_image = prerenderMap(tex_map_raw, GRRLIB_GRAY, GRRLIB_RED, GRRLIB_WHITE, GRRLIB_BLACK, 1, selectedStates);
                } 
            }
            

            // Gray rectangle in bottom left corner
            GRRLIB_Rectangle(0, 380, 400, 100, GRRLIB_GRAY, true);

            GRRLIB_Printf(5, 400, tex_BMfont5, GRRLIB_BLACK, 1, "X: %.2f, Y: %.2f", cursor_x, cursor_y);
            GRRLIB_Printf(5, 420, tex_BMfont5, GRRLIB_BLACK, 1, "Position on map: X: %.2f, Y: %.2f", click_pos.x, click_pos.y);
            GRRLIB_Printf(5, 440, tex_BMfont5, GRRLIB_BLACK, 1, "Clicked pixel color: %u", pixel_color);
            GRRLIB_Printf(5, 460, tex_BMfont5, GRRLIB_BLACK, 1, "State ID: %d, Name: %s", state_id, selected_state.getName().c_str());
        }


        GRRLIB_Render();
        FPS = CalculateFrameRate();
    }

    // Free some textures
    GRRLIB_FreeTexture(tex_BMfont5);
    GRRLIB_FreeTexture(tex_pointer);
    GRRLIB_FreeTexture(tex_map_image);
    GRRLIB_FreeTexture(tex_map_raw);
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
    return 0;
}

/**
 * This function calculates the number of frames we render each second.
 * @return The number of frames per second.
 */
static u8 CalculateFrameRate(void) {
    static u8 frameCount = 0;
    static u32 lastTime;
    static u8 FPS = 0;
    const u32 currentTime = ticks_to_millisecs(gettime());

    frameCount++;
    if(currentTime - lastTime > 1000) {
        lastTime = currentTime;
        FPS = frameCount;
        frameCount = 0;
    }
    return FPS;
}

std::string read_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) return "";

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::string contents(size, '\0');
    fread(&contents[0], 1, size, file);
    fclose(file);

    return contents;
}