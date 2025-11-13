#pragma once

#include <memory>
#include <string_view>
#include "SDL3/SDL_render.h"
#include <SDL3_image/SDL_image.h>
#include "Rectangle.h"

namespace badEngine {
	
	static constexpr auto SDLTextureDeleter = [](SDL_Texture* t) { if (t)SDL_DestroyTexture(t); };

	class Texture {

		inline SDL_FRect convert_rect(const rectF& rect)const noexcept {
			return SDL_FRect(rect.x, rect.y, rect.w, rect.h);
		}

	public:

		Texture(std::unique_ptr<SDL_Texture, decltype(SDLTextureDeleter)> texture);

		Texture(SDL_Surface& surface, SDL_Renderer* rendererRef);

		Texture(std::string_view path, SDL_Renderer* rendererRef);

		inline rectF get_control_block()const {
			return rectF(0.f, 0.f, (float)mTexture->w, (float)mTexture->h);
		}

		void draw(SDL_Renderer* renderer, const rectF& source, const rectF& dest)noexcept;

		inline bool isNullPtr()const noexcept {
			return mTexture == nullptr;
		}

	private:
		std::unique_ptr<SDL_Texture, decltype(SDLTextureDeleter)> mTexture;
	};
}