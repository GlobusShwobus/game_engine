#include "Window.h"

namespace badEngine {
	Window::Window(const nlohmann::json* const windowConfig) {

		const auto& windowItem = (*windowConfig)["Window"];

		const int windowWidth = windowItem["window_width"];
		const int windowHeight = windowItem["window_height"];
		const int cameraWidth = windowItem["camera_width"];
		const int cameraHeight = windowItem["camera_height"];
		const int FPSTarget = windowItem["FPS_target"];
		const std::string title = windowItem["game_name"];

		window = std::unique_ptr<SDL_Window, SDLWindowDeleter>(
			SDL_CreateWindow(title.c_str(), windowWidth, windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL));
		renderer = std::unique_ptr<SDL_Renderer, SDLRendererDeleter>(
			SDL_CreateRenderer(window.get(), nullptr));

		//THIS IS NOT INTENDED BEHAVIOR, CAMERA SHOULD DETERMINE LOGICAL RENDERING NOT WINDOW SIZE, FIX LATER
		//DONT DO IT HERE
		SDL_SetRenderLogicalPresentation(renderer.get(), windowWidth, windowHeight, SDL_LOGICAL_PRESENTATION_STRETCH);

		//SDL_LOGICAL_PRESENTATION_DISABLED,     --> There is no logical size in effect 
		//SDL_LOGICAL_PRESENTATION_STRETCH,      --> The rendered content is stretched to the output resolution 
		//SDL_LOGICAL_PRESENTATION_LETTERBOX,    --> The rendered content is fit to the largest dimension and the other dimension is letterboxed with black bars 
		//SDL_LOGICAL_PRESENTATION_OVERSCAN,     --> The rendered content is fit to the smallest dimension and the other dimension extends beyond the output bounds 
		//SDL_LOGICAL_PRESENTATION_INTEGER_SCALE --> The rendered content is scaled up by integer multiples to fit the output resolution 
	}
}


