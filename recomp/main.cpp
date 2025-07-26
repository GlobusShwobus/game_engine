#include "Window.h"
#include "Stopwatch.h"

#include <fstream> //file reader?
#include <thread>

#include "Surface.h"
#include "GPUSprite.h"
#include "Font.h"
#include "Animation.h"
#include "Sequence.h"
#include <iostream>

static nlohmann::json* initJSON(const char* path) {
    //look into assurances that the path is a json so we catch exception early
    std::ifstream in(path);
    nlohmann::json* json = nullptr;
    if (in.good()) {
        json = new nlohmann::json();
        in >> *json;
    }
    in.close();

    return json;
}
class JSON_Wrapper {//temporary until file reader sometime tm
public:
    nlohmann::json* json = nullptr;

    ~JSON_Wrapper() {
        delete json;
        json = nullptr;
    }
};

int main() {

    badEngine::Sequence<int> myInts;

    for (int i = 0; i < 99; i++) {
        myInts.add(i * i);
    }

    for (int o : myInts) {
        std::cout << o << '\n';
    }
    std::cout << "\n\n";
    std::cout << myInts.size() << " << MY SIZE\n";
    std::cout << myInts.capacity() << " << MY CAP";

    myInts.clear();

    std::cout << "\n\n";
    std::cout << myInts.size() << " << MY SIZE\n";
    std::cout << myInts.capacity() << " << MY CAP";

    //myInts.pop_back(); // crashes here == good

    myInts.clear();//should not crash on empty

    for (int i = 0; i < 99; i++) {
        myInts.add(i * i);
    }

    std::cout << "\n\n";
    std::cout << myInts.size() << " << MY SIZE\n";
    std::cout << myInts.capacity() << " << MY CAP";

    for (int i = 0; i < 27; i++) {
        myInts.pop_back();
    }

    std::cout << "\n\n";
    std::cout << myInts.size() << " << MY SIZE\n";
    std::cout << myInts.capacity() << " << MY CAP";

    std::cout << "\n\n";
    std::cout << myInts[10];

    std::cout << "\n\n";
   // std::cout << myInts[myInts.size()];//should crash
    std::cout << myInts[myInts.size() - 1];

    return 0;
}

/*
int main() {
    using namespace badEngine;
    
    //configs
    JSON_Wrapper entityConfig;
    JSON_Wrapper stageConfig;
    JSON_Wrapper windowConfig;

    try {
        entityConfig.json = initJSON("entity_config.json");
        stageConfig.json  = initJSON("stage_config.json");
        windowConfig.json = initJSON("window_config.json");
    }
    catch (const std::exception& excpt) {
        printf(excpt.what());
        return -1;
    }
    //initalize SDL and RenderWIndow
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("\nCRASH:: SDL_init Failure");
        return -1;
    }
    Window window(windowConfig.json);

    //initialize EntityFactory

    bool gameRunning = true;
    SDL_Event event;
    FrameTimer frameTimer;

    // TEST SURFACE TO GPU BULLSHIT

    //###############################

    while (gameRunning) {
        frameTimer.MarkFloat();

        window.displayClear();

        //LISTEN TO EVENTS
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                gameRunning = false;
            }
        }
        //###############################################################################

        window.displayPresent();
        //#################################################################################
        
        //HANDLE TASKS BETWEEN FRAMES
        auto dt = frameTimer.MarkMilliSec();
        auto limit = frameTimer.getLimitMilliSec();
        if (dt < limit) {

            const auto spareTime = limit - dt;

            Stopwatch timeIntermediary;
            //DO SHIT HERE.. ALSO IF LOOPITY ACTION THEN THE TIMER SHOULD PROBABLY BE PART OF THE LOOP INSTEAD 

            //#################################################################################

            auto idt = timeIntermediary.MarkMilliSec();

            const auto remainingTime = spareTime - idt;
            
            if (remainingTime.count() > 0) {
                std::this_thread::sleep_for(remainingTime);
            }
        }
        //#################################################################################
        
    }
    SDL_Quit();

    return 0;
}
*/
