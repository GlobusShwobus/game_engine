#pragma once

#include "GraphicsSys.h"

namespace badEngine {
	
	static constexpr auto SDLTextureDeleter = [](SDL_Texture* t) { if (t)SDL_DestroyTexture(t); };

	class Texture {

	public:

		Texture(SDL_Surface& surface, const GraphicsSys& gfx)
		{
			SDL_Texture* txtr = gfx.load_texture_static(&surface);
			if (!txtr) {
				// throw error
			}
			mTexture.reset(txtr);
		}

		Texture(std::string_view path, const GraphicsSys& gfx)
		{
			SDL_Texture* txtr = gfx.load_texture_static(path);
			if (!txtr) {
				// throw error
			}

			mTexture.reset(txtr);
		}

		inline rectF get_control_block()const {
			return rectF(0.f, 0.f, (float)mTexture->w, (float)mTexture->h);
		}

		SDL_Texture* const get() {
			return mTexture.get();
		}

		inline bool isNullPtr()const noexcept {
			return mTexture == nullptr;
		}

	private:
		std::unique_ptr<SDL_Texture, decltype(SDLTextureDeleter)> mTexture;
	};
}