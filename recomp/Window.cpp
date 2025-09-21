#include "Window.h"

namespace badEngine {
	Window::Window(const nlohmann::json& windowConfig) {
		if (!windowConfig.contains("Window")) {
			throw std::runtime_error("JSON object does not contain valid field: Window");
		}

		const auto& windowData = windowConfig["Window"];

		const std::string windowHeading = windowData["heading"];
		const int windowWidth = windowData["window_width"];
		const int windowHeight = windowData["window_height"];
		const int fpsLimit = windowData["fps_limit"];
		const int flags = windowData["engine"].get<int>() | windowData["mode"].get<int>();

		mWindow = std::unique_ptr<SDL_Window, SDLWindowDeleter>(
			SDL_CreateWindow(windowHeading.data(), windowWidth, windowHeight, SDL_WindowFlags(flags))
		);

		mRenderer = std::unique_ptr<SDL_Renderer, SDLRendererDeleter>(
			SDL_CreateRenderer(mWindow.get(), nullptr)
		);
	}

	void Window::set_logical_presentation(SDL_RendererLogicalPresentation mode, uint32_t width, uint32_t height) {
		SDL_SetRenderLogicalPresentation(mRenderer.get(), width, height, SDL_LOGICAL_PRESENTATION_STRETCH);
	}
	void Window::renderer_clear() {
		SDL_RenderClear(mRenderer.get());
	}
	void Window::renderer_present() {
		SDL_RenderPresent(mRenderer.get());
	}
}


