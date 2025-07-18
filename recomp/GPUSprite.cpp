#include "GPUSprite.h"

namespace badEngine {
	Sprite::Sprite(const Surface& surface, SDL_Renderer& renderer)
	{
		SDL_Texture* texture = SDL_CreateTexture(&renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, surface.getWidth(), surface.getHeight());
		SDL_UpdateTexture(texture, nullptr, surface.data(), surface.getWidth()*sizeof(Uint32));
		this->texture = std::shared_ptr<SDL_Texture>(texture, SDL_DestroyTexture);
	}
	Sprite::Sprite(const std::shared_ptr<SDL_Texture>& texture) :texture(texture) {}
	SDL_Texture& Sprite::getTexture() {
		return *texture;
	}
}