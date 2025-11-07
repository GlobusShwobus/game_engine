#pragma once

#include <string_view>
#include <memory>
#include <stdexcept>
#include "SDL3/SDL_render.h"
#include <SDL3_image/SDL_image.h>
#include "SequenceM.h"
#include "Rectangle.h"


namespace badEngine {

	class Texture {
		
		static constexpr auto SDLTextureDeleter = [](SDL_Texture* t) { if (t)SDL_DestroyTexture(t); };

	public:

		explicit Texture(SDL_Texture* texture)
			: mTexture(texture, SDLTextureDeleter) {}
		//takes ownership
		Texture(SDL_Surface& surface, SDL_Renderer* rendererRef)
			:mTexture(SDL_CreateTextureFromSurface(rendererRef, &surface), SDLTextureDeleter) {}
		Texture(std::string_view path, SDL_Renderer* rendererRef)
			: mTexture(IMG_LoadTexture(rendererRef, path.data()), SDLTextureDeleter) {}


		SDL_Texture* get()const noexcept {
			return mTexture.get();
		}
		int width()const noexcept {
			return mTexture->w;
		}
		int height()const noexcept {
			return mTexture->h;
		}
	private:
		std::unique_ptr<SDL_Texture, decltype(SDLTextureDeleter)> mTexture;
	};

	class Sprite {

	public:

		Sprite(Texture* texture)
		:mTexture(std::shared_ptr<Texture>(texture))
		{
			if (!texture)
				throw std::runtime_error("Sprite texture nullptr");

			mControlBlock = rectF(0, 0, mTexture->width(), mTexture->height());
		}
		Sprite(std::shared_ptr<Texture> texture)
		:mTexture(std::move(texture))
		{
			if (!texture)
				throw std::runtime_error("Sprite texture nullptr");

			mControlBlock = rectF(0, 0, mTexture->width(), mTexture->height());
		}

		virtual ~Sprite() = default;
		virtual void draw(SDL_Renderer* renderer, const vec2f& pos)const = 0;


	protected:

		bool is_within_control_block(const rectF& rect)const noexcept {
			return mControlBlock.contains_rect(rect);
		}

		void set_dest_pos(const vec2f& pos)noexcept {
			mDest.x = pos.x;
			mDest.y = pos.y;
		}
		void set_dest_size(const vec2f& size) {
			mDest.w = size.x;
			mDest.h = size.y;
		}
		SDL_Texture* get_texture()const noexcept {
			return mTexture->get();
		}

		rectF mSource = { 0,0,0,0 };
		rectF mDest = { 0,0,0,0 };

	private:
		rectF mControlBlock = { 0,0,0,0 };
		std::shared_ptr<Texture> mTexture;
	};

	class Animation: public Sprite {

	public:

		Animation(Texture* texture, const vec2i& start, const vec2i& frameSize, uint16_t frameCount) {

		}
		Animation(std::shared_ptr<Texture> texture, const vec2i& start, const vec2i& frameSize, uint16_t frameCount) {

		}
		
		//SET HOLD TIME FUNC

	private:
		SequenceM<vec2i> mFrames;

		uint16_t mFrameCount = 0;
		uint16_t mCurrentFrame = 0;

		float mHoldTime = 0.08f;
		float mCurrentFrameTime = 0.0f;
	};


	class Font :public Sprite {

	};

}