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

/*
DEPRICATED OLD COLLISION MODEL, BVH + hashgrid should be used...

    void script_do_continuous_collision_routine(SequenceM<Transform*>& objects) {
        //second do first round of checks
        struct Colliders {
            Transform* A = nullptr;
            Transform* B = nullptr;
            float time = 1.0f;
        };
        const std::size_t objCount = objects.size();

        SequenceM<Colliders> firstRoundColliders;
        for (std::size_t i = 0; i < objCount; ++i) {
            //iterate over itself but only do A vs B and skip B vs A (j = i + 1)
            for (std::size_t j = i + 1; j < objCount; ++j) {
                float time = 1.0f;
                if (sweptAABB_dynamic(*objects[i], *objects[j], time)) {
                    firstRoundColliders.emplace_back(objects[i], objects[j], time);
                }

            }
        }
        //sort the order of priority for collision
        std::sort(firstRoundColliders.begin(), firstRoundColliders.end(), [](Colliders& a, Colliders& b) {
            return a.time < b.time;
            });
        //third resolve the colliders first
        const std::size_t collidersCount = firstRoundColliders.size();

        for (auto& c : firstRoundColliders) {
            float time = 1.0f;
            vec2i normal;

            if (sweptAABB_dynamic(*c.A,*c.B, time, &normal)) {
                c.A->mCurrVelocity *= time;
                c.B->mCurrVelocity *= time;

                c.A->update_position();
                c.B->update_position();

                c.A->mCurrVelocity.nullify();
                c.B->mCurrVelocity.nullify();

                //SOME RESOLUTION HERE TOO???
            }
        }
    }
*/

#include <algorithm>

#include "Camera.h"
#include "SDL3/SDL_events.h"

#include "Transform.h"
#include "SequenceM.h"

namespace badEngine {

    void script_handle_camera_mouse(SDL_Event& event, Camera2D& camera)noexcept;
  
}