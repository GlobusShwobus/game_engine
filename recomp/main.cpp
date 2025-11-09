#include "Stopwatch.h"
#include "RenderManager.h"
#include "Configs.h"
#include <thread>

#include "Drawable.h"
#include "Collision_Functions.h"

#include "SequenceM.h"
#include "NumberGenerator.h"
#include "Transform.h"
#include "Color.h"
#include "Camera.h"
#include "Scripts.h"
#include "QuadTree.h"

#include <iostream>
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
    struct SomeObjWithArea {
        rectF rect;
        vec2f vel;
        Color col;
    };
    NumberGenerator rng;
    Camera2D camera(960, 540);
    float farea = 600;
    float fsearchsize = 50.0f;
    QuadTree<SomeObjWithArea> myObjsQuad(rectF(0, 0, farea, farea));

    for (int i = 0; i < 5000; i++) {

        rectF itemBox = rectF(rng.random_float(0, farea-10), rng.random_float(0, farea-10), rng.random_float(1, 10), rng.random_float(1, 10));
        SomeObjWithArea item = SomeObjWithArea(
            itemBox,
            vec2f(rng.random_float(1, 10), rng.random_float(1, 10)),
            Color(rng.random_int(1, 255), rng.random_int(1, 255), rng.random_int(1, 255), 255)
        );

        myObjsQuad.insert(std::move(item), itemBox);
    }


    camera.set_scale(1, 1);

    bool mouseHeld = false;
    std::shared_ptr<Texture> fontTexture = std::make_shared<Texture>("C:/Users/ADMIN/Desktop/recomp/Fonts/font_32x3.png", renManager.get_renderer_ref());
    std::unique_ptr<Font> font = std::make_unique<Font>(fontTexture, 32, 3);

    bool plzDeleteArea = false;
    bool plzPruneMe = false;
    int fuckingEventCounter = 0;
    renManager.enable_blend_mode();
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
            if (EVENT.key.key == SDLK_A) {
                fsearchsize += 10.0f;
            }
            if (EVENT.key.key == SDLK_S) {
                fsearchsize -= 10.0f;
            }
            if (EVENT.key.key == SDLK_P) {
                plzPruneMe = true;
            }
            if (EVENT.type == SDL_EVENT_MOUSE_BUTTON_DOWN && EVENT.button.button == SDL_BUTTON_LEFT) {
                plzDeleteArea = true;
            }
            if (EVENT.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                plzDeleteArea = false;
            }
            ///

            script_handle_camera_mouse(EVENT, camera);
        }


        //TEST CODE
        fsearchsize = std::clamp(fsearchsize, 10.0f, 500.0f);
        vec2f mouseScreenPos;
        SDL_GetMouseState(&mouseScreenPos.x, &mouseScreenPos.y);
        vec2f screenpos = camera.screen_to_world_point(mouseScreenPos);
        rectF rectAroundMouse = rectF(
            screenpos.x - fsearchsize / 2,
            screenpos.y - fsearchsize / 2,
            fsearchsize, fsearchsize
        );


        rectF cameraSpace = camera.get_view_rect();
        std::size_t DrawObjCount = 0;

        Stopwatch drawing1MILLIIONrects;
        for (const auto& each : myObjsQuad.search_area(cameraSpace)) {


            rectF cameraAdjusted = camera.world_to_screen(myObjsQuad[each].rect);//invalidtaion
            renManager.fill_area_with(cameraAdjusted, myObjsQuad[each].col);
            DrawObjCount++;


        }
        rectF camGirlAdjusted = camera.world_to_screen(rectAroundMouse);
        Color mouseCol = Colors::Magenta;
        mouseCol.set_alpha(125u);
        renManager.fill_area_with(camGirlAdjusted, mouseCol);
        
        if (plzDeleteArea) {
            myObjsQuad.remove_area(rectAroundMouse);
        }
        if (plzPruneMe) {
            if (fuckingEventCounter == 0) {
                std::size_t branchesBefore = myObjsQuad.branch_count();
                myObjsQuad.remove_dead_cells();
                std::size_t branchesAfter = myObjsQuad.branch_count();

                std::cout << "branches before: " << branchesBefore << '\n' << "branches after: " << branchesAfter;
                fuckingEventCounter++;
            }
            plzPruneMe = false;
        }

        float elapsedTime = drawing1MILLIIONrects.dt_float();

        //print out text
        std::string print = "quadtree: " + std::to_string(DrawObjCount) + "/" + std::to_string(myObjsQuad.size())+ "->time: " + std::to_string(elapsedTime);
        font->set_text(print);
        font->draw(renManager.get_renderer_ref(), rectF(0, 0, 32,32));

        //###############################################################

        //PRESENT
        renManager.renderer_present();
    }

    SDL_Quit();

    return 0;
}

