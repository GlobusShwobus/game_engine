#pragma once

#include <memory>
#include <string_view>
#include <stdexcept>
#include "SDL3/SDL_render.h"
#include <SDL3_image/SDL_image.h>

namespace badEngine {

	class Sprite {
		
		struct SDLTextureDeleter {
			void operator()(SDL_Texture* t)const {
				SDL_DestroyTexture(t);
			}
		};
	
	public:

		Sprite(SDL_Texture* texture) :mTexture(texture) {
			if (mTexture == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");
		}
		Sprite(SDL_Surface& surface, SDL_Renderer& renderer) {
			mTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(SDL_CreateTextureFromSurface(&renderer, &surface));
			if (mTexture == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");
		}
		Sprite(std::string_view path, SDL_Renderer& renderer) {
			mTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(IMG_LoadTexture(&renderer, path.data()));
			if (mTexture == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");
		}

		int get_width()const {
			return mTexture->w;
		}
		int get_height()const {
			return mTexture->h;
		}
		SDL_Texture* data()noexcept {
			return mTexture.get();
		}

	private:
		std::unique_ptr<SDL_Texture, SDLTextureDeleter> mTexture;
	};

}