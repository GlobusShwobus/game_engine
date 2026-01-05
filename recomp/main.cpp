#include "Stopwatch.h"
#include "GraphicsSys.h"
#include "Configs.h"
#include <thread>
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#include <conio.h>

#include "Sprite.h"

#include "UniformGrid.h"

#include "SequenceM.h"
#include "SLList.h"
#include "NumberGenerator.h"
#include "Transform.h"
#include "Color.h"
#include "Camera.h"
#include "Scripts.h"
#include "Ray.h"

#include <iostream>
/*
PRIORITY: constexpr for SLList and then correct signatures for quad/BHV tree

1) revist quadtree (though not a lot of justification), mainly create a hash grid instead
2) documentation and visit <functional>
2) make engine class for on user create and run loop sepparation

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
        AABB window = AABB(0, 0, 960, 540);
        NumberGenerator gen;
        SequenceM<std::unique_ptr<AABB>> myABBS;
        myABBS.set_capacity(1000000);
        Stopwatch vecT;
        for (int i = 0; i < myABBS.capacity(); i++) {
            myABBS.emplace_back(std::make_unique<AABB>(gen.random_float(1, 800), gen.random_float(1, 500), 25, 25));
        }
        auto vecTT = vecT.dt_nanosec();


        UniformGrid<int> grid(window, 32,32);

        Stopwatch stdtime;
        int a1 = std::min(69, 420);
        auto result1 = stdtime.dt_nanosec();

        Stopwatch mytime;
        int a2 = bad_minV(69,420);
        auto result2 = mytime.dt_nanosec();

        Stopwatch stdtime2;
        int a3 = std::max(69, 420);
        auto result3 = stdtime2.dt_nanosec();

        Stopwatch mytime2;
        int a4 = bad_maxV(69, 420);
        auto result4 = mytime2.dt_nanosec();


        std::cout << "std min: " << result1 << "\tstd max: " << result3 << "\nmy min: " << result2 << "\tmy max: " << result4 << '\n';

        //for quadtree 160-180m ns

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
            AABB mouseRect = AABB(
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
