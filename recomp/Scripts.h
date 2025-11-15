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

#include <algorithm>

#include "Camera.h"
#include "SDL3/SDL_events.h"

#include "Transform.h"
#include "SequenceM.h"

namespace badEngine {

    void script_handle_camera_mouse(SDL_Event& event, Camera2D& camera)noexcept;
    
   
    struct SweptResult {
        TransformF* pA = nullptr;
        TransformF* pB = nullptr;
        float collisionTime = 1.0f;
        vec2i collisionNormal;
    };

    SequenceM<SweptResult> script_sweptAABB_get_colliders(SequenceM<TransformF>& objects);

    template<typename Policy>
    concept SweptCollisionResolvePolicy = std::invocable<Policy, TransformF&, float, const vec2i&>;
    
    template<SweptCollisionResolvePolicy Policy>
    void script_do_sweptAABB_routine(SequenceM<TransformF>& objects, Policy policy) {
        // get the colliders
        auto collisions = script_sweptAABB_get_colliders(objects);

        //do the checks
        for (auto& colliders : collisions) {
            //shortcuts
            const float collisionTime = colliders.collisionTime;
            auto& objA = *colliders.pA;
            auto& objB = *colliders.pB;

            //set current velocity
            objA.mCurrVelocity *= collisionTime;
            objB.mCurrVelocity *= collisionTime;

            //move the box by the previous expression
            objA.update_position();
            objB.update_position();

            //set the current velocity to 0, because the maximum amount was moved this frame
            objA.mCurrVelocity = vec2f(0.0f, 0.0f);
            objB.mCurrVelocity = vec2f(0.0f, 0.0f);

            //set the behavior for the next frame (maybe better with a state behavior in the future???)
            policy(objA, collisionTime, colliders.collisionNormal);
            policy(objB, collisionTime, colliders.collisionNormal);
        }
    }

}