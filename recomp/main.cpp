#include "Stopwatch.h"
#include "RenderManager.h"
#include "Configs.h"
#include <thread>

#include "Sprite.h"
#include "Animation.h"
#include "Font.h"
#include "Collision_Functions.h"

#include "SequenceM.h"
#include "NumberGenerator.h"
#include "Transform.h"
#include "Color.h"
#include "Camera.h"

/*
MORE COLLISION RESOLUTIONS SECOND
QUADTREE THIRD
ENTITY+ continue chilis lessons
?????

SERIALIZE FIRST (Too early, still not there yet)
*/

int main() {
    using namespace badEngine;
    
    //configs
    Configs windowConfig;

    try {
        windowConfig.init_from_file("SystemManagerConfig.json");
    }
    catch (const std::exception& excpt) {
        printf(excpt.what());
        return -1;
    }
    //initalize SDL system, can throw
    RenderManager renManager;
    try {
        renManager.init(windowConfig.get());
    }
    catch (const std::exception& excpt) {
        printf(excpt.what());
        return -1;
    }

    //TEST CODE
    NumberGenerator rng;
    Camera2D camera(960,540);

    /*
    struct SomeObjWithArea {
        rectF rect;
        vec2f vel;
        Color col;
    };

    SequenceM<SomeObjWithArea> myObjs;

    float farea = 100000.0f;

    for (int i = 0; i < 1000000; i++) {
        
        myObjs.element_create(
            rectF(rng.random_float(0, farea), rng.random_float(0, farea), rng.random_float(1,10), rng.random_float(1, 10)),
            vec2f(rng.random_float(1, 10), rng.random_float(1, 10)),
            Color(rng.random_int(1, 255), rng.random_int(1, 255), rng.random_int(1, 255), 255)
            );
    }
    */
    rectF testRectCameraRect = rectF(960 / 2, 540 / 2, 50, 50);
    Color testRectCameraCol = Color(255, 0, 0, 255);
    camera.set_scale(1, 1);

    bool mouseHeld = false;
    ////#################################################################################

    //main loop
    Stopwatch UPDATE_DELTA_TIMER;
    bool GAME_RUNNING = true;
    SDL_Event EVENT;

    while (GAME_RUNNING) {
        float dt = UPDATE_DELTA_TIMER.dt_float();
        //############################## VISUAL TEMP
        SDL_SetRenderDrawColor(renManager.get_renderer_ref(), 0, 0, 0, 255);
        //##############################
        renManager.renderer_clear();

        //LISTEN TO EVENTS

        float MouseX, MouseY;
        SDL_GetMouseState(&MouseX, &MouseY);

        while (SDL_PollEvent(&EVENT)) {
            if (EVENT.type == SDL_EVENT_QUIT) {
                GAME_RUNNING = false;
                continue;
            }

            if (EVENT.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                mouseHeld = true;
            }
            if (EVENT.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                mouseHeld = false;
            }

            if (EVENT.type == SDL_EVENT_KEY_DOWN) {
                SDL_Keycode key = EVENT.key.key;

                if (key == SDLK_A) {
                    camera.zoom_towards(1.1f, vec2f(MouseX, MouseY));
                }
                if (key == SDLK_S) {
                    camera.zoom_towards(0.9f, vec2f(MouseX, MouseY));
                }

                if (key == SDLK_D) {
                    camera.pan(vec2f(1, 0));
                }

                if (key == SDLK_F) {
                    camera.pan(vec2f(-1, 0));
                }
            }
        }

        if (mouseHeld) {
            camera.focus_on(vec2f(MouseX, MouseY));
        }

        //COLLISION/MOVEMENT (LATER ISOLATE INTO SOME SCRIPT FUNC)
        static float hold = 0;
        hold += dt;
        if (hold >= 0.008f) {
            


            hold = 0;
        }
        //#################################################################################

        rectF screenRect = camera.world_to_screen(testRectCameraRect);
        
        renManager.fill_area_with(screenRect, testRectCameraCol);
        renManager.renderer_present();

        //HANDLE TASKS BETWEEN FRAMES
        /*
        auto dt = frameTimer.MarkMilliSec();
        auto limit = frameTimer.getLimitMilliSec();
        if (dt < limit) {

            const auto spareTime = limit - dt;

            Stopwatch timeIntermediary;
            //DO SHIT HERE.. ALSO IF LOOPITY ACTION THEN THE TIMER SHOULD PROBABLY BE PART OF THE LOOP INSTEAD 

            //#################################################################################

            auto idt = timeIntermediary.MarkMilliSec();

            const auto remainingTime = spareTime - idt;
            
            if (remainingTime.count() > 0) {
                std::this_thread::sleep_for(remainingTime);
            }
        }
        */
        //#################################################################################
        
    }

    SDL_Quit();

    return 0;
}

