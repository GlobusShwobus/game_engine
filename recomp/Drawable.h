#pragma once

#include <memory>
#include <string_view>
#include "SDL3/SDL_render.h"
#include <SDL3_image/SDL_image.h>
#include "SequenceM.h"
#include "Rectangle.h"
#include <assert.h>

namespace badEngine {
	
	class Texture {

		static constexpr auto SDLTextureDeleter = [](SDL_Texture* t) { if (t)SDL_DestroyTexture(t); };

		SDL_FRect convert_rect(const rectF& rect)const noexcept {
			return SDL_FRect(rect.x, rect.y, rect.w, rect.h);
		}

	public:

		explicit Texture(SDL_Texture* texture)
			: mTexture(texture, SDLTextureDeleter),
			mControlBlock(0,0, mTexture->w, mTexture->h) {}

		Texture(SDL_Surface& surface, SDL_Renderer* rendererRef)
			:mTexture(SDL_CreateTextureFromSurface(rendererRef, &surface), SDLTextureDeleter),
			mControlBlock(0,0, mTexture->w, mTexture->h) {}

		Texture(std::string_view path, SDL_Renderer* rendererRef)
			: mTexture(IMG_LoadTexture(rendererRef, path.data()), SDLTextureDeleter), 
			mControlBlock(0, 0, mTexture->w, mTexture->h) {}

		rectF get_texture_rect()const noexcept {
			return mControlBlock;
		}

		void draw(SDL_Renderer* renderer, const rectF& source, const rectF& dest) {
			if (mControlBlock.contains_rect(source)) {
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
	private:
		std::unique_ptr<SDL_Texture, decltype(SDLTextureDeleter)> mTexture;
		const rectF mControlBlock;
	};


	class Animation {

	public:

		Animation(Texture* texture, const vec2i& start, uint16_t fWidth, uint16_t fHeight, uint16_t fCount)
			:Animation(std::shared_ptr<Texture>(texture), start, fWidth, fHeight, fCount) {}

		Animation(std::shared_ptr<Texture> texture, const vec2i& start, uint16_t fWidth, uint16_t fHeight, uint16_t fCount)
			:mTexture(std::move(texture)), mFrameCount(fCount), mFrameWidth(fWidth), mFrameHeight(fHeight)
		{
			assert(start.x >= 0 && start.y >= 0 && "Out of bounds texture access");

			const rectF controlBlock = rectF(
				start.x,
				start.y,
				start.x + (fWidth * fCount),
				start.y + (fHeight * fCount)
			);
			const rectF textureRect = mTexture->get_texture_rect();
			//check if the entire demand is within the control block
			assert(textureRect.contains_rect(controlBlock) && "demanded size too large for this texture");

			for (uint16_t i = 0; i < fCount; ++i)
				mFrames.emplace_back(start.x + (i * fWidth), start.y);
		}
		void update(float dt)noexcept {
			mCurrentFrameTime += dt;
			while (mCurrentFrameTime >= mHoldTime) {
				advance();
				mCurrentFrameTime -= mHoldTime;
			}
		}
		inline void advance()noexcept {
			++mCurrentFrame;
			if (mCurrentFrame >= mFrameCount)
				mCurrentFrame = 0;
		}
		void set_frame_hold_time(float time)noexcept {
			assert(time >= 0 && "negative time");
			mHoldTime = time;
		}
		void draw(SDL_Renderer* renderer, const rectF& dest) {//getting dest as rect is actually great idea because of camera or other factors, gyat damn

			const auto& curFramePos = mFrames[mCurrentFrame];
			
			rectF source(
				curFramePos.x,
				curFramePos.y,
				mFrameWidth,
				mFrameHeight
			);

			mTexture->draw(renderer, source, dest);
		}
	private:
		SequenceM<vec2i> mFrames;
		std::shared_ptr<Texture> mTexture;

		uint16_t mFrameWidth = 0;
		uint16_t mFrameHeight = 0;
		uint16_t mFrameCount = 0;
		uint16_t mCurrentFrame = 0;

		float mHoldTime = 0.08f;
		float mCurrentFrameTime = 0.0f;
	};

	class Font {
	public:

		Font(Texture* texture, uint32_t columnsCount, uint32_t rowsCount) :Font(std::shared_ptr<Texture>(texture), columnsCount, rowsCount) {}

		Font(std::shared_ptr<Texture> texture, uint32_t columnsCount, uint32_t rowsCount)
			:mTexture(std::move(texture)),
			mColumnsCount(columnsCount),
			mRowsCount(rowsCount),
			mGylphWidth(mTexture->get_texture_rect().w / mColumnsCount),
			mGylphHeight(mTexture->get_texture_rect().h / mRowsCount)
		{
			assert((mGylphWidth * mColumnsCount) == mTexture->get_texture_rect().w && "texture image likely off size or invalid counts");
			assert((mGylphHeight * mRowsCount) == mTexture->get_texture_rect().h && "texture image likely off size or invalid counts");
		}

		void draw(SDL_Renderer* renderer, const rectF& dest) {
			
			for (const auto& each : letterPos) {

				rectF sdlSrc = rectF(each.first.x, each.first.y, mGylphWidth, mGylphHeight);
				
				rectF sdlDest = rectF(each.second.x + dest.x, each.second.y + dest.y, dest.w, dest.h);

				mTexture->draw(renderer, sdlSrc, sdlDest);
			}
		}
		void set_text(std::string_view string) {
			letterPos.clear();
			vec2i dest(0, 0);
			for (char c : string) {
				
				//std::pair<vec2f, vec2f> textPos;

				if (c == '\n') {
					//if new line start in the same position on x axis but below offset by 1 amount of height
					dest = vec2i(0, dest.y += mGylphHeight);
					continue;
				}
				// if char is the empty space key (c == first_ASCII_character), then skip this part as in anycase
				// position is incremented for the next iteration in the loop
				else if (c >= first_ASCII_character + 1 && c <= last_ASCII_character) {
					const int gylphIndex = c - first_ASCII_character;
					const int yGylph = gylphIndex / mColumnsCount;//ASCII math
					const int xGylph = gylphIndex % mColumnsCount;//ASCII math

					letterPos.emplace_back(vec2f(xGylph * mGylphWidth, yGylph * mGylphHeight), dest);
				}
				dest.x += mGylphWidth;
			}
		}
		void clear_text()noexcept {
			letterPos.clear();
		}

	private:
		SequenceM<std::pair<vec2f, vec2f>> letterPos;
		std::shared_ptr<Texture> mTexture;

		const uint32_t mColumnsCount = 0;
		const uint32_t mRowsCount = 0;

		const uint32_t mGylphWidth = 0;
		const uint32_t mGylphHeight = 0;

		static constexpr char first_ASCII_character = ' ';
		static constexpr char last_ASCII_character = '~';
	};

}