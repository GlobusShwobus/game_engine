#include "GraphicsSys.h"

namespace badEngine {
	void GraphicsSys::do_setup(std::string_view heading, Uint32 width, Uint32 height, SDL_WindowFlags flags)
	{
		//SDL internal init
		if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
			throw BAD_RENDERER_EXCEPTION("SDL Init", SDL_GetError());
		//setup window
		mWindow.reset(
			SDL_CreateWindow(heading.data(), width, height, flags)
		);
		if (!mWindow)
			throw BAD_RENDERER_EXCEPTION("SDL_Window", SDL_GetError());
		//setup renderer
		mRenderer.reset(
			SDL_CreateRenderer(mWindow.get(), nullptr)
		);
		if (!mRenderer)
			throw BAD_RENDERER_EXCEPTION("SDL_Renderer", SDL_GetError());
	}
	void GraphicsSys::init_default()
	{
		try {
			do_setup(default_window_heading, default_window_width, default_window_height, default_window_mode | default_system_engine);
		}
		catch (const BadException& e) {
			reset();
			throw;
		}
	}
	bool GraphicsSys::init_from_config(const nlohmann::json& windowConfig) 
	{
		try {
			const auto& config = windowConfig["GameSysConfig"];

			std::string heading = config["heading"];
			Uint32 width = config["window_width"];
			Uint32 height = config["window_height"];
			Uint64 engine = config["engine"];
			Uint64 windowMode = config["mode"];
			//if didnt throw thus far then
			do_setup(heading, width, height, engine | windowMode);
		}
		catch (const nlohmann::json::exception& e) {
			throw BAD_RENDERER_EXCEPTION("JSON EXCEPTION", e.what());
		}
		catch (const BadException& e) {
			reset();
			throw;
		}
	}
	void GraphicsSys::reset()noexcept
	{
		mRenderer.reset();
		mWindow.reset();
		mDrawColor = Colors::Black;
		SDL_Quit();
	}
	bool GraphicsSys::set_render_blend_mode(SDL_BlendMode mode)noexcept
	{
		return SDL_SetRenderDrawBlendMode(mRenderer.get(), mode);
	}
	bool GraphicsSys::set_render_draw_color(Color color)noexcept
	{
		if (SDL_SetRenderDrawColor(mRenderer.get(), color.get_red(), color.get_green(), color.get_blue(), color.get_alpha())) {
			mDrawColor = color;
			return true;
		}
		return false;
	}
	void GraphicsSys::fill_area_with(const rectF& area, Color color) 
	{
		SDL_Renderer* ren = mRenderer.get();
		SDL_SetRenderDrawColor(ren, color.get_red(), color.get_green(), color.get_blue(), color.get_alpha());
		SDL_FRect sdlArea = SDL_FRect(area.x, area.y, area.w, area.h);
		SDL_RenderFillRect(ren, &sdlArea);
		SDL_SetRenderDrawColor(ren, mDrawColor.get_red(), mDrawColor.get_green(), mDrawColor.get_blue(), mDrawColor.get_alpha());
	}
	bool GraphicsSys::set_render_target(SDL_Texture* target)
	{
		return target != nullptr && SDL_SetRenderTarget(mRenderer.get(), target);
	}
	bool GraphicsSys::renderer_present()
	{
		SDL_Renderer* ren = mRenderer.get();
		if (SDL_GetRenderTarget(ren) != nullptr)
			SDL_SetRenderTarget(ren, nullptr);//on setting to null should never error

		return SDL_RenderPresent(ren);
	}
	bool GraphicsSys::renderer_refresh() 
	{
		SDL_Renderer* ren = mRenderer.get();
		if (SDL_GetRenderTarget(ren) != nullptr)
			SDL_SetRenderTarget(ren, nullptr);

		//clears everything to current draw color
		return SDL_RenderClear(ren);
	}

}


