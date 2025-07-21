#include "Font.h"
#include <assert.h>

namespace badEngine {

	Font::Font(std::string_view path, SDL_Renderer* renderer, int columns, int rows)
		:sprite(path, renderer),
		nColumns(columns),
		nRows(rows),
		gylphWidth(sprite.getSurfaceWidth()/columns),
		gylphHeight(sprite.getSurfaceHeight()/rows)
	{
		assert(gylphWidth * nColumns == sprite.getSurfaceWidth());
		assert(gylphHeight * nRows == sprite.getSurfaceHeight());

		sprite.setSourceW((float)gylphWidth);
		sprite.setSourceH((float)gylphHeight);
		sprite.setStretchWidth((float)gylphWidth);
		sprite.setStretchHeight((float)gylphHeight);
	}

	void Font::DrawFont(std::string_view text, SDL_Renderer* renderer, int x, int y)
	{
		int curX = x;
		int curY = y;

		for (char c : text) {
			if (c == '\n') {
				curX = x;
				curY += gylphHeight;
				continue;
			}
			else if (c >= firstChar + 1 && c <= lastChar) {
				const int gylphIndex = c - firstChar;
				const int yGylph = gylphIndex / nColumns;
				const int xGylph = gylphIndex % nColumns;

				sprite.setSourceX(float(xGylph * gylphWidth));
				sprite.setSourceY(float(yGylph * gylphHeight));

				sprite.DrawTexture(renderer, curX, curY);
			}
			curX += gylphWidth;//if char is the empty space key, this by default skips over it and adds padding as well
		}
	}
}