#pragma once

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

#include <algorithm>

#include "Camera.h"
#include "SDL3/SDL_events.h"

#include "Transform.h"
#include "SequenceM.h"

namespace badEngine {

    void script_handle_camera_mouse(SDL_Event& event, Camera2D& camera)noexcept;
    

    void script_do_continuous_collision_routine(SequenceM<TransformF*>& objects);
}