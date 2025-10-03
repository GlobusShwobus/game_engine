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
        //###############################################################################

        float greenX;
        float greenY;
        static int redX = 200;
        static int redY = 200;
        SDL_GetMouseState(&greenX, &greenY);

        mGreenBox.set_destination_position(vec2i(greenX, greenY));
        mRedBox.set_destination_position(vec2i(redX, redY));
        mGreenBox.draw(sysManager.get_renderer());
        mRedBox.draw(sysManager.get_renderer());

        if (intersects_rectangle(mGreenBox.get_destination(), 300, 300)) {
            mBlueBox.set_destination_position(vec2i(400, 400));
            mBlueBox.draw(sysManager.get_renderer());
        }
        //if (intersects_rectangle(mGreenBox.get_destination(), mRedBox.get_destination())) {
        //    mBlueBox.set_destination_position(vec2i(400,400));
        //    mBlueBox.draw(sysManager.get_renderer());
        //}

        sysManager.renderer_present();
        //#################################################################################
        
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

