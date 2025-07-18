#pragma once
#include <memory>
#include "Surface.h"
#include "SDL3/SDL.h"

namespace badEngine {
	class Sprite {

		std::shared_ptr<SDL_Texture> texture;

	public:
		Sprite(const Surface& surface, SDL_Renderer& renderer);
		Sprite(const std::shared_ptr<SDL_Texture>& texture);
		SDL_Texture& getTexture();
	};
}
