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
using namespace badEngine;
struct SomeObjWithArea {
    rectF rect;
    vec2f vel;
    Color col;
};
void TEST_REMOVE_INSERT_QUADTREE(QuadTree<SomeObjWithArea>& muhquadtree,
    NumberGenerator& rng, float areaborder)
{
    const std::size_t current_size = muhquadtree.size();
    if (current_size == 0) {
        // nothing to remove, just create a few
        int create_amount = rng.random_int(1, 10);
        for (int i = 0; i < create_amount; ++i) {
            rectF itemBox = rectF(rng.random_float(0, areaborder),
                rng.random_float(0, areaborder),
                rng.random_float(1, 10),
                rng.random_float(1, 10));
            SomeObjWithArea item = SomeObjWithArea(
                itemBox,
                vec2f(rng.random_float(1, 10), rng.random_float(1, 10)),
                Color(rng.random_int(1, 255), rng.random_int(1, 255),
                    rng.random_int(1, 255), 255));
            muhquadtree.insert(std::move(item), itemBox);
        }
        return;
    }

    // first randomly remove
    const int random_remove_amount = rng.random_int(0, static_cast<int>(current_size));

    for (int i = 0; i < random_remove_amount && muhquadtree.size() > 0; ++i) {
        std::size_t remove_index = rng.random_int(0, muhquadtree.size() - 1);
        muhquadtree.remove(remove_index);
    }

    // then add back
    const int random_create_amount = random_remove_amount + rng.random_int(0, random_remove_amount);
    for (int i = 0; i < random_create_amount; ++i) {
        rectF itemBox = rectF(rng.random_float(0, areaborder),
            rng.random_float(0, areaborder),
            rng.random_float(1, 10),
            rng.random_float(1, 10));
        SomeObjWithArea item = SomeObjWithArea(
            itemBox,
            vec2f(rng.random_float(1, 10), rng.random_float(1, 10)),
            Color(rng.random_int(1, 255), rng.random_int(1, 255),
                rng.random_int(1, 255), 255));
        muhquadtree.insert(std::move(item), itemBox);
    }
}
int main() {

    //using namespace badEngine;

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
    Camera2D camera(960, 540);
    float farea = 600.f;
    QuadTree<SomeObjWithArea> myObjsQuad(rectF(0, 0, farea, farea));

    for (int i = 0; i < 500; i++) {

        rectF itemBox = rectF(rng.random_float(0, farea), rng.random_float(0, farea), rng.random_float(1, 10), rng.random_float(1, 10));
        SomeObjWithArea item = SomeObjWithArea(
            itemBox,
            vec2f(rng.random_float(1, 10), rng.random_float(1, 10)),
            Color(rng.random_int(1, 255), rng.random_int(1, 255), rng.random_int(1, 255), 255)
        );
        myObjsQuad.insert(std::move(item), std::move(itemBox));
    }


    camera.set_scale(1, 1);

    bool mouseHeld = false;
    Sprite sfont("C:/Users/ADMIN/Desktop/recomp/Fonts/font_32x3.png", renManager.get_renderer_ref());
    Font prettyText(sfont, 32, 3);
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

            script_handle_camera_mouse(EVENT, camera);
        }


        //TEST CODE
        rectF cameraSpace = camera.get_view_rect();
        std::size_t DrawObjCount = 0;

        Stopwatch drawing1MILLIIONrects;

        for (const auto& each : myObjsQuad.search_index(cameraSpace)) {


            rectF cameraAdjusted = camera.world_to_screen(myObjsQuad[each].rect);//invalidtaion
            renManager.fill_area_with(cameraAdjusted, myObjsQuad[each].col);
            DrawObjCount++;


        }


        static float add_remove_hold = 0;
        add_remove_hold += dt;
        if (add_remove_hold >= 2.0f) {//2 second
            printf("hello world?\n");
            TEST_REMOVE_INSERT_QUADTREE(myObjsQuad, rng, farea);
            add_remove_hold = 0;
        }


        float elapsedTime = drawing1MILLIIONrects.dt_float();

        //print out text
        std::string print = "quadtree: " + std::to_string(DrawObjCount) + "/1000000 -> time: " + std::to_string(elapsedTime);
        prettyText.draw_text(print, renManager.get_renderer_ref(), vec2i(0, 0));

        //###############################################################

        //PRESENT
        renManager.renderer_present();
    }

    SDL_Quit();

    return 0;
}

