#pragma once

#include <memory>
#include <string_view>
#include "Rectangle.h"
#include <stdexcept>
#include "SDL3/SDL_render.h"
#include <SDL3_image/SDL_image.h>

namespace badEngine {

	static SDL_FRect rectF_to_SDL_FRect(const rectF& rect)noexcept {
		return SDL_FRect(rect.mPosition.x, rect.mPosition.y, rect.mDimensions.x, rect.mDimensions.y);
	}


	class Sprite {
		
		struct SDLTextureDeleter {
			void operator()(SDL_Texture* t)const {
				SDL_DestroyTexture(t);
			}
		};
	
		void on_init_default_dimensions() {
			mSource = rectF(0, 0, (float)mTexture->w, (float)mTexture->h);
			mDestination = rectF(0, 0, (float)mTexture->w, (float)mTexture->h);
		}

	public:

		Sprite(SDL_Texture* texture) :mTexture(texture) {
			if (mTexture == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");
			on_init_default_dimensions();
		}
		Sprite(SDL_Surface& surface, SDL_Renderer* renderer) {
			mTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(SDL_CreateTextureFromSurface(renderer, &surface));
			if (mTexture == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");
			on_init_default_dimensions();
		}
		Sprite(std::string_view path, SDL_Renderer* renderer) {
			mTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(IMG_LoadTexture(renderer, path.data()));
			if (mTexture == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");
			on_init_default_dimensions();
		}

		//THE IMPORTANT SHIT
		void draw(SDL_Renderer* renderer) {
			SDL_FRect source = rectF_to_SDL_FRect(mSource);
			SDL_FRect dest = rectF_to_SDL_FRect(mDestination);

			SDL_RenderTexture( renderer, mTexture.get(), &source, &dest );
		}
		//##########################################################################

		template <typename T>
		void set_source(const Rectangle<T>& source)noexcept {
			mSource = source;
		}
		template <typename T>
		void set_destination(const Rectangle<T>& destination)noexcept {
			mDestination = destination;
		}


		template<typename T>
		void set_source_position(const Vec2M<T>& position)noexcept {
			mSource.mPosition = position;
		}
		template<typename T>
		void set_source_size(const Vec2M<T>& size)noexcept {
			mSource.mDimensions = size;
		}
		template<typename T>
		void set_destination_position(const Vec2M<T>& position)noexcept {
			mDestination.mPosition = position;
		}
		template<typename T>
		void set_destination_size(const Vec2M<T>& size)noexcept {
			mDestination.mDimensions = size;
		}


		int get_texture_width()const {
			return mTexture->w;
		}
		int get_texture_height()const {
			return mTexture->h;
		}

		const rectF& get_source()const noexcept {
			return mSource;
		}
		const rectF& get_destination()const noexcept {
			return mDestination;
		}
		vec2f get_destination_dimensions()const noexcept {
			return mDestination.mDimensions;
		}

	private:
		
		rectF mSource = { 0,0,0,0 };
		rectF mDestination = { 0,0,0,0 };

		std::unique_ptr<SDL_Texture, SDLTextureDeleter> mTexture;
	};

}