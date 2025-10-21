#include "Stopwatch.h"
#include "RenderManager.h"
#include "Configs.h"
#include <thread>

#include "Sprite.h"
#include "Animation.h"
#include "Font.h"
#include "Collision_Functions.h"


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
    RenderManager renManager;
    try {
        renManager.init(windowConfig.get());
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
        SDL_SetRenderDrawColor(renManager.get_renderer_ref(), 0, 0, 0, 255);
        //##############################
        renManager.renderer_clear();

        //LISTEN TO EVENTS
        while (SDL_PollEvent(&EVENT)) {
            if (EVENT.type == SDL_EVENT_QUIT) {
                GAME_RUNNING = false;
                continue;
            }
        }
        //TEST CODE
        static float hold = 0;
        hold += dt;
        if (hold >= 0.008f) {
            
            //BEFORE ANYTHING, UPDATE VELOCITY 
            for (int i = 0; i < 10; i++) {
                mRects[i].reset_velocity();
            }

            collision_algorithm_executable(mRects, COLLISION_POLICY_REFLECT_BOTH);

            //AFTER COLLISION, MOVE BLINDLY (if there was collision and resolution applied, mCurrentVelocity should be zeroed out meaning no movement)
            for (int i = 0; i < 10; i++) {
                mRects[i].update_position();
            }

            //check for being off the edge and reflect
            static rectI container(0, 0, 960, 540);
            objects_vs_container_resolved(mRects, container);

            hold = 0;
        }

        for (int i = 0; i < 10;i++) {
            renManager.fill_area_with(mRects[i].mBox, mColors[i]);
        }
        //#################################################################################

        renManager.renderer_present();

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

