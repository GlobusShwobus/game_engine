#pragma once

#include <memory>
#include <string_view>
#include "Vec2M.h"
#include <stdexcept>
#include "SDL3/SDL_render.h"
#include <SDL3_image/SDL_image.h>

namespace badEngine {

	class Sprite {
		
		struct SDLTextureDeleter {
			void operator()(SDL_Texture* t)const {
				SDL_DestroyTexture(t);
			}
		};
	
	public:

		Sprite(SDL_Texture* texture) :mTexture(texture) {
			if (mTexture == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");
		}
		Sprite(SDL_Surface& surface, SDL_Renderer* renderer) {
			mTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(SDL_CreateTextureFromSurface(renderer, &surface));
			if (mTexture == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");
		}
		Sprite(std::string_view path, SDL_Renderer* renderer) {
			mTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(IMG_LoadTexture(renderer, path.data()));
			if (mTexture == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");
		}

		void set_source(SDL_FRect newSource)noexcept {
			mSourcePosition = std::move(newSource);
		}
		template<typename T>
		void set_source_position(const Vec2M<T>& position)noexcept {
			mSourcePosition.x = position.x;
			mSourcePosition.y = position.y;
		}
		template<typename T>
		void set_source_size(const Vec2M<T>& size)noexcept {
			mSourcePosition.w = size.x;
			mSourcePosition.h = size.y;
		}
		template<typename T>
		void set_destination_scale(const Vec2M<T>& scale)noexcept {
			mDestinationScale.x = scale.x;
			mDestinationScale.y = scale.y;
		}
		template<typename T>
		requires IS_MATHMATICAL_T<T>
		void set_destination_scale(T scale)noexcept {
			mDestinationScale.x = scale;
			mDestinationScale.y = scale;
		}
		template<typename T>
		void draw(SDL_Renderer* renderer, const Vec2M<T>& destinationPosition) {
			SDL_FRect dest = { destinationPosition.x, destinationPosition.y, mDestinationScale.x, mDestinationScale.y };
			SDL_RenderTexture(
				renderer,
				mTexture.get(),
				&mSourcePosition,
				&dest
			);
		}
		int texture_width()const {
			return mTexture->w;
		}
		int texture_height()const {
			return mTexture->h;
		}
		const SDL_FRect& get_source()const {
			return mSourcePosition;
		}
		const vec2f& get_scale()const {
			return mDestinationScale;
		}

	private:
		SDL_FRect mSourcePosition = { 0,0,0,0 };
		vec2f     mDestinationScale = { 0.0f,0.0f };

		std::unique_ptr<SDL_Texture, SDLTextureDeleter> mTexture;
	};

}