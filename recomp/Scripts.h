#pragma once

//COLLISION TEMPLATE
/*

        static float hold = 0;
        hold += dt;
        if (hold >= 0.008f) {

            //FIRST UPDATE VELOCITY
            for (auto& each : mRects)//later entities
                each.set_currVel_to_mainVel();
            //OR ANY OTHER COLLISION ALGO IN THE FUTURE IF SIMPLER NEEDED OR EVEN A SWITCH
            sweptAABB_algorithm(mRects, COLLISION_POLICY_PUSH);

            //THEN MOVE BLINDLY
            for (auto& each : mRects)//later entities
                each.update_position_default();

            //THIS IS EXTRA THAT MIGHT NOT BE REQUIRED IN THE END, WORLD EDGE DETECTION
            objects_vs_container_resolved(mRects, rectI(0, 0, 960, 540));

            hold = 0;
        }
*/

//HANDLE TASKS BETWEEN FRAMES, OLD CRUDE LOGIC (BUT GENERALLY GOOD IDEA)
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

#include "Camera.h"
#include "SDL3/SDL_events.h"

namespace badEngine {

    void script_handle_camera_mouse(SDL_Event& event, Camera2D& camera, vec2f mousePos)noexcept;

}