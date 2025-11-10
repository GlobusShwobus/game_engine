#pragma once

#include <memory>
#include <string_view>
#include "SDL3/SDL_render.h"
#include <SDL3_image/SDL_image.h>
#include "Rectangle.h"

namespace badEngine {
	class Texture {

		static constexpr auto SDLTextureDeleter = [](SDL_Texture* t) { if (t)SDL_DestroyTexture(t); };

		inline SDL_FRect convert_rect(const rectF& rect)const noexcept {
			return SDL_FRect(rect.x, rect.y, rect.w, rect.h);
		}

	public:

		Texture(SDL_Texture* texture);

		Texture(SDL_Surface& surface, SDL_Renderer* rendererRef);

		Texture(std::string_view path, SDL_Renderer* rendererRef);

		inline rectF get_control_block()const noexcept {
			return rectF(0, 0, mTexture->w, mTexture->h);
		}

		bool draw(SDL_Renderer* renderer, const rectF& source, const rectF& dest)noexcept;

		inline bool isNullPtr()const noexcept {
			return mTexture == nullptr;
		}

	private:
		std::unique_ptr<SDL_Texture, decltype(SDLTextureDeleter)> mTexture;
	};
}