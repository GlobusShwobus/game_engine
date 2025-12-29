#include "Stopwatch.h"
#include "GraphicsSys.h"
#include "Configs.h"
#include <thread>
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#include <conio.h>

#include "Sprite.h"

#include "SequenceM.h"
#include "NumberGenerator.h"
#include "Transform.h"
#include "Color.h"
#include "Camera.h"
#include "Scripts.h"
#include "BVH.h"

#include <iostream>
/*

1) do a hash map data structure for reasons
2) get back to quad tree reenvigorated
3) make engine class for on user create and run loop sepparation
4) entities??? probably not

*/

int main() {

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    {
        using namespace badEngine;

        //configs
        Configs windowConfig("SystemManagerConfig.json");

        //init SDL system, can throw
        GraphicsSys renManager(windowConfig.get());

        ////// TEST CODE
        struct MyTester {
            rectF rect;
            Color col;
            MyTester(const rectF& r, Color col) :rect(r), col(col) {}
        };
        NumberGenerator rng;

        const float windowWidth = 960.f;
        const float windowHeight = 540.f;
        const rectI window(0, 0, windowWidth, windowHeight);
        SequenceM<std::unique_ptr<MyTester>> myObjsStore;
        myObjsStore.set_capacity(100);

        BVHTree<MyTester> tree(myObjsStore.capacity());

        Stopwatch insertionTimeVector;
        const float spacing = 10;
        const float width = 10;
        const float height = 10;
        float boxWidth = width + spacing;
        float boxHeight = height + spacing;

        for (int i = 0; i < 100; ++i) {
           //CREATING A WORST CASE SCENARIO OF A LINKED LIST BVH
            //rectF rect(
            //    i* boxWidth, // x-position increases linearly
            //    0,            // all boxes at the same y
            //    boxWidth,
            //    boxHeight
            //);

            //CREATING A RANDOM CASE
            rectF rect(
                rng.random_float(0, windowWidth),
                rng.random_float(0, windowHeight),
                rng.random_float(1, 100),
                rng.random_float(1, 100)
            );

            myObjsStore.emplace_back(std::make_unique<MyTester>(rect, Colors::Green));
        }

        std::size_t vectorInsertionTime = insertionTimeVector.dt_nanosec();

        Stopwatch insertionTimeBVH;
        for (auto it = myObjsStore.begin(); it != myObjsStore.end(); ++it) {
            MyTester* p = it->get();
            auto id = tree.create_proxy(p->rect, p);
        }
        std::size_t BVHInsertionTime = insertionTimeBVH.dt_nanosec();

        std::cout << "insertion into vector: " << vectorInsertionTime << "\ninserting into quadtree: " << BVHInsertionTime << "\ntotal: " << vectorInsertionTime + BVHInsertionTime << "\n";
        

        renManager.set_render_blend_mode(SDL_BLENDMODE_BLEND);


        long double time = 0;
        std::size_t frames = 0;
        const std::size_t frame_target = 2000;
        Camera2D camera(windowWidth, windowHeight);
        //////#######################################################

        //main loop
        bool GAME_RUNNING = true;
        SDL_Event EVENT;
        //this whole main loop is badly bad but engine class in the future so fuck it for now
        while (GAME_RUNNING) {

            //CLEAR RENDERING
            renManager.renderer_refresh();

            //LISTEN TO EVENTS
            while (SDL_PollEvent(&EVENT)) {
                if (EVENT.type == SDL_EVENT_QUIT) {
                    GAME_RUNNING = false;
                    continue;
                }
                script_handle_camera_mouse(EVENT, camera);
            }

            //////TEST CODE        
            //Stopwatch HOW_LONG_THIS_SHIT_TAKES;
            //
            //
            //rectF cameraSpace = camera.get_view_rect();
            //std::size_t objectsCount = 0;
            ////draw
            //auto foundObjects = myObjsQuad.search_area(cameraSpace);
            //for (const auto& each : foundObjects) {
            //    //first draw
            //    auto& obj = myObjsQuad[each];
            //    rectF cameraAdjusted = camera.world_to_screen(obj.rect);
            //    renManager.fill_area_with(cameraAdjusted, obj.col);
            //    objectsCount++;
            //}
            ////ONLY MOVE THE OBJECTS ON THE SCREEN
            //for (auto& each : foundObjects) {
            //    auto& object = myObjsQuad[each];
            //    rectF newPos(object.rect.x + object.vel.x, object.rect.y + object.vel.y, object.rect.w, object.rect.h);
            //
            //    //myObjsQuad.relocate(each, newPos);
            //    //object.rect = newPos;
            //}
            //
            ////BOUNCHE OFF WINDOW EDGES TO KEEP THINGS IN VIEW, ALSO THIS IS HOW COLLISIONS GET COLLECTED
            //SequenceM<std::pair<std::size_t, rectF>> relocations;
            //for (std::size_t i = 0; i < myObjsQuad.size(); ++i) {
            //    auto& obj = myObjsQuad[i];
            //    rectF newBox = obj.rect;
            //    vec2f newVel = obj.vel;
            //    newBox.move_by(newVel);
            //
            //    if (newBox.x < 0) {
            //        newBox.x = 0;
            //        newVel.x = -newVel.x;
            //    }
            //    else if (newBox.x + newBox.w >= 960) {
            //        newBox.x = 960 - newBox.w;
            //        newVel.x = -newVel.x;
            //    }
            //
            //    if (newBox.y < 0) {
            //        newBox.y = 0;
            //        newVel.y = -newVel.y;
            //    }
            //    else if (newBox.y + newBox.h >= 540) {
            //        newBox.y = 540 - newBox.h;
            //        newVel.y = -newVel.y;
            //    }
            //    obj.rect = newBox;
            //    obj.vel = newVel;
            //    relocations.emplace_back(i, std::move(newBox));
            //}
            //
            ////SEARCH FOR COLLIDERS, NO RESOLUTION
            //auto colliders = myObjsQuad.search_collisions();
            //
            //myObjsQuad.relocate(relocations);
            //
            ////remove dead cells every frame
            //myObjsQuad.remove_dead_cells();
            //
            ////DRAW MOUSE BOX AND IF DELETE AREA
            //vec2f mouseScreenPos;
            //SDL_GetMouseState(&mouseScreenPos.x, &mouseScreenPos.y);
            //vec2f screenPos = camera.screen_to_world_point(mouseScreenPos);
            //rectF rectAroundMouse = rectF(
            //    screenPos.x - mouseBoxSize / 2,
            //    screenPos.y - mouseBoxSize / 2,
            //    mouseBoxSize, mouseBoxSize
            //);
            //rectF  cameraAdjustedMouse = camera.world_to_screen(rectAroundMouse);
            //
            //if (plzDeleteArea) {
            //    myObjsQuad.remove_area(rectAroundMouse);
            //}
            //
            //Color mouseCol = Colors::Magenta;
            //mouseCol.set_alpha(125u);
            //renManager.fill_area_with(cameraAdjustedMouse, mouseCol);
            //
            //time += HOW_LONG_THIS_SHIT_TAKES.dt_float();
            //frames++;
            //
            //if (frames == frame_target) {
            //    GAME_RUNNING = false;
            //}
            //////########################################################

            for (const auto& each : tree.myNodes()) {
                auto camAdjusted = camera.world_to_screen(each.aabb);

                static const float inset = 2.0f; // thickness of the hollow frame
                rectF inner{
                    camAdjusted.x + inset,
                    camAdjusted.y + inset,
                    camAdjusted.w - 2 * inset,
                    camAdjusted.h - 2 * inset
                };
                renManager.fill_area_with(camAdjusted, inner, Colors::Green);
            }

            //PRESENT
            renManager.renderer_present();
        }

        std::cout << "average time: " << time / frames << '\n';
        //OLD   0.00827729    0.00803674     0.00808599 -->> just about 60 FPS just doing quadtree stuff with 5k objects... sweaty
        //    
    }
    _CrtDumpMemoryLeaks();
    return 0;
}
