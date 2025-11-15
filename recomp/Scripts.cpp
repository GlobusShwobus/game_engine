#include "Scripts.h"
#include "SDL3/SDL_mouse.h"

namespace badEngine{

    void script_handle_camera_mouse(SDL_Event& event, Camera2D& camera)noexcept {

        //TODO:: currently when zoomed out it will pan really quick, zoomed in very slow. standardize it
        static bool mouseHeld = false;
        static vec2f lastMouseWorldPos;

        vec2f mouseScreenPos;
        SDL_GetMouseState(&mouseScreenPos.x, &mouseScreenPos.y);
        vec2f mouseWorldPos = camera.screen_to_world_point(mouseScreenPos);

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT) {//may be bugged atm whatever
            mouseHeld = true;
            lastMouseWorldPos = mouseWorldPos;//this event only sets it ONCE, not every frame, that's why delta is not 0
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            mouseHeld = false;
        }

        if (mouseHeld) {
            camera.pan(lastMouseWorldPos - mouseWorldPos);
            lastMouseWorldPos = camera.screen_to_world_point(mouseScreenPos);
        }

        if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            //wheel.y is vertial mouse wheel motion
            //TODO: make the zoom scaling modifiable in the future (prob just attach it to camera)
            if (event.wheel.y > 0) {
                camera.zoom_towards(1.1f, mouseScreenPos);
            }
            else if (event.wheel.y < 0) {
                camera.zoom_towards(0.9f, mouseScreenPos);
            }
        }
    }

    SequenceM<SweptResult> script_sweptAABB_get_colliders(SequenceM<TransformF>& objects) {
        //for loops
        const std::size_t entityCount = objects.size();
        //return type
        SequenceM<SweptResult> colliders;
        //reserve fat amount up front
        colliders.reserve(entityCount);
        //for every object
        for (std::size_t i = 0; i < entityCount; ++i) {
            //iterate over itself but only do A vs B and skip B vs A (j = i + 1)
            for (std::size_t j = i + 1; j < entityCount; ++j) {
                //do swept check, the function returns a bool and if true, we know it is a collision
                SweptResult result;
                if (objects[i].sweptAABB_dynamic(objects[j], result.collisionTime, result.collisionNormal)) {
                    result.pA = &objects[i];
                    result.pB = &objects[j];
                    colliders.emplace_back(std::move(result));
                }

            }
        }
        //trim down the mem (tho maybe notworth the extra reallocation, at least it is guaranteed to only be 2 allocations)
        colliders.shrink_to_fit();
        //sort the order of priority
        std::sort(colliders.begin(), colliders.end(), [](const auto& a, const auto& b) {
            return a.collisionTime < b.collisionTime;
            });

        return colliders;
    }
}