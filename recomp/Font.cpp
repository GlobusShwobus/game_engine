#include "Font.h"
#include <assert.h>

namespace badEngine {
	/*
	Font::Font(const std::string& filename, Color chroma)
		:surface(filename),
		gylphWidth(surface.GetWidth() / nColumns),
		gylphHeight(surface.GetHeight() / nRows),
		chroma(chroma)
	{
		assert(gylphWidth * nColumns == surface.GetWidth());
		assert(gylphHeight * nRows == surface.GetHeight());
	}

	REC::Rectangle Font::MapGylphRect(char c) const
	{
		assert(c >= firstChar && c <= lastChar);
		const int gylphIndex = c - firstChar;
		const int yGylph = gylphIndex / nColumns;
		const int xGylph = gylphIndex % nColumns;

		return REC::Rectangle(float(xGylph * gylphWidth), float(yGylph * gylphHeight), float(gylphWidth), float(gylphHeight));
	}

	void Font::DrawText(const std::string& text, const Point& pos, Graphics& gfx) const
	{
		auto curPos = pos;
		for (auto c : text) {
			if (c == '\n') {
				curPos.x = pos.x;
				curPos.y += gylphHeight;
				continue;
			}
			else if (c >= firstChar + 1 && c <= lastChar) {
				gfx.DrawSurface(curPos.x, curPos.y, MapGylphRect(c), { 0,0,800,600 }, surface, chroma);
			}
			curPos.x += gylphWidth;
		}
	}

	void Font::DrawText(const std::string& text, const Point& pos, Graphics& gfx, Color substitute) const
	{
		auto curPos = pos;
		for (auto c : text) {
			if (c == '\n') {
				curPos.x = pos.x;
				curPos.y += gylphHeight;
				continue;
			}
			else if (c >= firstChar + 1 && c <= lastChar) {

				gfx.DrawSpriteSubstitute(curPos.x, curPos.y, substitute, MapGylphRect(c), { 0,0,800,600 }, surface, chroma);
			}
			curPos.x += gylphWidth;
		}
	}
	*/
}