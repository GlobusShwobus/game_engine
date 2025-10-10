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

    TransformF tester1(rectF(0,100,64,64), vec2f(10,0));
    TransformF tester2(rectF(900,100,64,64),vec2f(-10,0));
    Color tester1Col = Colors::Magenta;
    Color tester2Col = Colors::Blue;

    TransformF tester3(rectF(0, 300, 64, 64), vec2f(999, 0));
    TransformF tester4(rectF(900, 300, 64, 64), vec2f(-999, 0));
    Color tester3Col = Colors::Gray;
    Color tester4Col = Colors::Red;


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

            float contactTime1 = 1.0f;
            vec2f contactNormal1;
           
     
            if (do_swept_collision(tester1, tester2, contactTime1, contactNormal1)) {
                printf("yey\n");
              
                tester1.mBox.x += (tester1.mVelocity.x * contactTime1);
                tester1.mBox.y += (tester1.mVelocity.y * contactTime1);

                tester2.mBox.x += (tester2.mVelocity.x * contactTime1);
                tester2.mBox.y += (tester2.mVelocity.y * contactTime1);
              
                tester1.mVelocity *= -1;
                tester2.mVelocity *= -1;
            }
            else {
                tester1.mBox.x += (tester1.mVelocity.x);
                tester1.mBox.y += (tester1.mVelocity.y);
                
                tester2.mBox.x += (tester2.mVelocity.x);
                tester2.mBox.y += (tester2.mVelocity.y);
            }
            float contactTime2 = 1.0f;
            vec2f contactNormal2;


            if (do_swept_collision(tester3, tester4, contactTime2, contactNormal2)) {
                printf("yey\n");

                tester3.mBox.x += (tester3.mVelocity.x * contactTime2);
                tester3.mBox.y += (tester3.mVelocity.y * contactTime2);

                tester4.mBox.x += (tester4.mVelocity.x * contactTime2);
                tester4.mBox.y += (tester4.mVelocity.y * contactTime2);

                tester3.mVelocity *= -1;
                tester4.mVelocity *= -1;
            }
            else {
                tester3.mBox.x += (tester3.mVelocity.x);
                tester3.mBox.y += (tester3.mVelocity.y);

                tester4.mBox.x += (tester4.mVelocity.x);
                tester4.mBox.y += (tester4.mVelocity.y);
            }


            rectI edge(0, 0, 960, 540);
            do_if_edge_collision(edge, tester1);
            do_if_edge_collision(edge, tester2);
            do_if_edge_collision(edge, tester3);
            do_if_edge_collision(edge, tester4);
            hold = 0;
        }

        //just coloring in
        SDL_SetRenderDrawColor(sysManager.get_renderer(), tester1Col.get_red(), tester1Col.get_green(), tester1Col.get_blue(), tester1Col.get_alpha());
        SDL_FRect box1 = rectF_to_SDL_FRect(tester1.mBox);
        SDL_RenderFillRect(sysManager.get_renderer(), &box1);

        SDL_SetRenderDrawColor(sysManager.get_renderer(), tester2Col.get_red(), tester2Col.get_green(), tester2Col.get_blue(), tester2Col.get_alpha());
        SDL_FRect box2 = rectF_to_SDL_FRect(tester2.mBox);
        SDL_RenderFillRect(sysManager.get_renderer(), &box2);

        SDL_SetRenderDrawColor(sysManager.get_renderer(), tester3Col.get_red(), tester3Col.get_green(), tester3Col.get_blue(), tester3Col.get_alpha());
        SDL_FRect box3 = rectF_to_SDL_FRect(tester3.mBox);
        SDL_RenderFillRect(sysManager.get_renderer(), &box3);

        SDL_SetRenderDrawColor(sysManager.get_renderer(), tester4Col.get_red(), tester4Col.get_green(), tester4Col.get_blue(), tester4Col.get_alpha());
        SDL_FRect box4 = rectF_to_SDL_FRect(tester4.mBox);
        SDL_RenderFillRect(sysManager.get_renderer(), &box4);

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

