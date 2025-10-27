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