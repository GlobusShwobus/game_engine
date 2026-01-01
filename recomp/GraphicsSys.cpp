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
			throw e;
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
			throw e;
		}
	}
	void GraphicsSys::reset()noexcept
	{
		mRenderer.reset();
		mWindow.reset();
		mDrawColor = Colors::Black;
		SDL_Quit();
	}
	bool GraphicsSys::set_render_blend_mode(SDL_BlendMode mode)const noexcept
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
	void GraphicsSys::fill_area_with(const float4& area, Color color)const noexcept
	{
		SDL_Renderer* ren = mRenderer.get();
		SDL_SetRenderDrawColor(ren, color.get_red(), color.get_green(), color.get_blue(), color.get_alpha());
		SDL_FRect sdlArea = SDL_FRect(area.x, area.y, area.w, area.h);
		SDL_RenderFillRect(ren, &sdlArea);
		SDL_SetRenderDrawColor(ren, mDrawColor.get_red(), mDrawColor.get_green(), mDrawColor.get_blue(), mDrawColor.get_alpha());
	}
	void GraphicsSys::fill_area_with(const float4& outer, const float4& inner, Color color)const noexcept
	{
		if (outer.contains(inner)) {
			SDL_Renderer* ren = mRenderer.get();
			SDL_SetRenderDrawColor(ren, color.get_red(), color.get_green(), color.get_blue(), color.get_alpha());
			SDL_FRect sdlOuter = SDL_FRect(outer.x, outer.y, outer.w, outer.h);
			SDL_FRect sdlInner = SDL_FRect(inner.x, inner.y, inner.w, inner.h);

			SDL_RenderFillRect(ren, &sdlOuter);
			SDL_SetRenderDrawColor(ren, mDrawColor.get_red(), mDrawColor.get_green(), mDrawColor.get_blue(), mDrawColor.get_alpha());
			SDL_RenderFillRect(ren, &sdlInner);
		}
	}
	bool GraphicsSys::set_render_target(SDL_Texture* target)const noexcept
	{
		return SDL_SetRenderTarget(mRenderer.get(), target);
	}
	bool GraphicsSys::renderer_present()const noexcept
	{
		SDL_Renderer* ren = mRenderer.get();
		SDL_SetRenderTarget(ren, nullptr);//on setting to null should never error
		return SDL_RenderPresent(ren);
	}
	bool GraphicsSys::renderer_refresh()const noexcept
	{
		return SDL_RenderClear(mRenderer.get());
	}
	SDL_Texture* GraphicsSys::load_texture_static(SDL_Surface* surface)const noexcept
	{
		return SDL_CreateTextureFromSurface(mRenderer.get(), surface);
	}
	SDL_Texture* GraphicsSys::load_texture_static(std::string_view path)const noexcept
	{
		return IMG_LoadTexture(mRenderer.get(), path.data());
	}
	SDL_Texture* GraphicsSys::create_texture_targetable(Uint32 width, Uint32 height, SDL_Texture* copy_from, float4* src, float4* dest)const noexcept
	{
		SDL_Renderer* ren = mRenderer.get();
		//create texture
		SDL_Texture* texture = SDL_CreateTexture(
			ren,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET,
			width,
			height
		);
		if (!texture)
			return nullptr;
		//set blend mode to blend to read alpha channels, this is default behavior
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

		if (copy_from) {
			//get full size of the copy texture
			float copyTextureW, copyTextureH;
			SDL_GetTextureSize(copy_from, &copyTextureW, &copyTextureH);

			//set copied size, by default whole texture
			SDL_FRect cSrc = (src) ? convert_rect(*src) : SDL_FRect(0, 0, copyTextureW, copyTextureH);
			//set destination size, by default cSrc size
			SDL_FRect cDest = (dest) ? convert_rect(*dest) : cSrc;

			//store current target, if null is fine
			SDL_Texture* oldTarget = SDL_GetRenderTarget(ren);
			//set this texture as target so we copy data onto it
			SDL_SetRenderTarget(ren, texture);
			//copy from copy_from using RenderTexture which renders to current rendering target
			SDL_RenderTexture(ren, copy_from, &cSrc, &cDest);
			//reset target
			SDL_SetRenderTarget(ren, oldTarget);
		}
		return texture;
	}
	bool GraphicsSys::draw(SDL_Texture* texture, const float4& source, const float4& dest)const noexcept
	{
		SDL_FRect sdlSrc = convert_rect(source);
		SDL_FRect sdlDest = convert_rect(dest);
		SDL_Renderer* ren = mRenderer.get();

		int screenW, screenH;
		SDL_GetRenderOutputSize(ren, &screenW, &screenH);

		//if obj is fully off screen skip any further rendering, might be faulty logic though, just keep eyes open
		if (sdlDest.x + sdlDest.w <= 0 || sdlDest.y + sdlDest.h <= 0 || sdlDest.x >= screenW || sdlDest.y >= screenH)
			return true;

		return SDL_RenderTexture(ren, texture, &sdlSrc, &sdlDest);
	}
	bool GraphicsSys::draw(SDL_Texture* texture, const SequenceM<std::pair<float4, float4>>& list)const noexcept
	{

		SDL_Renderer* ren = mRenderer.get();
		int screenW, screenH;
		SDL_GetRenderOutputSize(ren, &screenW, &screenH);

		for (const auto& pair : list) {
			auto src = convert_rect(pair.first);
			auto dest = convert_rect(pair.second);

			if (dest.x + dest.w <= 0 || dest.y + dest.h <= 0 || dest.x >= screenW || dest.y >= screenH)
				return true;
			//if obj is fully off screen skip any further rendering, might be faulty logic though, just keep eyes open
			if (!SDL_RenderTexture(ren, texture, &src, &dest))
				return false;
		}
		return true;
	}
	bool GraphicsSys::draw(SDL_Texture* texture)const noexcept
	{
		return SDL_RenderTexture(mRenderer.get(), texture, nullptr, nullptr);
	}
}


