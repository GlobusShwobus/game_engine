#include "Window.h"
#include "Stopwatch.h"

#include <fstream> //file reader?
#include <thread>

#include "Surface.h"
#include "GPUSprite.h"
#include "Font.h"
#include "Animation.h"
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
    Stopwatch shittest;

    Sprite sprite("../Textures/player_sheet.png", window.getRenderer());
    sprite.setSourceW(32);
    sprite.setSourceH(32);
    sprite.setStretchWidth(32);
    sprite.setStretchHeight(32);
    Animation anime(Vek2(0.f, 32.f), Vek2(sprite.getSurfaceWidth(), sprite.getSurfaceHeight()), 32, 32, 8);

    auto shit = shittest.MarkMicroSec();
    std::cout << "shit version: " << shit << "\n";
    //###############################

    Stopwatch deltaTime;
    while (gameRunning) {
        float dt1 = deltaTime.MarkFloat();
        frameTimer.MarkFloat();

        window.displayClear();

        //LISTEN TO EVENTS
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                gameRunning = false;
            }
        }
        //###############################################################################
        anime.update(dt1);
        printf("infinite loop\n");
        Vek2 pos = anime.getFrame();
        sprite.setSourceX(pos.x);
        sprite.setSourceY(pos.y);
        sprite.DrawTexture(window.getRenderer(), 0,0);
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
