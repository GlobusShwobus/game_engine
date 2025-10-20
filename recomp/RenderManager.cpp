#include "RenderManager.h"

namespace badEngine {

	RenderManager::system_setup_data RenderManager::get_JSON_setup(const nlohmann::json& data) {
		system_setup_data setup;
		setup.windowHeading = data.value("heading", default_window_heading);
		setup.windowWidth = data.value("window_width", default_window_width);
		setup.windowHeight = data.value("window_height", default_window_height);

		int sysEngine = data.value("engine", default_system_engine);
		int windowMode = data.value("mode", default_window_mode);
		setup.flags = sysEngine | windowMode;

		return setup;
	}
	void RenderManager::do_setup(const system_setup_data& setup) {
		//SDL internal init
		if (!SDL_Init(SDL_INIT_VIDEO))
			throw std::runtime_error(std::string("SDL_Init fail: ") + SDL_GetError());
		//setup window
		mWindow.reset(
			SDL_CreateWindow(setup.windowHeading.c_str(), setup.windowWidth, setup.windowHeight, SDL_WindowFlags(setup.flags))
		);
		if (!mWindow)
			throw std::runtime_error(std::string("SDL_Window init fail: ") + SDL_GetError());
		//setup renderer
		mRenderer.reset(
			SDL_CreateRenderer(mWindow.get(), nullptr)
		);
		if (!mRenderer)
			throw std::runtime_error(std::string("SDL_Renderer init fail") + SDL_GetError());
	}
	void RenderManager::set_logical_presentation(SDL_RendererLogicalPresentation mode, uint32_t width, uint32_t height) {
		SDL_SetRenderLogicalPresentation(mRenderer.get(), width, height, SDL_LOGICAL_PRESENTATION_STRETCH);
	}
	void RenderManager::renderer_clear() {
		SDL_RenderClear(mRenderer.get());
	}
	void RenderManager::renderer_present() {
		SDL_RenderPresent(mRenderer.get());
	}
}


