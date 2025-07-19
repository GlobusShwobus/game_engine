
#include "Surface.h"
#include "BMPHeader.h"
#include <cassert>
#include <fstream>
#include <vector>

namespace badEngine {
	Surface::Surface(const std::string& filePath)
	{
		BMPFileHeader bmFileHeader;
		BMPInfoHeader bmInfoHeader;

		std::ifstream file(filePath, std::ios::binary);
		assert(file);

		file.read(reinterpret_cast<char*>(&bmFileHeader), sizeof(bmFileHeader));
		file.read(reinterpret_cast<char*>(&bmInfoHeader), sizeof(bmInfoHeader));

		assert(bmInfoHeader.biBitCount == 24 || bmInfoHeader.biBitCount == 32);
		assert(bmInfoHeader.biCompression == static_BI_RGB);

		width = bmInfoHeader.biWidth;
		height = bmInfoHeader.biHeight;

		//handle case if height is negative, otherwise we read ass up
		int yStart;
		int yEnd;
		int dy;

		if (height < 0) {
			yStart = 0;
			yEnd = -height;
			dy = 1;
			height = -height;
		}
		else {
			yStart = height - 1;
			yEnd = -1;
			dy = -1;
		}
		pPixels = new Color[width * height];

		file.seekg(bmFileHeader.bfOffBits);
		// padding is per row not per pixel
		// if width is 123 pixels, 3 bytes per pixel (24 bits) == 369 bytes
		// 369 modulated by 4 is 1, 1 byte 'overweight'
		// 4-1 = 3. need to add 3 bytes of padding
		// edge case is if the row is already a multiple of 4, in which case it becomes 4-0 = 4 bytes padding
		// in this case the second modulation saves the day. the second modulation does jackshit otherwise
		//padding also only makes sense for 24 bit things
		const int padding = (4 - (width * 3) % 4) % 4;
		const bool is32b = bmInfoHeader.biBitCount == 32;

		for (int y = yStart; y != yEnd; y += dy)
		{
			for (int x = 0; x < width; x++)
			{
				unsigned char b = file.get();
				unsigned char g = file.get();
				unsigned char r = file.get();

				if (is32b) {
					unsigned char a = file.get();
					putPixel(x, y, Color(a, r, g, b));
				}
				else {
					putPixel(x, y, Color(255, r, g, b));
				}
			}

			if (!is32b)
				file.seekg(padding, std::ios::cur);
		}
	}

	Surface::Surface(const std::string& filePath, int deleteThisLol)
	{
		BMPFileHeader bmFileHeader;
		BMPInfoHeader bmInfoHeader;

		std::ifstream file(filePath, std::ios::binary);
		assert(file);

		file.read(reinterpret_cast<char*>(&bmFileHeader), sizeof(bmFileHeader));
		file.read(reinterpret_cast<char*>(&bmInfoHeader), sizeof(bmInfoHeader));

		assert(bmInfoHeader.biBitCount == 24 || bmInfoHeader.biBitCount == 32);
		assert(bmInfoHeader.biCompression == static_BI_RGB);

		width = bmInfoHeader.biWidth;
		height = bmInfoHeader.biHeight;

		//handle case if height is negative, otherwise we read ass up
		int yStart;
		int yEnd;
		int dy;

		if (height < 0) {
			yStart = 0;
			yEnd = -height;
			dy = 1;
			height = -height;
		}
		else {
			yStart = height - 1;
			yEnd = -1;
			dy = -1;
		}
		pPixels = new Color[width * height];

		file.seekg(bmFileHeader.bfOffBits);
		// padding is per row not per pixel
		// if width is 123 pixels, 3 bytes per pixel (24 bits) == 369 bytes
		// 369 modulated by 4 is 1, 1 byte 'overweight'
		// 4-1 = 3. need to add 3 bytes of padding
		// edge case is if the row is already a multiple of 4, in which case it becomes 4-0 = 4 bytes padding
		// in this case the second modulation saves the day. the second modulation does jackshit otherwise
		//padding also only makes sense for 24 bit things
		const int padding = (4 - (width * 3) % 4) % 4;
		const bool is32b = bmInfoHeader.biBitCount == 32;



		std::vector<unsigned char> rowData(width * (is32b ? 4 : 3));//bytes in row

		for (int y = yStart; y != yEnd; y += dy)
		{
			file.read(reinterpret_cast<char*>(rowData.data()), rowData.size());//into rowData for the amount of bytes
			for (int x = 0; x < width; x++)
			{
				size_t i = x * (is32b ? 4 : 3);//bit index
				unsigned char b = rowData[i];
				unsigned char g = rowData[i+1];
				unsigned char r = rowData[i+2];
				unsigned char a = is32b ? rowData[i + 3] : 255u;
				putPixel(x, y, Color(a, r, g, b));
			}

			if (!is32b)
				file.seekg(padding, std::ios::cur);
		}
	}

	Surface::Surface(int width, int height) :
		width(width), height(height), pPixels(new Color[width * height])
	{}
	Surface::Surface(const Surface& rhs)
		:Surface(rhs.width, rhs.height)
	{
		const int nPixels = width * height;
		for (int i = 0; i < nPixels; i++) {
			pPixels[i] = rhs.pPixels[i];
		}
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