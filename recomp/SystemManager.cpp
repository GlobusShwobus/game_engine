#include "SystemManager.h"

namespace badEngine {

	SystemManager::system_setup_data SystemManager::get_JSON_setup(const nlohmann::json& data) {
		system_setup_data setup;
		setup.windowHeading = data.value("heading", default_window_heading);
		setup.windowWidth = data.value("window_width", default_window_width);
		setup.windowHeight = data.value("window_height", default_window_height);

		int sysEngine = data.value("engine", default_system_engine);
		int windowMode = data.value("mode", default_window_mode);
		setup.flags = sysEngine | windowMode;

		return setup;
	}
	void SystemManager::do_setup(const system_setup_data& setup) {
		//SDL internal init
		if (!SDL_Init(SDL_INIT_VIDEO))
			throw std::runtime_error(std::string("SDL_Init fail: ") + SDL_GetError());
		//setup window
		mWindow = SDL_CreateWindow(setup.windowHeading.c_str(), setup.windowWidth, setup.windowHeight, SDL_WindowFlags(setup.flags));
		if (!mWindow)
			throw std::runtime_error(std::string("SDL_Window init fail: ") + SDL_GetError());
		//setup renderer
		mRenderer = SDL_CreateRenderer(mWindow, nullptr);
		if (!mRenderer)
			throw std::runtime_error(std::string("SDL_Renderer init fail") + SDL_GetError());
	}
	void SystemManager::set_logical_presentation(SDL_RendererLogicalPresentation mode, uint32_t width, uint32_t height) {
		SDL_SetRenderLogicalPresentation(mRenderer, width, height, SDL_LOGICAL_PRESENTATION_STRETCH);
	}
	void SystemManager::renderer_clear() {
		SDL_RenderClear(mRenderer);
	}
	void SystemManager::renderer_present() {
		SDL_RenderPresent(mRenderer);
	}
}


