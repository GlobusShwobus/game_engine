#pragma once

#include "SDL3/SDL.h"
#include "json.hpp"

namespace badEngine {
	class SystemManager {

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

		SystemManager() = default;
		SystemManager(const nlohmann::json& windowConfig) {
			init(windowConfig);
		}
		SystemManager(const SystemManager&) = delete;
		SystemManager(SystemManager&&)noexcept = delete;
		SystemManager& operator=(const SystemManager&) = delete;
		SystemManager& operator=(SystemManager&&)noexcept = delete;

		~SystemManager() {
			SDL_DestroyRenderer(mRenderer);
			SDL_DestroyWindow(mWindow);
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


		
		SDL_Renderer* get_renderer()noexcept {
			return mRenderer;
		}
		SDL_Window* get_window()noexcept {
			return mWindow;
		}
		

	private:
		SDL_Window*   mWindow = nullptr;
		SDL_Renderer* mRenderer = nullptr;

		static constexpr std::string_view default_window_heading = "DEFAULT HEADING";
		static constexpr size_t default_window_width  = 960;
		static constexpr size_t default_window_height = 540;
		static constexpr size_t default_window_mode   = SDL_WINDOW_RESIZABLE;
		static constexpr size_t default_system_engine = SDL_WINDOW_OPENGL;
	};
}