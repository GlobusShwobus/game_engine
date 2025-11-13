#include "Texture.h"

namespace badEngine {

	Texture::Texture(std::unique_ptr<SDL_Texture, decltype(SDLTextureDeleter)> texture)
		: mTexture(std::move(texture))
	{
	}

	Texture::Texture(SDL_Surface& surface, SDL_Renderer* rendererRef)
		:mTexture(SDL_CreateTextureFromSurface(rendererRef, &surface), SDLTextureDeleter)
	{
	}

	Texture::Texture(std::string_view path, SDL_Renderer* rendererRef)
		: mTexture(IMG_LoadTexture(rendererRef, path.data()), SDLTextureDeleter)
	{
	}

	void Texture::draw(SDL_Renderer* renderer, const rectF& source, const rectF& dest)noexcept {
		SDL_FRect sdlSrc = convert_rect(source);
		SDL_FRect sdlDest = convert_rect(dest);

		int screenW, screenH;
		SDL_GetRenderOutputSize(renderer, &screenW, &screenH);

		//if obj is fully off screen skip any further rendering
		if (sdlDest.x + sdlDest.w <= 0 || sdlDest.y + sdlDest.h <= 0 || sdlDest.x >= screenW || sdlDest.y >= screenH)
			return;
		SDL_RenderTexture(renderer, mTexture.get(), &sdlSrc, &sdlDest);
	}
}