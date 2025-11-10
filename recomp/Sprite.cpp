#include "Sprite.h"

namespace badEngine {
	Sprite::Sprite(Texture* texture) :Sprite(std::shared_ptr<Texture>(texture)) {}
	Sprite::Sprite(std::shared_ptr<Texture> texture) :mTexture(std::move(texture)) {}

	void Sprite::draw(SDL_Renderer* renderer, std::string* err)noexcept {
		if (!mTexture->draw(renderer, mSource, mDest))
			if (err)
				*err = SDL_GetError();
	}
	bool Sprite::is_within_bounds(const rectF& rect)const noexcept {
		return mTexture->get_control_block().contains_rect(rect);
	}
	//####################################################################################

	Animation::Animation(Texture* texture, const vec2i& start, uint16_t fWidth, uint16_t fHeight, uint16_t fCount)
		:Animation(std::shared_ptr<Texture>(texture), start, fWidth, fHeight, fCount) {
	}
	Animation::Animation(std::shared_ptr<Texture> texture, const vec2i& start, uint16_t fWidth, uint16_t fHeight, uint16_t fCount)
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

	void Animation::draw(SDL_Renderer* renderer, const vec2f& pos) noexcept {
		//source size is set in constructor with frame width, height, not a good idea to willynilly edit that
		set_source_pos(mFrames[mCurrentFrame]);
		set_dest_pos(pos);

		Sprite::draw(renderer);
	}
	void Animation::progress(float dt)noexcept {
		mCurrentFrameTime += dt;
		while (mCurrentFrameTime >= mHoldTime) {
			next_frame();
			mCurrentFrameTime -= mHoldTime;
		}
	}
	//#########################################################################################

	Font::Font(Texture* texture, uint32_t columnsCount, uint32_t rowsCount)
		:Font(std::shared_ptr<Texture>(texture), columnsCount, rowsCount) {
	}

	Font::Font(std::shared_ptr<Texture> texture, uint32_t columnsCount, uint32_t rowsCount)
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
	void Font::draw(SDL_Renderer* renderer, const vec2f& pos) {
		for (const auto& letter : mLetterPos) {
			set_source_pos(letter.mSourcePos);
			set_dest_pos(letter.mDestPos);
			Sprite::draw(renderer);
		}
	}
	void Font::set_text(std::string_view string)noexcept {
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
	void Font::clear_text()noexcept {
		mLetterPos.clear();
	}
}