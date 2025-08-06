#pragma once

#include "SDL3/SDL.h"
#include "json.hpp"

/*
SERIALIZE THE WINDOW INIT STRUCT IN SERIALIZE
*/
namespace badEngine {
	class Window {

		struct SDLWindowDeleter {
			void operator()(SDL_Window* w)const {
				SDL_DestroyWindow(w);
				printf("\ndeleted window proof\n");
			}
		};
		struct SDLRendererDeleter {
			void operator()(SDL_Renderer* r)const {
				SDL_DestroyRenderer(r);
				printf("\ndeleted renderer proof\n");
			}
		};

		std::unique_ptr<SDL_Window, SDLWindowDeleter> window;
		std::unique_ptr<SDL_Renderer, SDLRendererDeleter> renderer;

	public:

		Window(const nlohmann::json* const windowConfig);

		void displayClear() {
			SDL_RenderClear(renderer.get());
		}
		void displayPresent() {
			SDL_RenderPresent(renderer.get());
		}

		SDL_Renderer* getRenderer() {
			return renderer.get();
		}
		SDL_Window* getWindow() {
			return window.get();
		}
	};
}