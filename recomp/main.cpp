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

        static float holdTime = 0.0f;
        holdTime += dt;
        if (holdTime>=0.008f) {


            SequenceM<std::pair<int, float>> hits;
            float tHitTimes = 0;

            for (int ii = 0; ii < 10; ii++) {
                for (int jj = 0; jj < 10; jj++) {

                    if (ii != jj) {

                        if (intersects_projection_adjusted(mRects[ii], mRects[jj], tHitTimes, nullptr, nullptr)) {
                            hits.element_create(std::make_pair(ii, tHitTimes));
                        }


                    }

                }
            }

            std::sort(hits.begin(), hits.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
                {
                    return a.second < b.second;
                });





            for (int i = 0; i < 10; i++) {
                auto& transform = mRects[i];
                auto& box = transform.mBox;

                //SAVE THIS AS A REMINDER, OTHERWISE IDK WHERE TO PUT THIS SHIT, IF AT ALL. MAYBE NOT AT ALL...
                if (box.mPosition.x < 0) {
                    box.mPosition.x = 0;
                    transform.mVelocity.x *= -1;
                }
                else if(box.mPosition.x+box.mDimensions.x > 960) {
                    box.mPosition.x = 960 - box.mDimensions.x;
                    transform.mVelocity.x *= -1;
                }
                if (box.mPosition.y < 0) {
                    box.mPosition.y = 0;
                    transform.mVelocity.y *= -1;
                }
                else if (box.mPosition.y+box.mDimensions.y > 540) {
                    box.mPosition.y = 540 - box.mDimensions.y;
                    transform.mVelocity.y *= -1;
                }

                transform.mBox.mPosition += transform.mVelocity;
            }


            holdTime = 0;
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

