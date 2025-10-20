#pragma once

#include <memory>
#include "SDL3/SDL.h"
#include "json.hpp"

namespace badEngine {
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
		void set_logical_presentation(SDL_RendererLogicalPresentation mode, uint32_t width, uint32_t height);
		void renderer_clear();
		void renderer_present();


		
		SDL_Renderer* get_renderer_ref()noexcept {
			return mRenderer.get();
		}
		SDL_Window* get_window_ref()noexcept {
			return mWindow.get();
		}
		

	private:
		/* ORDER MATTERS BECAUSE OF DELETER! */
		std::unique_ptr<SDL_Renderer, decltype(SDLRendererDeleter)> mRenderer{nullptr, SDLRendererDeleter };
		std::unique_ptr<SDL_Window, decltype(SDLWindowDeleter)>     mWindow{nullptr, SDLWindowDeleter };

		static constexpr std::string_view default_window_heading = "DEFAULT HEADING";
		static constexpr size_t default_window_width  = 960;
		static constexpr size_t default_window_height = 540;
		static constexpr size_t default_window_mode   = SDL_WINDOW_RESIZABLE;
		static constexpr size_t default_system_engine = SDL_WINDOW_OPENGL;
	};
}