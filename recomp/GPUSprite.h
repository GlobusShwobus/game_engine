#pragma once

#include <memory>
#include <string_view>
#include "Surface.h"
#include "SDL3/SDL.h"

namespace badEngine {
	class Sprite {

		std::shared_ptr<SDL_Texture> texture = nullptr;

	public:
		
		Sprite(const Surface& surface, SDL_Renderer* renderer);

		Sprite(SDL_Surface* sdl_surface, SDL_Renderer* renderer);

		Sprite(std::string_view path, SDL_Renderer* renderer);//best version, others are for show

		bool isGood()const;

		void DrawTexture(SDL_Renderer* renderer, SDL_FRect* source = nullptr, SDL_FRect* dest = nullptr);
	};
}
