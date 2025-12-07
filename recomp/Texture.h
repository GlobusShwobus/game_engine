#pragma once

#include "GraphicsSys.h"

namespace badEngine {

	class Texture {
		
		static constexpr auto SDLTextureDeleter = [](SDL_Texture* t) { if (t)SDL_DestroyTexture(t); };

	public:

		Texture(SDL_Surface& surface, const GraphicsSys& gfx)
		{
			SDL_Texture* txtr = gfx.load_texture_static(&surface);
			assert(txtr != nullptr && "Texture is nullptr");
			mTexture.reset(txtr);
		}

		Texture(std::string_view path, const GraphicsSys& gfx)
		{
			SDL_Texture* txtr = gfx.load_texture_static(path);
			assert(txtr != nullptr && "Texture is nullptr");
			mTexture.reset(txtr);
		}

		rectF get_control_block()const noexcept {
			float w = -1, h = -1;
			SDL_GetTextureSize(mTexture.get(), &w,&h);
			return rectF(0.f, 0.f, w, h);
		}

		SDL_Texture* const get()const noexcept {
			return mTexture.get();
		}

		bool isNullPtr()const noexcept {
			return mTexture == nullptr;
		}

	private:
		std::unique_ptr<SDL_Texture, decltype(SDLTextureDeleter)> mTexture;
	};
}