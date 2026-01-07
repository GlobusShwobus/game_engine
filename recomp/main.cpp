#include "SLList.h"
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
        AABB window = AABB(0, 0, 960, 640);
        NumberGenerator gen;
        SequenceM<AABB> myABBS;
        myABBS.set_capacity(10000);
        Stopwatch vecT;
        for (int i = 0; i < myABBS.capacity(); i++) {
            myABBS.emplace_back(gen.random_float(0,959), gen.random_float(0,639), gen.random_float(1,64), gen.random_float(1,64));
        }
        auto wtf = vecT.dt_nanosec();

        UniformGrid muhGrid(window, 32.0f, 32.0f);
    
        Stopwatch insertTime;
        muhGrid.insert(myABBS.begin(), myABBS.end(), 0);
        auto it = insertTime.dt_nanosec();

        Stopwatch clearTime;
        muhGrid.clear();
        auto ct = clearTime.dt_nanosec();


        Stopwatch reinsertTime;
        for (std::size_t i = 0; i < myABBS.size(); ++i) {
            muhGrid.insert(i, myABBS[i]);
        }
        auto rit = reinsertTime.dt_nanosec();


        Stopwatch NsquaredTime;
        std::size_t collisionsTried = 0;
        std::size_t collisionsDetected = 0;
        for (std::size_t a = 0; a < myABBS.size(); ++a) {
            for (std::size_t b = a+1; b < myABBS.size(); ++b) {
                if (myABBS[a].intersects(myABBS[b])) {
                    collisionsDetected++;
                }
                collisionsTried++;
            }
        }
        auto NsquaredTimeTaken = NsquaredTime.dt_nanosec();
        

        SequenceM<std::pair<int, int>> potentials;
        potentials.set_capacity(myABBS.size());//arbitrary prediction

        Stopwatch GridQueryTime;
        muhGrid.query_pairs(potentials);
        auto GQTR = GridQueryTime.dt_nanosec();

        Stopwatch queryResultChecker;
        std::size_t collisionsTried2 = 0;
        std::size_t collisionsDetected2 = 0;
        for (const auto pair: potentials) {
            if (myABBS[pair.first].intersects(myABBS[pair.second])) {
                collisionsDetected2++;
            }
            collisionsTried2++;
        }
        auto QRC = queryResultChecker.dt_nanosec();



        std::cout << "vec: " << wtf << '\n';
        std::cout << "grid insert time: " << it << "\nclear time: " << ct << "\nreinsert time: " << rit << "\n";
        std::cout << "\n\nNAIVE N SQUARED SEARCH AND DETECT TIME: " << NsquaredTimeTaken << "\tcollisions tested: " << collisionsTried << "\tcollisions found: " << collisionsDetected << "\n";
        std::cout << "\n\nGRID SEARCH AND QUERY TIME: " << GQTR << "\tcollisions TEST TIME: " << QRC << "\tcombined: " << GQTR+ QRC << "\tcollisions tested: "<< collisionsTried2<<"\tcollisions found: "<< collisionsDetected2 <<'\n';


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
