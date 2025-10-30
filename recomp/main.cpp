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
#include "QuadTree.h"

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
    float farea = 10000.0f;
    
    struct SomeObjWithArea {
        rectF rect;
        vec2f vel;
        Color col;
    };
    SequenceM<SomeObjWithArea> myObjsSeq;
    QuadTree<SomeObjWithArea> myObjsQuad(rectF(0,0, farea, farea));

    for (int i = 0; i < 1000000; i++) {
        
        rectF itemBox = rectF(rng.random_float(0, farea), rng.random_float(0, farea), rng.random_float(1, 10), rng.random_float(1, 10));
        SomeObjWithArea item = SomeObjWithArea(
            itemBox,
            vec2f(rng.random_float(1, 10), rng.random_float(1, 10)),
            Color(rng.random_int(1, 255), rng.random_int(1, 255), rng.random_int(1, 255), 255)
            );
        myObjsQuad.insert(item, itemBox);
        myObjsSeq.element_assign(item);
    }
    

    camera.set_scale(1, 1);

    bool mouseHeld = false;
    Sprite sfont("C:/Users/ADMIN/Desktop/recomp/Fonts/font_32x3.png", renManager.get_renderer_ref());
    Font prettyText(sfont, 32,3);
    bool seqORquad = false;
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

        while (SDL_PollEvent(&EVENT)) {
            if (EVENT.type == SDL_EVENT_QUIT) {
                GAME_RUNNING = false;
                continue;
            }

            //BS
            if (EVENT.type == SDL_EVENT_KEY_DOWN) {
                if (EVENT.key.key == SDLK_A) {
                    seqORquad = true;
                }
                if (EVENT.key.key == SDLK_S) {
                    seqORquad = false;
                }
            }
            ////////

            script_handle_camera_mouse(EVENT, camera);
        }


        //TEST CODE
        rectF cameraSpace = camera.get_view_rect();
        std::size_t DrawObjCount = 0;

        Stopwatch drawing1MILLIIONrects;
        if (seqORquad == false) {
         

            for (auto& each : myObjsSeq) {

                if (!cameraSpace.intersects_rect(each.rect)) {
                    continue;
                }

                rectF cameraAdjusted = camera.world_to_screen(each.rect);
                renManager.fill_area_with(cameraAdjusted, each.col);
                DrawObjCount++;
            
            }

        }
        else {

            for (const auto each : myObjsQuad.search(cameraSpace)) {


                rectF cameraAdjusted = camera.world_to_screen(each->rect);
                renManager.fill_area_with(cameraAdjusted, each->col);
                DrawObjCount++;


            }

        }
        float elapsedTime = drawing1MILLIIONrects.dt_float();

        //print out text
        std::string mode = (seqORquad == false) ? "pepega" : "quadtree";
        std::string print = "drawing mode: " + mode + " " + std::to_string(DrawObjCount) + "/1000000 -> time: " + std::to_string(elapsedTime);
        prettyText.draw_text(print, renManager.get_renderer_ref(), vec2i(0, 0));

        //###############################################################

        //PRESENT
        renManager.renderer_present();        
    }

    SDL_Quit();

    return 0;
}

