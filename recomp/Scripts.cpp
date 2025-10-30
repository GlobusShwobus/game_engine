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

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
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
}