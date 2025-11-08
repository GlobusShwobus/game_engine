#pragma once
#include <memory>
#include <string_view>
#include "SDL3/SDL_render.h"
#include <SDL3_image/SDL_image.h>

namespace badEngine {
	class Texture {

		static constexpr auto SDLTextureDeleter = [](SDL_Texture* t) { if (t)SDL_DestroyTexture(t); };

	public:

		explicit Texture(SDL_Texture* texture)
			: mTexture(texture, SDLTextureDeleter) {
		}
		//takes ownership
		Texture(SDL_Surface& surface, SDL_Renderer* rendererRef)
			:mTexture(SDL_CreateTextureFromSurface(rendererRef, &surface), SDLTextureDeleter) {
		}
		Texture(std::string_view path, SDL_Renderer* rendererRef)
			: mTexture(IMG_LoadTexture(rendererRef, path.data()), SDLTextureDeleter) {
		}


		SDL_Texture* get()const {
			return mTexture.get();
		}
		int width()const {
			return mTexture->w;
		}
		int height()const {
			return mTexture->h;
		}
	private:
		std::unique_ptr<SDL_Texture, decltype(SDLTextureDeleter)> mTexture;
	};
}