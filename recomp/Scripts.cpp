#include "Scripts.h"

namespace badEngine{

    void script_handle_camera_mouse(SDL_Event& event, Camera2D& camera, vec2f mousePos)noexcept {

        static bool mouseHeld = false;

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            mouseHeld = true;
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            mouseHeld = false;
        }

        if (mouseHeld) {
            camera.focus_on(mousePos);
        }

        if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            //wheel.y is vertial mouse wheel motion
            //TODO: make the zoom scaling modifiable in the future (prob just attach it to camera)
            if (event.wheel.y > 0) {
                camera.zoom_towards(1.1f, mousePos);
            }
            else if (event.wheel.y < 0) {
                camera.zoom_towards(0.9f, mousePos);
            }
        }

    }
}