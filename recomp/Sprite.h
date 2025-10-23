#pragma once

#include <memory>
#include <string_view>
#include "Rectangle.h"
#include <stdexcept>
#include "SDL3/SDL_render.h"
#include <SDL3_image/SDL_image.h>

namespace badEngine {
	
	//to be moved to factory
	static constexpr auto SDLTextureDeleter = [](SDL_Texture* t) {
		if(t)SDL_DestroyTexture(t);
		};
	

	class Sprite {
			
		void on_init_default_dimensions() {
			mSource = rectF(0, 0, (float)mTexture->w, (float)mTexture->h);
			mDestination = rectF(0, 0, (float)mTexture->w, (float)mTexture->h);
		}

	public:

		//does not take ownership
		Sprite(std::shared_ptr<SDL_Texture> owner):mTexture(owner) {
			if (mTexture == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");
			on_init_default_dimensions();
		}

		//takes ownership
		Sprite(SDL_Texture* wildRaw) {
			if (wildRaw == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");

			mTexture = std::shared_ptr<SDL_Texture>(wildRaw, SDLTextureDeleter);
			on_init_default_dimensions();
		}
		//takes ownership
		Sprite(SDL_Surface& surface, SDL_Renderer* rendererRef) {
			mTexture = std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(rendererRef, &surface), SDLTextureDeleter);
			if (mTexture == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");
			on_init_default_dimensions();
		}
		//takes ownership
		Sprite(std::string_view path, SDL_Renderer* rendererRef) {
			mTexture = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(rendererRef, path.data()), SDLTextureDeleter);
			if (mTexture == nullptr)
				throw std::runtime_error("Failed to create an SDL_Texture");
			on_init_default_dimensions();
		}

		//THE IMPORTANT SHIT
		void draw(SDL_Renderer* rendererRef) {
			SDL_FRect source = SDL_FRect(mSource.x, mSource.y, mSource.w, mSource.h);
			SDL_FRect dest = SDL_FRect(mDestination.x, mDestination.y, mDestination.w, mDestination.h);

			SDL_RenderTexture(rendererRef, mTexture.get(), &source, &dest );
		}
		//##########################################################################

		void set_source(const rectF& source)noexcept {
			mSource = source;
		}
		void set_destination(const rectF& destination)noexcept {
			mDestination = destination;
		}

		void set_source_position(const vec2f& position)noexcept {
			mSource.set_XY(position);
		}
		void set_source_size(const vec2f& size)noexcept {
			mSource.set_WH(size);
		}

		void set_destination_position(const vec2f& position)noexcept {
			mDestination.set_XY(position);
		}
		void set_destination_size(const vec2f& size)noexcept {
			mDestination.set_WH(size);
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
			return vec2f(mDestination.w, mDestination.h);
		}

	private:
		
		rectF mSource = { 0,0,0,0 };
		rectF mDestination = { 0,0,0,0 };

		std::shared_ptr<SDL_Texture> mTexture;
	};

}