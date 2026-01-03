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
PRIORITY: constexpr for SLList and then correct signatures for quad/BHV tree

1) BVH and related
2) make engine class for on user create and run loop sepparation
3) probably transform...
3) entities??? probably not

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

        SequenceM<float4> things;
        things.set_capacity(10);
        BinaryBVH<float4> things2(things.capacity());

        for (int i = 0; i < 10; i++) {
            float4 rect(i*20, i*20, i*20, i*20);
            things.emplace_back(rect);
        }

        for (auto& t : things) {
            things2.dynamic_insert(t, &t);
        }

        for (auto& n : things2) {
            if (n.is_leaf()) {
                static const float inset = 2.0f; // 
                const auto& nodeAABB = n.aabb;
                float4 inner{
                    nodeAABB.x + inset,
                    nodeAABB.y + inset,
                    nodeAABB.w - 2 * inset,
                    nodeAABB.h - 2 * inset
                };
                renManager.render_rectangle(nodeAABB,inner, Colors::Green);

            }
        }
        renManager.renderer_present();
        std::cin.get();
        return 69;

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

            if (sweep_fast(ray1, mouseRect)) {
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
