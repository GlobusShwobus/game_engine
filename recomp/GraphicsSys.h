#pragma once

#include <memory>
#include "SDL3/SDL.h"
#include "Color.h"
#include "Rectangle.h"
#include "json.hpp"
#include "BadExceptions.h"

namespace badEngine {
#define BAD_RENDERER_EXCEPTION(type,note) BadException(__FILE__, __LINE__,type,note)

	class GraphicsSys {

		static constexpr auto SDLWindowDeleter = [](SDL_Window* window)noexcept {
			if (window) SDL_DestroyWindow(window);
			};


		static constexpr auto SDLRendererDeleter = [](SDL_Renderer* renderer)noexcept {
			if (renderer) SDL_DestroyRenderer(renderer);
			};


		void do_setup(std::string_view heading, Uint32 width, Uint32 height, SDL_WindowFlags flags);

	public:

		GraphicsSys() {
			init_default();
		}
		GraphicsSys(const nlohmann::json& windowConfig) {
			init_from_config(windowConfig);
		}
		GraphicsSys(const GraphicsSys&) = delete;
		GraphicsSys(GraphicsSys&&)noexcept = delete;
		GraphicsSys& operator=(const GraphicsSys&) = delete;
		GraphicsSys& operator=(GraphicsSys&&)noexcept = delete;

		~GraphicsSys() {
			reset();
		}
		//may throw
		void init_default();
		//if ALL required config fields exist, may throw on creation
		//otherwise returns a bool. for specific failure reason call SDL_GetError
		bool init_from_config(const nlohmann::json& windowConfig);
		//shuts down all sub systems and SDL
		void reset()noexcept;

		//on fail call SDL_GetError()
		bool set_render_blend_mode(SDL_BlendMode mode)noexcept;
		//on fail call SDL_GetError()
		bool set_render_draw_color(Color color)noexcept;

		
		void fill_area_with(const rectF& area, Color color);

		//overrides location that is being drawn on. 
		//Texture will be permanently overwritten in memory, so make a copy or just keep in mind to reload a clean slate
		//must be called each frame because renderer_present will reset it
		//useful for soemthing like a worldmap to have permanent changes
		bool set_render_target(SDL_Texture* target);
		
		bool renderer_present();
		bool renderer_refresh();

		
		SDL_Renderer* const get_renderer()noexcept {
			return mRenderer.get();
		}
		SDL_Window* const get_window()noexcept {
			return mWindow.get();
		}

	private:
		/* ORDER MATTERS BECAUSE OF DELETER! */
		std::unique_ptr<SDL_Renderer, decltype(SDLRendererDeleter)> mRenderer{nullptr, SDLRendererDeleter };
		std::unique_ptr<SDL_Window, decltype(SDLWindowDeleter)>     mWindow{nullptr, SDLWindowDeleter };
		Color mDrawColor = Colors::Black;

		static constexpr std::string_view default_window_heading = "DEFAULT HEADING";
		static constexpr size_t default_window_width  = 960;
		static constexpr size_t default_window_height = 540;
		static constexpr size_t default_window_mode   = SDL_WINDOW_RESIZABLE;
		static constexpr size_t default_system_engine = SDL_WINDOW_OPENGL;
	};
}