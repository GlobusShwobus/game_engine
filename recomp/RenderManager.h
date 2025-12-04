#pragma once

#include <memory>
#include "SDL3/SDL.h"
#include "Color.h"
#include "Rectangle.h"
#include "json.hpp"
#include "BadExceptions.h"

namespace badEngine {
#define BAD_RENDERER_EXCEPTION(type,note) BadException(__FILE__, __LINE__,type,note)

	class RenderManager {

		static constexpr auto SDLWindowDeleter = [](SDL_Window* window)noexcept {
			if (window) SDL_DestroyWindow(window);
			};


		static constexpr auto SDLRendererDeleter = [](SDL_Renderer* renderer)noexcept {
			if (renderer) SDL_DestroyRenderer(renderer);
			};

		struct system_setup_data {
			std::string windowHeading = default_window_heading.data();
			int windowWidth           = default_window_width;
			int windowHeight          = default_window_height;
			int flags                 = default_window_mode | default_system_engine;
		};

		constexpr system_setup_data get_default_setup()noexcept {
			return system_setup_data();
		}
		system_setup_data get_JSON_setup(const nlohmann::json& data);
		void do_setup(const system_setup_data& setup);

	public:

		RenderManager() = default;
		RenderManager(const nlohmann::json& windowConfig) {
			init(windowConfig);
		}
		RenderManager(const RenderManager&) = delete;
		RenderManager(RenderManager&&)noexcept = delete;
		RenderManager& operator=(const RenderManager&) = delete;
		RenderManager& operator=(RenderManager&&)noexcept = delete;

		~RenderManager() {
			SDL_Quit();
		}

		void init() {
			do_setup(get_default_setup());
		}
		void init(const nlohmann::json& windowConfig) {
			if (windowConfig.contains("GameSysConfig"))
				do_setup(
					get_JSON_setup(windowConfig["GameSysConfig"])
				);
		}

		bool set_render_blend_mode(SDL_BlendMode mode)noexcept {
			return SDL_SetRenderDrawBlendMode(mRenderer.get(), mode);
		}

		bool set_render_draw_color(Color color)noexcept {
			if (SDL_SetRenderDrawColor(mRenderer.get(), color.get_red(), color.get_green(), color.get_blue(), color.get_alpha())) {
				mDrawColor = color;
				return true;
			}
			return false;
		}


		void fill_area_with(const rectF& area, Color color) {
			SDL_Renderer* ren = mRenderer.get();
			SDL_SetRenderDrawColor(ren, color.get_red(), color.get_green(), color.get_blue(), color.get_alpha());
			SDL_FRect sdlArea = SDL_FRect(area.x, area.y, area.w, area.h);
			SDL_RenderFillRect(ren, &sdlArea);
			SDL_SetRenderDrawColor(ren, mDrawColor.get_red(), mDrawColor.get_green(), mDrawColor.get_blue(), mDrawColor.get_alpha());
		}

		//overrides location that is being drawn on. 
		//Texture will be permanently overwritten in memory, so make a copy or just keep in mind to reload a clean slate
		//must be called each frame because renderer_present will reset it
		//useful for soemthing like a worldmap to have permanent changes
		bool set_render_target(SDL_Texture* target) {
			return target != nullptr && SDL_SetRenderTarget(mRenderer.get(), target);
		}
		
		bool renderer_present() {
			SDL_Renderer* ren = mRenderer.get();
			if (SDL_GetRenderTarget(ren) != nullptr) 
				SDL_SetRenderTarget(ren, nullptr);//on setting to null should never error
			
			return SDL_RenderPresent(ren);
		}
		bool renderer_refresh() {
			SDL_Renderer* ren = mRenderer.get();
			if (SDL_GetRenderTarget(ren) != nullptr)
				SDL_SetRenderTarget(ren, nullptr);

			//clears everything to current draw color
			return SDL_RenderClear(ren);
		}

		
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