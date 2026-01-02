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
        NumberGenerator rng;
        const float windowWidth = 960.f;
        const float windowHeight = 540.f;
        Camera2D camera(windowWidth, windowHeight);
        int plz_do_dis_many = 2210;//same as erin cattos PDF

        SequenceM<std::unique_ptr<float4>> myObjsStore;
        myObjsStore.set_capacity(plz_do_dis_many);

        Stopwatch insertionTimeVector;
        for (int i = 0; i < plz_do_dis_many; ++i) {

            float w = rng.random_float(10, 50);
            float h = rng.random_float(10, 50);
            float x = rng.random_float(0, windowWidth-w);
            float y = rng.random_float(0, windowHeight-h);

            myObjsStore.emplace_back(std::make_unique<float4>(float4(x,y,w,h)));
        }

        std::size_t vectorInsertionTime = insertionTimeVector.dt_nanosec();

        BVHTree<float4> tree((myObjsStore.capacity() * 2) - 1);
        Stopwatch insertionTimeBVH;
        for (auto it = myObjsStore.begin(); it != myObjsStore.end(); ++it) {
            
            auto id = tree.create_proxy(*it->get(), it->get());
        }
        std::size_t BVHInsertionTime = insertionTimeBVH.dt_nanosec();

        std::cout << "insertion into vector: " << vectorInsertionTime << "\ninserting into quadtree: " << BVHInsertionTime << "\ntotal: " << vectorInsertionTime + BVHInsertionTime << "\n";
        std::cout << "height of tree: " << tree.get_height() << '\n';

        renManager.set_render_blend_mode(SDL_BLENDMODE_BLEND);


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
            }


            //PRESENT
            struct myObject {
                float2 pos = float2(100, 100);
                float2 vec = float2(0, 540);
            }myObj;
            float2 myObjEndPoint = myObj.pos + myObj.vec;
            renManager.render_line(myObj.pos, myObjEndPoint, Colors::Blue);
            //
            Ray ray1;
            ray1.origin = myObj.pos;
            ray1.dir = unit_vector(myObj.vec);

            float2 mousePos;
            SDL_GetMouseState(&mousePos.x, &mousePos.y);
            float4 mouseRect = float4(
                mousePos.x - 64 / 2,
                mousePos.y - 64 / 2,
                64, 64
            );


            Color mouseCol;
            Hit hit;
            sweep(ray1, mouseRect, hit);

            if (hit.is_hit_dot(dot_vector(myObj.vec))) {
                mouseCol = Colors::Red;
            }
            else {
                mouseCol = Colors::Green;
            }
            renManager.render_rectangle(mouseRect, mouseCol);


            renManager.renderer_present();
        }
    }
    _CrtDumpMemoryLeaks();
    return 0;
}
