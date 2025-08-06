#include "Surface.h"
#include "BMPHeader.h"
#include <cassert>
#include <fstream>
#include <vector>

namespace badEngine {
	
	Surface::Surface(std::string_view path)
	{
		BMPFileHeader bmFileHeader;
		BMPInfoHeader bmInfoHeader;

		std::ifstream file(path.data(), std::ios::binary);
		assert(file);

		file.read(reinterpret_cast<char*>(&bmFileHeader), sizeof(bmFileHeader));
		file.read(reinterpret_cast<char*>(&bmInfoHeader), sizeof(bmInfoHeader));

		assert(bmInfoHeader.biBitCount == 24 || bmInfoHeader.biBitCount == 32);

		width = bmInfoHeader.biWidth;
		height = bmInfoHeader.biHeight;

		pPixels = new Color[width * height];

		const int padding = (4 - (width * 3) % 4) % 4;//padding in case bytes are not multiple of 4
		const bool is32b = bmInfoHeader.biBitCount == 32;

		const int bytesPerPixel = is32b ? 4 : 3;
		const int rowStride = is32b ? width * 4 : width * 3 + padding;//bytes per row
		const int imageSize = rowStride * height;

		std::vector<unsigned char> buffer(imageSize);

		file.seekg(bmFileHeader.bfOffBits);
		file.read(reinterpret_cast<char*>(buffer.data()), imageSize);

		for (int y = 0; y < height; ++y) {
			int bmpY = (bmInfoHeader.biHeight > 0) ? (height - 1 - y) : y;//handle both positive or negative pixel layout
			unsigned char* row = buffer.data() + bmpY * rowStride;//beginning of buffer + (height*width) == row

			for (int x = 0; x < width; ++x) {
				unsigned int i = x * bytesPerPixel;
				unsigned char b = row[i];
				unsigned char g = row[i + 1];
				unsigned char r = row[i + 2];
				unsigned char a = is32b ? row[i + 3] : 255u;
				pPixels[y * width + x] = (a << 24u) | (r << 16u) | (g << 8u) | b;
			}
		}
	}
	
	Surface::Surface(int width, int height) :
		width(width), height(height), pPixels(new Color[width * height])
	{}
	Surface::Surface(const Surface& rhs)
	{
		*this = rhs;
	}
	Surface::~Surface()
	{
		delete[]pPixels;
		pPixels = nullptr;
	}
	Surface& Surface::operator=(const Surface& rhs)
	{
		width = rhs.width;
		height = rhs.height;
		delete[] pPixels;

		pPixels = new Color[width * height];

		const int nPixels = width * height;
		for (int i = 0; i < nPixels; i++) {
			pPixels[i] = rhs.pPixels[i];
		}

		return *this;
	}

	void Surface::putPixel(int x, int y, Color c)
	{
		assert(x >= 0);
		assert(x < width);
		assert(y >= 0);
		assert(y < height);
		pPixels[y * width + x] = c;
	}
	Color Surface::getPixel(int x, int y)const
	{
		assert(x >= 0);
		assert(x < width);
		assert(y >= 0);
		assert(y < height);

		return pPixels[y * width + x];
	}
	const Color* const Surface::data()const {
		return pPixels;
	}
	int Surface::getWidth()const 
	{
		return width;
	}
	int Surface::getHeight()const
	{
		return height;
	}
}