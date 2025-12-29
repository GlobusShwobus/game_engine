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
        myObjsStore.set_capacity(2210);

        BVHTree<MyTester> tree(myObjsStore.capacity());

        Stopwatch insertionTimeVector;
        const float spacing = 10;
        const float width = 10;
        const float height = 10;
        float boxWidth = width + spacing;
        float boxHeight = height + spacing;

        for (int i = 0; i < 2210; ++i) {
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
            for (const auto& each : tree.myNodes()) {
                if (each.is_leaf()) {
                    static const float inset = 2.0f; // thickness of the hollow frame
                    auto camLeaf = camera.world_to_screen(each.user_data->rect);
                    rectF inner{
                        camLeaf.x + inset,
                        camLeaf.y + inset,
                        camLeaf.w - 2 * inset,
                        camLeaf.h - 2 * inset
                    };
                    renManager.fill_area_with(camLeaf, inner, Colors::Red);
                }
            }
            std::cout << tree.node_count() << '\n';

            //PRESENT
            renManager.renderer_present();
        }
    }
    _CrtDumpMemoryLeaks();
    return 0;
}
