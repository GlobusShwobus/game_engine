#pragma once

#include "GraphicsSys.h"

namespace badEngine {

	static constexpr auto SDLTextureDeleter = [](SDL_Texture* t) { if (t)SDL_DestroyTexture(t); };

	class StaticTexture {
		
	public:
		StaticTexture(SDL_Surface& surface, const GraphicsSys& gfx)
		{
			SDL_Texture* txtr = gfx.load_texture_static(&surface);
			assert(txtr != nullptr && "Texture is nullptr");
			mTexture.reset(txtr);
		}
		StaticTexture(std::string_view path, const GraphicsSys& gfx)
		{
			SDL_Texture* txtr = gfx.load_texture_static(path);
			assert(txtr != nullptr && "Texture is nullptr");
			mTexture.reset(txtr);
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

	class TargetTexture {
	public:

		TargetTexture(Uint32 w, Uint32 h, const GraphicsSys& gfx)
		{
			SDL_Texture* txtr = gfx.create_texture_targetable(w, h);
			assert(txtr != nullptr && "Texture is nullptr");
			mTexture.reset(txtr);
		}
		TargetTexture(Uint32 w, Uint32 h, const GraphicsSys& gfx, SDL_Texture* copy_from = nullptr, rectF* src = nullptr, rectF* dest = nullptr)
		{
			SDL_Texture* txtr = gfx.create_texture_targetable(w, h, copy_from, src, dest);
			assert(txtr != nullptr && "Texture is nullptr");
			mTexture.reset(txtr);
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