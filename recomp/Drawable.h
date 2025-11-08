#pragma once

#include "Texture.h"
#include "SequenceM.h"
#include "Rectangle.h"
#include <assert.h>

namespace badEngine {

	class Sprite {

	public:

		Sprite(Texture* texture)
		:mTexture(std::shared_ptr<Texture>(texture))
		{
			assert(mTexture != nullptr, "Sprite texture nullptr");
			mControlBlock = rectF(0, 0, mTexture->width(), mTexture->height());
		}
		Sprite(std::shared_ptr<Texture> texture)
		:mTexture(std::move(texture))
		{
			assert(mTexture != nullptr, "Sprite texture nullptr");
			mControlBlock = rectF(0, 0, mTexture->width(), mTexture->height());
		}

		virtual ~Sprite() = default;
		virtual void draw(SDL_Renderer* renderer, const vec2f& pos) = 0;

	protected:

		bool is_within_control_block(const rectF& rect)const noexcept {
			return mControlBlock.contains_rect(rect);
		}

		SDL_Texture* get_texture()const {
			return mTexture->get();
		}

		void set_source_size(const vec2f& size)noexcept {
			assert(size.x <= mTexture->width() && size.y <= mTexture->height());
			mSourceSize = size;
		}
		const vec2f& get_source_size()const noexcept {
			return mSourceSize;
		}
		void set_dest_size(const vec2f& size)noexcept {
			mDestSize = size;
		}
		const vec2f& get_dest_size()const noexcept {
			return mDestSize;
		}


	private:
		rectF mControlBlock = { 0,0,0,0 };
		std::shared_ptr<Texture> mTexture;
		vec2f mSourceSize;
		vec2f mDestSize;
	};

	class Animation: public Sprite {

	public:

		Animation(Texture* texture, const vec2i& start, uint16_t fWidth, uint16_t fHeight, uint16_t fCount)
			:Sprite(texture), mFrameCount(fCount)
		{
			record_frames(start, fWidth, fHeight, fCount);
		}
		Animation(std::shared_ptr<Texture> texture, const vec2i& start, uint16_t fWidth, uint16_t fHeight, uint16_t fCount)
			:Sprite(std::move(texture)), mFrameCount(fCount)
		{
			record_frames(start, fWidth, fHeight, fCount);
		}
		void record_frames(const vec2i& start, uint16_t fWidth, uint16_t fHeight, uint16_t fCount) {

			assert(start.x >= 0 && start.y >= 0, "Out of bounds texture access");

			const rectF controlBlock = rectF(
				start.x,
				start.y,
				start.x + (fWidth * fCount),
				start.y + (fHeight * fCount)
			);

			assert(is_within_control_block(controlBlock), "texture does not support film of such size");

			for (uint16_t i = 0; i < fCount; ++i)
				mFrames.emplace_back(start.x + (i * fWidth), start.y);

			set_source_size(vec2f(fWidth, fHeight));
			set_dest_size(vec2f(fWidth, fHeight));

			mFrameCount = fCount;
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
		void set_frame_hold_time(float time) {
			assert(time >= 0, "negative time");
			mHoldTime = time;
		}
		void draw(SDL_Renderer* renderer, const vec2f& pos) override {
			const auto& sSize = get_source_size();
			const auto& dSize = get_dest_size();
			const auto& curFramePos = mFrames[mCurrentFrame];
			
			SDL_FRect source = SDL_FRect(curFramePos.x, curFramePos.y, sSize.x, sSize.y);
			SDL_FRect dest = SDL_FRect(pos.x, pos.y, dSize.x, dSize.y);

			int screenW, screenH;
			SDL_GetRenderOutputSize(renderer, &screenW, &screenH);

			//if obj is fully off screen skip any further rendering
			if (dest.x + dest.w <= 0 || dest.y + dest.h <= 0 || dest.x >= screenW || dest.y >= screenH)
				return;

			//SDL handles cliping to window
			SDL_RenderTexture(renderer, get_texture(), &source, &dest);
		}
	private:
		SequenceM<vec2i> mFrames;

		uint16_t mFrameCount = 0;
		uint16_t mCurrentFrame = 0;

		float mHoldTime = 0.08f;
		float mCurrentFrameTime = 0.0f;
	};


	class Font :public Sprite {
		void init(uint32_t columnsCount, uint32_t rowsCount) {
			mGylphWidth = get_texture()->w / mColumnsCount;
			mGylphHeight = get_texture()->h / mRowsCount;

			assert((mGylphWidth * mColumnsCount) == get_texture()->w, "texture image likely off size or invalid counts");
			assert((mGylphHeight * mRowsCount) == get_texture()->h, "texture image likely off size or invalid counts");
		
			set_source_size(vec2f(mGylphWidth, mGylphHeight));
			set_dest_size(vec2f(mGylphWidth, mGylphHeight));
		}
	public:

		Font(Texture* texture, uint32_t columnsCount, uint32_t rowsCount)
			:Sprite(texture), mColumnsCount(columnsCount), mRowsCount(rowsCount)
		{
			init(columnsCount, rowsCount);
		}

		Font(std::shared_ptr<Texture> texture, uint32_t columnsCount, uint32_t rowsCount)
			:Sprite(std::move(texture)), mColumnsCount(columnsCount), mRowsCount(rowsCount)
		{
			init(columnsCount, rowsCount);
		}

		void draw(SDL_Renderer* renderer, const vec2f& pos)override {
			
			const auto& sSize = get_source_size();
			const auto& dSize = get_dest_size();
			
			for (const auto& each : textPos) {

				SDL_FRect source = SDL_FRect(each.first.x, each.first.y, sSize.x, sSize.y);
				SDL_FRect dest = SDL_FRect(each.second.x + pos.x, each.second.y + pos.y, dSize.x, dSize.y);

				int screenW, screenH;
				SDL_GetRenderOutputSize(renderer, &screenW, &screenH);

				//if obj is fully off screen skip any further rendering
				if (dest.x + dest.w <= 0 || dest.y + dest.h <= 0 || dest.x >= screenW || dest.y >= screenH)
					continue;

				//SDL handles cliping to window
				SDL_RenderTexture(renderer, get_texture(), &source, &dest);
			}
		}
		void set_text(std::string_view string) {
			textPos.clear();
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

					textPos.emplace_back(vec2f(xGylph * mGylphWidth, yGylph * mGylphHeight), dest);
				}
				dest.x += mGylphWidth;
			}
		}
		void clear_text()noexcept {
			textPos.clear();
		}

	private:
		SequenceM<std::pair<vec2f, vec2f>> textPos;
		const uint32_t mColumnsCount = 0;
		const uint32_t mRowsCount = 0;

		uint32_t mGylphWidth = 0;
		uint32_t mGylphHeight = 0;

		static constexpr char first_ASCII_character = ' ';
		static constexpr char last_ASCII_character = '~';
	};

}