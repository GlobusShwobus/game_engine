#include "GPUSprite.h"
#include "SDL3_image/SDL_image.h"

namespace badEngine {
	Sprite::Sprite(const Surface& surface, SDL_Renderer* renderer)
	{
		SDL_Texture* raw = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, surface.getWidth(), surface.getHeight());
		SDL_UpdateTexture(raw, nullptr, surface.data(), surface.getWidth()*sizeof(Uint32));
		texture = std::shared_ptr<SDL_Texture>(raw, SDL_DestroyTexture);
	}

	Sprite::Sprite(SDL_Surface* sdl_surface, SDL_Renderer* renderer)
	{
		SDL_Texture* raw = SDL_CreateTextureFromSurface(renderer, sdl_surface);
		texture = std::shared_ptr<SDL_Texture>(raw, SDL_DestroyTexture);
	}

	Sprite::Sprite(std::string_view path, SDL_Renderer* renderer)
	{
		SDL_Texture* raw  = IMG_LoadTexture(renderer, path.data());
		texture = std::shared_ptr<SDL_Texture>(raw, SDL_DestroyTexture);
	}

	bool Sprite::isGood()const
	{
		return texture != nullptr;
	}

	void Sprite::DrawTexture(SDL_Renderer* renderer, SDL_FRect* source, SDL_FRect* dest)
	{
		SDL_RenderTexture(renderer, texture.get(), source, dest);
	}
}