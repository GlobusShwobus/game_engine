#pragma once

#include <memory>
#include <string_view>
#include "Rectangle.h"
#include "SDL3/SDL_render.h"
#include <SDL3_image/SDL_image.h>

namespace badEngine {
	
	//to be moved to factory
	static constexpr auto SDLTextureDeleter = [](SDL_Texture* t) {
		if(t)SDL_DestroyTexture(t);
		};
	

	class Sprite {
			
		void on_init_default_set()noexcept;

	public:

		//does not take ownership
		Sprite(std::shared_ptr<SDL_Texture> owner);
		//takes ownership
		Sprite(SDL_Texture* wildRaw);
		//takes ownership
		Sprite(SDL_Surface& surface, SDL_Renderer* rendererRef);
		//takes ownership
		Sprite(std::string_view path, SDL_Renderer* rendererRef);

		void draw(SDL_Renderer* rendererRef);

		bool set_source(const rectF& setter)noexcept;
		void set_destination(const rectF& setter)noexcept;

		bool set_source_position(const vec2f& position)noexcept;
		bool set_source_size(const vec2f& size)noexcept;

		void set_destination_position(const vec2f& position)noexcept;
		bool set_destination_size(const vec2f& size)noexcept;

		inline int get_texture_width()const {
			return mTexture->w;
		}
		inline int get_texture_height()const {
			return mTexture->h;
		}

		inline const rectF& get_source()const noexcept {
			return mSource;
		}
		inline const rectF& get_destination()const noexcept {
			return mDest;
		}
		inline vec2f get_destination_dimensions()const noexcept {
			return vec2f(mDest.w, mDest.h);
		}

	private:
		
		rectF mControlBlock = { 0,0,0,0 };

		rectF mSource = { 0,0,0,0 };
		rectF mDest = { 0,0,0,0 };

		std::shared_ptr<SDL_Texture> mTexture;
	};

}