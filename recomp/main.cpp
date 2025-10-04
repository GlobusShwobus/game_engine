#include "Stopwatch.h"
#include "SystemManager.h"
#include "Configs.h"
#include <thread>

#include "Sprite.h"
#include "Animation.h"
#include "Font.h"
#include "Geometric_Functions.h"

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
    //initalize SDL system, can throw
    SystemManager sysManager;
    try {
        sysManager.init(windowConfig.get());
    }
    catch (const std::exception& excpt) {
        printf(excpt.what());
        return -1;
    }

    //TEST CODE
    Sprite mGreenBox("C:/Users/ADMIN/Desktop/recomp/Textures/green_box.png", sysManager.get_renderer());
    Sprite mRedBox("C:/Users/ADMIN/Desktop/recomp/Textures/red_box.png", sysManager.get_renderer());
    Sprite mBlueBox("C:/Users/ADMIN/Desktop/recomp/Textures/blue_box.png", sysManager.get_renderer());

    int redX = 200;
    int redY = 200;
    mRedBox.set_destination_position(vec2i(redX, redY));
    mBlueBox.set_destination_position(vec2i(400,400));

    float lineX = 20;
    float lineY = 20;

    ////#################################################################################

    //main loop
    Stopwatch UPDATE_DELTA_TIMER;
    bool GAME_RUNNING = true;
    SDL_Event EVENT;

    while (GAME_RUNNING) {
        float dt = UPDATE_DELTA_TIMER.dt_float();
        sysManager.renderer_clear();

        //LISTEN TO EVENTS
        while (SDL_PollEvent(&EVENT)) {
            if (EVENT.type == SDL_EVENT_QUIT) {
                GAME_RUNNING = false;
                continue;
            }
        }
        //TEST CODE

        float moseX = 0, mouseY = 0;
        Uint32 mouseState = SDL_GetMouseState(&moseX, &mouseY);

        /*
        ### 
        ALSO ADD RENDER COLOR TO SYS MANAGER/RENAME TO RENDER MANAGER FOR BETTER REP?
        ###
        */

        SDL_SetRenderDrawColor(sysManager.get_renderer(), 0, 0, 0, 255);
        SDL_RenderClear(sysManager.get_renderer());

        //white line
        SDL_SetRenderDrawColor(sysManager.get_renderer(), 255, 255, 255, 255);
        SDL_RenderLine(sysManager.get_renderer(), lineX, lineY, moseX, mouseY);

        //origin visual
        SDL_FRect originDot = { lineX - 3, lineY - 3, 7, 7 };
        SDL_SetRenderDrawColor(sysManager.get_renderer(), 0, 200, 0, 255);
        SDL_RenderFillRect(sysManager.get_renderer(), &originDot);

        //dest visual
        SDL_FRect mouseDot = { moseX - 4, mouseY - 4, 9, 9 };
        SDL_SetRenderDrawColor(sysManager.get_renderer(), 200, 0, 0, 255);
        SDL_RenderFillRect(sysManager.get_renderer(), &mouseDot);

        vec2f rayOrigin = vec2f(lineX, lineY);
        vec2f rayVec = vec2f(moseX, mouseY);
        auto target = mRedBox.get_destination();
        float t;
        vec2f cp;
        vec2f cn;
        if ((intersects_projection(rayOrigin, rayVec, target, t, &cp,&cn)) && t < 1.0f) {
            SDL_FRect contactPoint = { cp.x - 4, cp.y - 4, 9, 9 };
            SDL_SetRenderDrawColor(sysManager.get_renderer(), 255, 0, 255, 255);
            SDL_RenderFillRect(sysManager.get_renderer(), &contactPoint);
            mBlueBox.draw(sysManager.get_renderer());
        }

        mRedBox.draw(sysManager.get_renderer());

        //#################################################################################

        sysManager.renderer_present();

        //HANDLE TASKS BETWEEN FRAMES
        /*
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
        */
        //#################################################################################
        
    }
    SDL_Quit();

    return 0;
}

