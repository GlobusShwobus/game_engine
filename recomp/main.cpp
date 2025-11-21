#include "Stopwatch.h"
#include "RenderManager.h"
#include "Configs.h"
#include <thread>

#include "Sprite.h"

#include "SequenceM.h"
#include "NumberGenerator.h"
#include "Transform.h"
#include "Color.h"
#include "Camera.h"
#include "Scripts.h"
#include "QuadTree.h"

#include <iostream>
/*
* 
FIRST DO A OVERLAP CHECK
THEN GET COLLIDERS FROM THE QUADTREE

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
    //init SDL system, can throw
    RenderManager renManager;
    try {
        renManager.init(windowConfig.get());
    }
    catch (const std::exception& excpt) {
        printf(excpt.what());
        return -1;
    }
    ////// TEST CODE
    struct SomeObjWithArea {
        rectF rect;
        vec2f vel;
        Color col;
    };
    NumberGenerator rng;
    float windowWidth = 960;
    float windowHeight = 540;
    const rectI window(0, 0, windowWidth, windowHeight);

    QuadTree<SomeObjWithArea> myObjsQuad(rectF(0, 0, windowWidth, windowHeight));

    for (int i = 0; i < 5000; i++) {
        //ALSO TEST OUT WITH SMALLER RANGES TO TEST IF contains() is worth it for collision
        float boxWidth = rng.random_float(1, 10);
        float boxHeight = rng.random_float(1, 10);
    
        rectF box = rectF(rng.random_float(0, windowWidth - boxWidth), rng.random_float(0, windowHeight - boxHeight), boxWidth, boxHeight);
        SomeObjWithArea item = SomeObjWithArea(
            box,
            vec2f(rng.random_float(-1, 1), rng.random_float(-1, 1)),
            Color(rng.random_int(1, 255), rng.random_int(1, 255), rng.random_int(1, 255), 255)
        );
    
        myObjsQuad.insert(std::move(item), box);
    }

    Camera2D camera(960, 540);
    camera.set_scale(1, 1);

    float mouseBoxSize = 50.0f;
    bool mouseHeld = false;

    std::unique_ptr<Font> font = std::make_unique<Font>(32, 3, "C:/Users/ADMIN/Desktop/recomp/Fonts/font_32x3.png", renManager.get_renderer());

    bool plzDeleteArea = false;
    bool plzPruneMe = false;
    renManager.enable_blend_mode();
    //////#######################################################

    //main loop
    Stopwatch UPDATE_DELTA_TIMER;
    bool GAME_RUNNING = true;
    SDL_Event EVENT;

    while (GAME_RUNNING) {
        static float frameHold = 0;
        float dt = UPDATE_DELTA_TIMER.dt_float();
        frameHold += dt;
        if (frameHold < 0.008f) {
            continue;//skip the frame. a bit rigged atm, better to encapsulate in the IF
        }
        frameHold = 0;//fucking oops...

        //CLEAR RENDERING
        renManager.renderer_clear();

        //LISTEN TO EVENTS

        while (SDL_PollEvent(&EVENT)) {
            if (EVENT.type == SDL_EVENT_QUIT) {
                GAME_RUNNING = false;
                continue;
            }

            ////// TEST CODE
            if (EVENT.key.key == SDLK_A) {
                mouseBoxSize += 10.0f;
            }
            if (EVENT.key.key == SDLK_S) {
                mouseBoxSize -= 10.0f;
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
            script_handle_camera_mouse(EVENT, camera);
            ////////######################################################
        }

        //////TEST CODE
        vec2f mouseScreenPos;
        SDL_GetMouseState(&mouseScreenPos.x, &mouseScreenPos.y);
        vec2f screenPos = camera.screen_to_world_point(mouseScreenPos);
        rectF rectAroundMouse = rectF(
            screenPos.x - mouseBoxSize / 2,
            screenPos.y - mouseBoxSize / 2,
            mouseBoxSize, mouseBoxSize
        );
        
        rectF cameraSpace = camera.get_view_rect();
        std::size_t objectsCount = 0;
        Stopwatch timer;
        //draw
        auto foundObjects = myObjsQuad.search_area(cameraSpace);
        for (const auto& each : foundObjects) {
            //first draw
            auto& object = myObjsQuad[each];
            rectF cameraAdjusted = camera.world_to_screen(object.rect);
            renManager.fill_area_with(cameraAdjusted, object.col);
            objectsCount++;
        }
        //apply move
        for (auto& each : foundObjects) {
            auto& object = myObjsQuad[each];
            rectF newPos(object.rect.x + object.vel.x, object.rect.y + object.vel.y, object.rect.w, object.rect.h);
        
          //  myObjsQuad.relocate(each, newPos);
           // object.rect = newPos;
        }

        for (std::size_t i = 0; i < myObjsQuad.size();++i) {
            auto& obj = myObjsQuad[i];
            rectF newBox = obj.rect;
            vec2f newVel = obj.vel;
            bool ifReflect = false;
            if (newBox.x < 0) {
                newBox.x = 0;
                newVel.x = -newVel.x;
                ifReflect = true;
            }
            else if (newBox.x + newBox.w >= 960) {
                newBox.x = 960 - newBox.w;
                newVel.x = -newVel.x;
                ifReflect = true;
            }

            if (newBox.y < 0) {
                newBox.y = 0;
                newVel.y = -newVel.y;
                ifReflect = true;
            }
            else if (newBox.y + newBox.h >= 540) {
                newBox.y = 540 - newBox.h;
                newVel.y = -newVel.y;
                ifReflect = true;
            }

            if (ifReflect) {
               // myObjsQuad.relocate(i, newBox);
               // obj.rect = newBox;
               // obj.vel = newVel;
            }

        }
        auto colliders = myObjsQuad.search_collisions();
        
        float elapsedTime = timer.dt_float();
        
        static int removeafafafafaf = 0;
        if (removeafafafafaf == 10) {
            myObjsQuad.remove_dead_cells();
            removeafafafafaf = 0;
        }
        removeafafafafaf++;
        //draw mouse
        rectF camGirlAdjusted = camera.world_to_screen(rectAroundMouse);
        Color mouseCol = Colors::Magenta;
        mouseCol.set_alpha(125u);
        renManager.fill_area_with(camGirlAdjusted, mouseCol);
        
        
        //draw text
        std::string print = "Object Count: " + std::to_string(objectsCount) + "/" + std::to_string(myObjsQuad.size()) + "\ntime: " + std::to_string(elapsedTime) + "\nnodes: "+std::to_string(myObjsQuad.branch_count());
        font->set_text(print);
        font->draw(renManager.get_renderer(), vec2f(0, 0));
        //////########################################################
        
        //PRESENT
        renManager.renderer_present();
    }
    SDL_Quit();

    return 0;
}

