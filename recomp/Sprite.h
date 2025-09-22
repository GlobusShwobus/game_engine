#pragma once

#include <memory>
#include "SDL3/SDL_render.h"

namespace badEngine {

	class Sprite {
		Sprite(SDL_Texture* texture) :mTexture(texture) {}


	private:
		std::unique_ptr<SDL_Texture> mTexture;
	};

}