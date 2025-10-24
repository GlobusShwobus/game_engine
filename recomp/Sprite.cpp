#include "Sprite.h"
#include <stdexcept>
namespace badEngine {

	Sprite::Sprite(std::shared_ptr<SDL_Texture> owner) :mTexture(owner) {
		if (mTexture == nullptr)
			throw std::runtime_error("Failed to create an SDL_Texture");
		on_init_default_set();
	}

	//takes ownership
	Sprite::Sprite(SDL_Texture* wildRaw) {
		if (wildRaw == nullptr)
			throw std::runtime_error("Failed to create an SDL_Texture");

		mTexture = std::shared_ptr<SDL_Texture>(wildRaw, SDLTextureDeleter);
		on_init_default_set();
	}
	//takes ownership
	Sprite::Sprite(SDL_Surface& surface, SDL_Renderer* rendererRef) {
		mTexture = std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(rendererRef, &surface), SDLTextureDeleter);
		if (mTexture == nullptr)
			throw std::runtime_error("Failed to create an SDL_Texture");
		on_init_default_set();
	}
	//takes ownership
	Sprite::Sprite(std::string_view path, SDL_Renderer* rendererRef) {
		mTexture = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(rendererRef, path.data()), SDLTextureDeleter);
		if (mTexture == nullptr)
			throw std::runtime_error("Failed to create an SDL_Texture");
		on_init_default_set();
	}
	void Sprite::on_init_default_set()noexcept {
		mControlBlock = rectF(0, 0, (float)mTexture->w, (float)mTexture->h);

		mSource = mControlBlock;
		mDest = mControlBlock;
	}

	void Sprite::draw(SDL_Renderer* rendererRef) {
		SDL_FRect source = SDL_FRect(mSource.x, mSource.y, mSource.w, mSource.h);
		SDL_FRect dest = SDL_FRect(mDest.x, mDest.y, mDest.w, mDest.h);

		int screenW, screenH;
		SDL_GetRenderOutputSize(rendererRef, &screenW, &screenH);//idk if this is correct. logically it would be fine, desired even to render a bit more than camera view
		//if obj is fully of screen skip any further rendering
		if (dest.x + dest.w <= 0 || dest.y + dest.h <= 0 || dest.x >= screenW || dest.y >= screenH)
			return;
		//SDL handles cliping to window
		SDL_RenderTexture(rendererRef, mTexture.get(), &source, &dest);
	}
	//##########################################################################

	bool Sprite::set_source(const rectF& setter)noexcept {
		if (mControlBlock.rect_contains(setter)) {
			mSource = setter;
			return true;
		}
		return false;
	}
	void Sprite::set_destination(const rectF& setter)noexcept {
		// don't need to check dest because, assuming correct screen position, it will stretch the img
		// also logically caller should make sure whatever pos is correct because the sprite can't possibly know
		mDest = setter;
	}

	bool Sprite::set_source_position(const vec2f& position)noexcept {
		rectF tester = rectF(position, mSource.w, mSource.h);
		if (mControlBlock.rect_contains(tester)) {
			mSource.x = position.x;
			mSource.y = position.y;
			return true;
		}
		return false;
	}
	bool Sprite::set_source_size(const vec2f& size)noexcept {
		rectF tester = rectF(mSource.x, mSource.y, size.x, size.y);
		if (mControlBlock.rect_contains(tester)) {
			mSource.w = size.x;
			mSource.h = size.y;
			return true;
		}
		return false;
	}

	void Sprite::set_destination_position(const vec2f& position)noexcept {
		mDest.x = position.x;
		mDest.y = position.y;
	}
	bool Sprite::set_destination_size(const vec2f& size)noexcept {
		if (size.x <= 0 || size.y <= 0)
			return false;

		mDest.w = size.x;
		mDest.h = size.y;
		return true;
	}

}