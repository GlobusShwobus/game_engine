#pragma once

#include <memory>
#include "SDL3/SDL_render.h"

namespace badEngine {

	class Sprite {

	private:
		std::unique_ptr<SDL_Texture> texture;
	};

}