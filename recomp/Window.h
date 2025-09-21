#pragma once

#include "SDL3/SDL_video.h"
#include "SDL3/SDL_render.h"
#include "json.hpp"

namespace badEngine {
	class Window {

		struct SDLWindowDeleter {
			void operator()(SDL_Window* w)const {
				SDL_DestroyWindow(w);
			}
		};
		struct SDLRendererDeleter {
			void operator()(SDL_Renderer* r)const {
				SDL_DestroyRenderer(r);
			}
		};

	public:

		Window(const nlohmann::json& windowConfig);

		void set_logical_presentation(SDL_RendererLogicalPresentation mode, uint32_t width, uint32_t height);
		void renderer_clear();
		void renderer_present();

		inline SDL_Renderer* get_renderer()noexcept {
			return mRenderer.get();
		}
		inline SDL_Window* get_window()noexcept {
			return mWindow.get();
		}
	private:

		std::unique_ptr<SDL_Window, SDLWindowDeleter> mWindow;
		std::unique_ptr<SDL_Renderer, SDLRendererDeleter> mRenderer;
	};
}