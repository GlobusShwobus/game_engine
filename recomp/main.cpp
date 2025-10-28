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
#include "Scripts.h"

/*
MORE COLLISION RESOLUTIONS SECOND
QUADTREE THIRD
MAKE ENGINE CLASS TO MAKE HOW CHILI AND JAVIDX DO THE GAME THING
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

    
    struct SomeObjWithArea {
        rectF rect;
        vec2f vel;
        Color col;
    };

    SequenceM<SomeObjWithArea> myObjs;

    float farea = 10000.0f;

    for (int i = 0; i < 1000000; i++) {
        
        myObjs.element_create(
            rectF(rng.random_float(0, farea), rng.random_float(0, farea), rng.random_float(1,10), rng.random_float(1, 10)),
            vec2f(rng.random_float(1, 10), rng.random_float(1, 10)),
            Color(rng.random_int(1, 255), rng.random_int(1, 255), rng.random_int(1, 255), 255)
            );
    }
    

    camera.set_scale(1, 1);

    bool mouseHeld = false;
    Sprite sfont("C:/Users/ADMIN/Desktop/recomp/Fonts/font_32x3.png", renManager.get_renderer_ref());
    Font prettyText(sfont, 32,3);
    ////#################################################################################

    //main loop
    Stopwatch UPDATE_DELTA_TIMER;
    bool GAME_RUNNING = true;
    SDL_Event EVENT;

    while (GAME_RUNNING) {
        static float frameHold = 0;
        float dt = UPDATE_DELTA_TIMER.dt_float();
        frameHold += dt;
        if (frameHold < 0.008f) {
            continue;//skip the frame. a bit rigged atm, better to encapuselate in the IF
        }
       
        //CLEAR RENDERING
        renManager.renderer_clear();

        //LISTEN TO EVENTS
        float MouseX, MouseY;
        SDL_GetMouseState(&MouseX, &MouseY);

        while (SDL_PollEvent(&EVENT)) {
            if (EVENT.type == SDL_EVENT_QUIT) {
                GAME_RUNNING = false;
                continue;
            }
            script_handle_camera_mouse(EVENT, camera, vec2f(MouseX, MouseY));
        }


        //TEST CODE
        Stopwatch drawing1MILLIIONrects;

        rectF cameraSpace = camera.get_view_rect();
        for (auto& each : myObjs) {

            if (!cameraSpace.intersects_rect(each.rect)) {
                continue;
            }

            rectF cameraAdjusted = camera.world_to_screen(each.rect);
            renManager.fill_area_with(cameraAdjusted, each.col);
        }


        float drawTime = drawing1MILLIIONrects.dt_float();
        std::string print = "this took me dis long: " + std::to_string(drawTime);
        prettyText.draw_text(print, renManager.get_renderer_ref(), vec2i(0, 0));

        //###############################################################

        //PRESENT
        renManager.renderer_present();        
    }

    SDL_Quit();

    return 0;
}

