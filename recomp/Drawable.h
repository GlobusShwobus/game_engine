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
			: mTexture(texture, SDLTextureDeleter)
		{}

		Texture(SDL_Surface& surface, SDL_Renderer* rendererRef)
			:mTexture(SDL_CreateTextureFromSurface(rendererRef, &surface), SDLTextureDeleter)
		{}

		Texture(std::string_view path, SDL_Renderer* rendererRef)
			: mTexture(IMG_LoadTexture(rendererRef, path.data()), SDLTextureDeleter)
		{}

		rectF get_control_block()const noexcept {
			return rectF(0, 0, mTexture->w, mTexture->h);
		}

		bool draw(SDL_Renderer* renderer, const rectF& source, const rectF& dest)noexcept {
			SDL_FRect sdlSrc = convert_rect(source);
			SDL_FRect sdlDest = convert_rect(dest);

			int screenW, screenH;
			SDL_GetRenderOutputSize(renderer, &screenW, &screenH);

			//if obj is fully off screen skip any further rendering
			if (sdlDest.x + sdlDest.w <= 0 || sdlDest.y + sdlDest.h <= 0 || sdlDest.x >= screenW || sdlDest.y >= screenH)
				return true;
			return SDL_RenderTexture(renderer, mTexture.get(), &sdlSrc, &sdlDest);
		}
		bool isNullPtr()const noexcept {
			return mTexture == nullptr;
		}

	private:
		std::unique_ptr<SDL_Texture, decltype(SDLTextureDeleter)> mTexture;
	};


	class Sprite {
	
	protected:

		Sprite(Texture* texture):mTexture(std::shared_ptr<Texture>(texture)) {}
		Sprite(std::shared_ptr<Texture> texture):mTexture(std::move(texture)) {}

		void draw(SDL_Renderer* renderer, std::string* err = nullptr)noexcept {
			if (!mTexture->draw(renderer, mSource, mDest))
				if (err)
					*err = SDL_GetError();
		}
		//source managment should be restricted to inheritence because the assumption is
		//designing future derived classes the user knows to check bounds using is_within_bounds themselves at setup

		inline void set_source_pos(const vec2f& pos)noexcept {
			mSource.set_pos(pos);
		}
		inline void set_source_size(const vec2f& size)noexcept {
			mSource.set_size(size);
		}

	public:

		virtual ~Sprite() = default;
		
		bool is_within_bounds(const rectF& rect)const noexcept {
			return mTexture->get_control_block().contains_rect(rect);
		}

		const rectF& get_source()const noexcept {
			return mSource;
		}
		const rectF& get_dest()const noexcept {
			return mDest;
		}
		const rectF& get_bounds()const noexcept {
			return mTexture->get_control_block();
		}

		inline void set_dest_pos(const vec2f& pos)noexcept {
			mDest.set_pos(pos);
		}		
		inline void set_dest_size(const vec2f& size)noexcept {
			mDest.set_size(size);
		}
	private:

		std::shared_ptr<Texture> mTexture;
		rectF mSource;
		rectF mDest;
	};

	class Animation :public Sprite {

	public:
		Animation(Texture* texture, const vec2i& start, uint16_t fWidth, uint16_t fHeight, uint16_t fCount)
			:Animation(std::shared_ptr<Texture>(texture), start, fWidth, fHeight, fCount) {
		}
		Animation(std::shared_ptr<Texture> texture, const vec2i& start, uint16_t fWidth, uint16_t fHeight, uint16_t fCount)
			:Sprite(std::move(texture)), mFrameCount(fCount)
		{
			assert(start.x >= 0 && start.y >= 0 && "Out of bounds texture access");

			const rectF requiredArea = rectF(
				start.x,
				start.y,
				start.x + (fWidth * fCount),
				start.y + (fHeight * fCount)
			);
			
			//check if the entire demand is within the control block
			assert(is_within_bounds(requiredArea) && "demanded size too large for this texture");

			for (uint16_t i = 0; i < fCount; ++i)
				mFrames.emplace_back(start.x + (i * fWidth), start.y);
			set_source_pos(mFrames.front());
			set_source_size(vec2f(fWidth, fHeight));

			set_dest_size(vec2f(fWidth, fHeight));//default inital
		}

		void draw(SDL_Renderer* renderer, const vec2f& pos) {
			//source size is set in constructor with frame width, height, not a good idea to willynilly edit that
			set_source_pos(mFrames[mCurrentFrame]);
			set_dest_pos(pos);

			Sprite::draw(renderer);
		}

		void progress(float dt)noexcept {
			mCurrentFrameTime += dt;
			while (mCurrentFrameTime >= mHoldTime) {
				next_frame();
				mCurrentFrameTime -= mHoldTime;
			}
		}
		inline void set_frame_hold_time(float time)noexcept {
			assert(time >= 0 && "negative time");
			mHoldTime = time;
		}

	private:

		inline void next_frame()noexcept {
			++mCurrentFrame;
			if (mCurrentFrame >= mFrameCount)
				mCurrentFrame = 0;
		}

	private:
		SequenceM<vec2i> mFrames;

		uint16_t mFrameCount = 0;
		uint16_t mCurrentFrame = 0;

		float mHoldTime = 0.08f;
		float mCurrentFrameTime = 0.0f;
	};

	class Font : public Sprite {

		static constexpr char first_ASCII_character = ' ';
		static constexpr char last_ASCII_character = '~';

		struct LetterPositions {
			vec2f mSourcePos;
			vec2f mDestPos;
		};

	public:

		Font(Texture* texture, uint32_t columnsCount, uint32_t rowsCount)
			:Font(std::shared_ptr<Texture>(texture), columnsCount, rowsCount) {
		}

		Font(std::shared_ptr<Texture> texture, uint32_t columnsCount, uint32_t rowsCount)
			:Sprite(std::move(texture)),
			mColumnsCount(columnsCount),
			mRowsCount(rowsCount)
		{
			rectF textureBounds = get_bounds();

			const int GylphWidth = int(textureBounds.w / columnsCount);
			const int GylphHeight = int(textureBounds.h / rowsCount);
			//becasue int vs float
			assert(GylphWidth * columnsCount == textureBounds.w && "texture image likely off size or invalid counts");
			assert(GylphHeight * rowsCount == textureBounds.h && "texture image likely off size or invalid counts");
			set_source_size(vec2f(GylphWidth, GylphHeight));
			set_dest_size(vec2f(GylphWidth, GylphHeight));
		}
		void draw(SDL_Renderer* renderer, const vec2f& pos) {
			for (const auto& letter : mLetterPos) {
				set_source_pos(letter.mSourcePos);
				set_dest_pos(letter.mDestPos);
				Sprite::draw(renderer);
			}
		}
		void set_text(std::string_view string) {
			clear_text();
			vec2i arrangement(0, 0);
			vec2f gylphSize = get_source().get_size();

			for (char c : string) {

				if (c == '\n') {
					//if new line start in the same position on x axis but below offset by 1 amount of height
					arrangement = vec2i(0, arrangement.y += gylphSize.y);
					continue;
				}
				// if char is the empty space key (c == first_ASCII_character), then skip this part as in anycase
				// position is incremented for the next iteration in the loop
				else if (c >= first_ASCII_character + 1 && c <= last_ASCII_character) {
					const int gylphIndex = c - first_ASCII_character;
					const int yGylph = gylphIndex / mColumnsCount;//ASCII math
					const int xGylph = gylphIndex % mColumnsCount;//ASCII math

					mLetterPos.emplace_back(
						vec2f(xGylph * gylphSize.x, yGylph * gylphSize.y),
						arrangement
					);
				}
				arrangement.x += gylphSize.x;
			}
		}
		void clear_text()noexcept {
			mLetterPos.clear();
		}

	private:
		SequenceM<LetterPositions> mLetterPos;

		const uint32_t mColumnsCount = 0;
		const uint32_t mRowsCount = 0;
	};

}
