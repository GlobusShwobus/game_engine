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

    TransformF tester1(rectF(0,100,64,64), vec2f(999,0));
    TransformF tester2(rectF(900,100,64,64),vec2f(-999,0));
    Color tester1Col = Colors::Magenta;
    Color tester2Col = Colors::Blue;


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


           //rectF broadBox = make_broad_phase_box(tester1);
           //rectF broadBox2 = make_broad_phase_box(tester2);
           //
           //if (rect_vs_rect(broadBox, broadBox2)) {
           //
           //    vec2f contactNormal;
           //    float contactTime = another_swept_check(tester1, tester2, contactNormal);
           //    
           //    tester1.mBox.x += tester1.mVelocity.x * contactTime;
           //    tester1.mBox.y += tester1.mVelocity.y * contactTime;
           //
           //    float remainder = 1.0f - contactTime;
           //
           //    tester2.mBox.x += tester2.mVelocity.x * remainder;
           //    tester2.mBox.y += tester2.mVelocity.y * remainder;
           //    
           //    tester1.mVelocity = vec2f(0, 0);
           //    tester2.mVelocity = vec2f(0, 0);
           //}
           //else {
           //    tester1.mBox.x += tester1.mVelocity.x;
           //    tester1.mBox.y += tester1.mVelocity.y;
           //
           //    tester2.mBox.x += tester2.mVelocity.x;
           //    tester2.mBox.y += tester2.mVelocity.y;
           //}

            float contactTime = 1.0f;
            vec2f contactNormal;
           
     
            if (do_swept_collision(tester1, tester2, contactNormal, contactTime)) {
                printf("yey\n");
              
                tester1.mBox.x += (tester1.mVelocity.x * contactTime);
                tester1.mBox.y += (tester1.mVelocity.y * contactTime);

                float remainder = 1.0f - contactTime;
               
                tester2.mBox.x += (tester2.mVelocity.x * remainder);
                tester2.mBox.y += (tester2.mVelocity.y * remainder);
              
                tester1.mVelocity = vec2f(0, 0);
                tester2.mVelocity = vec2f(0, 0);
            }
           
            tester1.mBox.x += (tester1.mVelocity.x*contactTime);
            tester1.mBox.y += (tester1.mVelocity.y*contactTime);
            tester2.mBox.x += (tester2.mVelocity.x*contactTime);
            tester2.mBox.y += (tester2.mVelocity.y*contactTime);

           //world wall collision
           // for (int i = 0; i < 10;i++) {
           //
           //     auto& rect = mRects[i].mBox;
           //     auto& vel = mRects[i].mVelocity;
           //
           //     if (rect.x < 0) {
           //         rect.x = 0;
           //         vel.x *= -1;
           //     }
           //     if (rect.y < 0) {
           //         rect.y = 0;
           //         vel.y *= -1;
           //     }
           //     if (rect.x + rect.w > 960) {
           //         rect.x = 960 - rect.w;
           //         vel.x *= -1;
           //     }
           //     if (rect.y + rect.h > 540) {
           //         rect.y = 540 - rect.h;
           //         vel.y *= -1;
           //     }
           // }
            hold = 0;
        }

        //just coloring in
        SDL_SetRenderDrawColor(sysManager.get_renderer(), tester1Col.get_red(), tester1Col.get_green(), tester1Col.get_blue(), tester1Col.get_alpha());
        SDL_FRect box1 = rectF_to_SDL_FRect(tester1.mBox);
        SDL_RenderFillRect(sysManager.get_renderer(), &box1);

        SDL_SetRenderDrawColor(sysManager.get_renderer(), tester2Col.get_red(), tester2Col.get_green(), tester2Col.get_blue(), tester2Col.get_alpha());
        SDL_FRect box2 = rectF_to_SDL_FRect(tester2.mBox);
        SDL_RenderFillRect(sysManager.get_renderer(), &box2);

        for (int i = 0; i < 10;i++) {


            //Color color = mColors[i];
            //SDL_SetRenderDrawColor(sysManager.get_renderer(),color.get_red(), color.get_green(),color.get_blue(), color.get_alpha());
            //SDL_FRect box = rectF_to_SDL_FRect(mRects[i].mBox);
            //SDL_RenderFillRect(sysManager.get_renderer(), &box);
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

