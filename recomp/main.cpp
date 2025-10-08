#include "Stopwatch.h"
#include "SystemManager.h"
#include "Configs.h"
#include <thread>

#include "Sprite.h"
#include "Animation.h"
#include "Font.h"
#include "Geometric_Functions.h"


#include "SequenceM.h"
#include "NumberGenerator.h"
#include "Transform.h"
#include "Color.h"

#include <tuple>


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
    SequenceM<TransformF> mRects;
    SequenceM<Color> mColors;
    NumberGenerator mRng;

    for (int i = 0; i < 10;i++) {
        rectF rect = rectF(mRng.get_random(0, 900), mRng.get_random(0, 500), 35, 35);
        vec2f vel = vec2f(mRng.get_random(-5, 5), mRng.get_random(-5, 5));
        Color color = Colors::makeRGB(255u, mRng.get_random(0,255), mRng.get_random(0,255), mRng.get_random(0,255));
        
        mColors.element_create(color);
        mRects.element_assign(TransformF(rect, vel));
    }

    ////#################################################################################

    //main loop
    Stopwatch UPDATE_DELTA_TIMER;
    bool GAME_RUNNING = true;
    SDL_Event EVENT;

    while (GAME_RUNNING) {
        float dt = UPDATE_DELTA_TIMER.dt_float();
        //############################## VISUAL TEMP
        SDL_SetRenderDrawColor(sysManager.get_renderer(), 0, 0, 0, 255);
        //##############################
        sysManager.renderer_clear();

        //LISTEN TO EVENTS
        while (SDL_PollEvent(&EVENT)) {
            if (EVENT.type == SDL_EVENT_QUIT) {
                GAME_RUNNING = false;
                continue;
            }
        }
        //TEST CODE

        /*
        ### 
        ALSO ADD RENDER COLOR TO SYS MANAGER/RENAME TO RENDER MANAGER FOR BETTER REP?
        ###
        */

        static float hold = 0;
        hold += dt;
        if (hold >= 0.008f) {

            COLLISION_DEFLECT deflect;
            for (int i = 0; i < 10; ++i) {
                for (int j = i + 1; j < 10; ++j) {
                   
                    AABB_collision_algorithm(mRects[i], mRects[j], deflect);

                }
            }
            rectI worldEdge(0, 0, 960, 540);
            for (int i = 0; i < 10; ++i) {
                if (world_edge_collision_clamp(mRects[i], worldEdge)) {
                    mRects[i].mVelocity *= -1;
                }
            }

            hold = 0;
        }

        //just coloring in
        for (int i = 0; i < 10;i++) {
            Color color = mColors[i];
            SDL_SetRenderDrawColor(sysManager.get_renderer(),color.get_red(), color.get_green(),color.get_blue(), color.get_alpha());
            SDL_FRect box = rectF_to_SDL_FRect(mRects[i].mBox);
            SDL_RenderFillRect(sysManager.get_renderer(), &box);
        }

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

