#pragma once

#include <memory>
#include "SDL3/SDL.h"
#include <SDL3_image/SDL_image.h>
#include "Color.h"
#include "Rectangle.h"
#include "json.hpp"
#include "BadExceptions.h"
#include "SequenceM.h"

namespace badEngine {
#define BAD_RENDERER_EXCEPTION(type,note) BadException(__FILE__, __LINE__,type,note)

	class GraphicsSys {

		static constexpr auto SDLWindowDeleter = [](SDL_Window* window)noexcept {
			if (window) SDL_DestroyWindow(window);
			};


		static constexpr auto SDLRendererDeleter = [](SDL_Renderer* renderer)noexcept {
			if (renderer) SDL_DestroyRenderer(renderer);
			};


		void do_setup(std::string_view heading, Uint32 width, Uint32 height, SDL_WindowFlags flags);

		inline SDL_FRect convert_rect(const rectF& rect)const noexcept {
			return SDL_FRect(rect.x, rect.y, rect.w, rect.h);
		}

	public:

		GraphicsSys() {
			init_default();
		}
		GraphicsSys(const nlohmann::json& windowConfig) {
			init_from_config(windowConfig);
		}
		GraphicsSys(const GraphicsSys&) = delete;
		GraphicsSys(GraphicsSys&&)noexcept = delete;
		GraphicsSys& operator=(const GraphicsSys&) = delete;
		GraphicsSys& operator=(GraphicsSys&&)noexcept = delete;

		~GraphicsSys() {
			reset();
		}
		//may throw
		void init_default();
		//if ALL required config fields exist, may throw on creation
		//otherwise returns a bool. for specific failure reason call SDL_GetError
		bool init_from_config(const nlohmann::json& windowConfig);
		//shuts down all sub systems and SDL
		void reset()noexcept;

		//on fail call SDL_GetError()
		bool set_render_blend_mode(SDL_BlendMode mode)const noexcept;
		//on fail call SDL_GetError()
		bool set_render_draw_color(Color color)noexcept;

		
		void fill_area_with(const rectF& area, Color color)const noexcept;

		//overrides location that is being drawn on. 
		//Texture will be permanently overwritten in memory, so make a copy or just keep in mind to reload a clean slate
		//must be called each frame because renderer_present will reset it
		//useful for soemthing like a worldmap to have permanent changes
		bool set_render_target(SDL_Texture* target);
		
		bool renderer_present()const noexcept;
		bool renderer_refresh()const noexcept;


		//returns texture or nullptr on failure, call SDL_GetError
		SDL_Texture* load_texture_static(SDL_Surface* surface)const noexcept;
		//returns texture or nullptr on failure, call SDL_GetError
		SDL_Texture* load_texture_static(std::string_view path)const noexcept;
		//creates a new texture set as targetable, meaning it can be set as render target
		//params width and height are mandatory
		//params copy from, src, dest dictate what texture to copy from, the copied area source and destination
		//by default if src is nullptr, it copies the whole texture and destination is by default same as src
		//NOTE:: if source and/or dest are out of bounds, default SDL behavior occurs meaning things get cliped or simply not drawn at all
		SDL_Texture* create_texture_targetable(Uint32 width, Uint32 height, SDL_Texture* copy_from = nullptr, rectF* src = nullptr, rectF* dest = nullptr)const noexcept;
		//draws a source rectangle from texture to the position of dest on current rendering target
		//returns false on failure, call SDL_GetError
		bool draw(SDL_Texture* texture, const rectF& source, const rectF& dest)const noexcept;
		//draws pairs of sources and destinations from texture to current rendering target
		//returns false on failure, call SDL_GetError
		bool draw(SDL_Texture* texture, const SequenceM<std::pair<rectF, rectF>>& list)const noexcept;

	private:
		/* ORDER MATTERS BECAUSE OF DELETER! */
		std::unique_ptr<SDL_Renderer, decltype(SDLRendererDeleter)> mRenderer{nullptr, SDLRendererDeleter };
		std::unique_ptr<SDL_Window, decltype(SDLWindowDeleter)>     mWindow{nullptr, SDLWindowDeleter };
		Color mDrawColor = Colors::Black;

		static constexpr std::string_view default_window_heading = "DEFAULT HEADING";
		static constexpr size_t default_window_width  = 960;
		static constexpr size_t default_window_height = 540;
		static constexpr size_t default_window_mode   = SDL_WINDOW_RESIZABLE;
		static constexpr size_t default_system_engine = SDL_WINDOW_OPENGL;
	};
}