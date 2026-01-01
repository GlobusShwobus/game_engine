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
#include "Ray.h"

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

        //#####################################################################################################################################################################
        //#####################################################################################################################################################################
        //#####################################################################################################################################################################
        //TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE 
        struct MyTester {
            float4 rect;
            Color col;
            MyTester(const float4& r, Color col) :rect(r), col(col) {}
        };
        NumberGenerator rng;

        const float windowWidth = 960.f;
        const float windowHeight = 540.f;
        int plz_do_dis_many = 2210;//same as erin cattos PDF

        SequenceM<std::unique_ptr<MyTester>> myObjsStore;
        myObjsStore.set_capacity(plz_do_dis_many);

        Stopwatch insertionTimeVector;
        for (int i = 0; i < plz_do_dis_many; ++i) {

            float w = rng.random_float(10, 50);
            float h = rng.random_float(10, 50);
            float x = rng.random_float(0, windowWidth-w);
            float y = rng.random_float(0, windowHeight-h);

            myObjsStore.emplace_back(std::make_unique<MyTester>(float4(x,y,w,h), Colors::Green));
        }

        std::size_t vectorInsertionTime = insertionTimeVector.dt_nanosec();

        BVHTree<MyTester> tree((myObjsStore.capacity() * 2) - 1);
        Stopwatch insertionTimeBVH;
        int counter = 0;
        for (auto it = myObjsStore.begin(); it != myObjsStore.end(); ++it) {
            MyTester* p = it->get();
            auto id = tree.create_proxy(p->rect, p);
            counter++;
            std::cout << counter << '\n';
        }
        std::size_t BVHInsertionTime = insertionTimeBVH.dt_nanosec();

        std::cout << "insertion into vector: " << vectorInsertionTime << "\ninserting into quadtree: " << BVHInsertionTime << "\ntotal: " << vectorInsertionTime + BVHInsertionTime << "\n";
        std::cout << "height of tree: " << tree.get_height() << '\n';

        TargetTexture t1(windowWidth, windowHeight, renManager);
        TargetTexture t2(windowWidth, windowHeight, renManager);

        Canvas canvas_SAH(t1);
        Canvas canvas_aabb(t2);
        SDL_Texture* pCanvas = canvas_SAH.get_texture();


        //draw SAH
        canvas_SAH.start_drawing(renManager);
        for (const auto& each : tree.myNodes()) {

            static const float inset = 2.0f; // thickness of the hollow frame
            const auto& nodeAABB = each.aabb;
            float4 inner{
                nodeAABB.x + inset,
                nodeAABB.y + inset,
                nodeAABB.w - 2 * inset,
                nodeAABB.h - 2 * inset
            };
            renManager.fill_area_with(nodeAABB, inner, Colors::Green);
        }
        canvas_SAH.end_drawing(renManager);
        //draw leafs
        canvas_aabb.start_drawing(renManager);
        for (const auto& each : tree.myNodes()) {
            if (each.is_leaf()) {
                static const float inset = 2.0f; // thickness of the hollow frame
                const auto& nodeAABB = each.aabb;
                float4 inner{
                    nodeAABB.x + inset,
                    nodeAABB.y + inset,
                    nodeAABB.w - 2 * inset,
                    nodeAABB.h - 2 * inset
                };
                renManager.fill_area_with(nodeAABB, inner, Colors::Red);
            }
        }
        canvas_aabb.end_drawing(renManager);
        renManager.set_render_blend_mode(SDL_BLENDMODE_BLEND);

        long double time = 0;
        std::size_t frames = 0;
        const std::size_t frame_target = 2000;
        Camera2D camera(windowWidth, windowHeight);


        SequenceM<Ray> sray;
        sray.set_capacity(640 * 640);
        float2 originPos(0, 0);
        float2 p0(1, 0), p1(1,1), p2(0,1);
        for (int y = 0; y < 640; y++) {
            for (int x = 0; x < 640; x++) {
                float2 pixelPos = p0 + (p1 - p0) * (x / 640.0f) + (p2 - p0) * (y / 640.f);
                Ray r;
                r.origin = originPos;
                r.dir = unit_vector(pixelPos - originPos);
                sray.push_back(r);
            }
        }
        float4 ayyy(0, 0, 640, 640);
 
        Stopwatch testingRays;
        for (int i = 0; i < 8; i++) {
            for (auto& r : sray) {
                Hit h;
                sweep(r, ayyy, h);
            }
        }
        std::size_t raystime = testingRays.dt_nanosec();
        std::cout << "ray test: " << raystime << '\n';
        //TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE 
        //#####################################################################################################################################################################
        //#####################################################################################################################################################################
        //#####################################################################################################################################################################
       
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

                if (EVENT.key.key == SDLK_A) {
                    pCanvas = canvas_SAH.get_texture();
                }
                if (EVENT.key.key == SDLK_S) {
                    pCanvas = canvas_aabb.get_texture();
                }
                script_handle_camera_mouse(EVENT, camera);
            }


            //PRESENT
            
            // test out ray shit. make a ray from one end to another and a rectangle around mouse

            renManager.renderer_present();
        }
    }
    _CrtDumpMemoryLeaks();
    return 0;
}
