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
            //first current frame velocity to velocity
            for (int i = 0; i < 10; i++) {
                mRects[i].reset_velocity();
            }
            //do braod phase check and store colliders
            SequenceM<std::pair<int , float>> cols;

            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {

                    if (i == j)continue;

                    vec2f normal;
                    float time;

                    //only say this index collided with something, currently the only thing that matters is who and time
                    if (do_swept_collision(mRects[i], mRects[j], time, normal)) {
                        cols.element_create(i, time);
                    }
                }
            }
            //sort priority
            std::sort(cols.begin(), cols.end(),
                [](const std::pair<int, float>& a, const std::pair<int, float>& b){
                    return a.second < b.second;
                });
            //do final collision based on priority and set velocity for both current frame and what to do next frame
            for (const auto& co:cols) {
            
                for (int i = 0; i < 10;i++) {

                    if (co.first == i) {
                        continue;
                    }

                    vec2f normal;
                    float time;

                    if (do_swept_collision(mRects[co.first], mRects[i], time, normal)) {

                        mRects[co.first].set_current_velocity(mRects[co.first].mVelocity* time);
                        mRects[i].set_current_velocity(mRects[i].mVelocity* time);

                        mRects[co.first].set_velocity(mRects[co.first].mVelocity *= -1);
                        mRects[i].set_velocity(mRects[i].mVelocity *= -1);

                    }

                }

            }
            //set position, sets on current velocity. if current wasn't set it just sets whatever was velocity
            for (int i = 0; i < 10; i++) {
                mRects[i].update_position();
            }

            //do wall check
            rectI edge(0, 0, 960, 540);
            for (int i = 0; i < 10; i++) {
                vec2f output;
                if (contaier_vs_rect(edge, mRects[i].mBox, output)) {
                    mRects[i].mBox.x += output.x;
                    mRects[i].mBox.y += output.y;
                   
                    //tester code
                    //mRects[i].mVelocity *= -1;
                }
            }
          // rectI edge(0, 0, 960, 540);
          // for (int i = 0; i < 10; i++) {
          //     do_if_edge_collision(edge, mRects[i]);
          // }
            hold = 0;
        }

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

