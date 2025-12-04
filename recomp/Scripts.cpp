#include "Scripts.h"
#include "SDL3/SDL_mouse.h"
#include "Collision.h"

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

        if (event.type == SDL_EVENT_WINDOW_RESIZED) {

            camera.set_screen_update(event.window.data1, event.window.data2);

        }

    }

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
}