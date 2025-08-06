#pragma once

#include <memory>
#include <string_view>
#include "Surface.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"

namespace badEngine {
	class Sprite {

		std::shared_ptr<SDL_Texture> texture = nullptr;
		
		SDL_FRect source{0,0,0,0};
		
		float destWidth=0.0f;
		float destHeight=0.0f;

	public:
		
		Sprite(const Surface& surface, SDL_Renderer* renderer);

		Sprite(SDL_Surface& sdl_surface, SDL_Renderer* renderer);

		Sprite(std::string_view path, SDL_Renderer* renderer);//best version, others are for show

		bool isGood()const;

		inline void setSourceX(float x) { source.x = x; }
		inline void setSourceY(float y) { source.y = y; }
		inline void setSourceW(float w) { source.w = w; }
		inline void setSourceH(float h) { source.h = h; }

		inline void setSource(float x, float y, float w, float h)
		{
			source = { x,y,w,h };
		}

		inline void setStretchWidth(float w) { destWidth = w; }
		inline void setStretchHeight(float h) { destHeight = h; }

		int getSurfaceWidth()const {
			return texture->w;
		}
		int getSurfaceHeight()const {
			return texture->h;
		}


		void DrawTexture(SDL_Renderer* renderer, int x, int y);
	};
}
