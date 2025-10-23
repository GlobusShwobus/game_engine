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


#include "Serialization.h"
/*
SERIALIZE FIRST
MORE COLLISION RESOLUTIONS SECOND
QUADTREE THIRD
?????
*/

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
        Color color = Colors::makeRGBA(mRng.get_random(0,255), mRng.get_random(0,255), mRng.get_random(0,255), 255u);
        
        mColors.element_create(color);
        mRects.element_assign(TransformF(rect, vel));
    }
    ////#################################################################################

    //TEST CODE 2
    //testObj tester(255, 2, true);
    Test1 t1(69);//generic
    Test2 t2(420); //big oopsie, still serializes but leaves hidden data as garbage == big bad
    Test3 t3;//throws static assert == good


    //std::ofstream ff("C:/Users/ADMIN/Desktop/test_serialize/t3.bin", std::ios::binary);
    //POD_serialize(ff, t3);

    std::ifstream ff("C:/Users/ADMIN/Desktop/test_serialize/t1.bin", std::ios::binary);
    POD_deserialize(ff, t2);//AYY CARAMBA!!!   

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
        //COLLISION
        static float hold = 0;
        hold += dt;
        if (hold >= 0.008f) {
            
            collision_algorithm_executable(mRects, rectI(0, 0, 960, 540), COLLISION_POLICY_REFLECT_BOTH);

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

