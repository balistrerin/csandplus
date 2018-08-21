#include "Surface.h"
#include "ChiliWin.h"
#include <cassert>
#include <fstream>

Surface::Surface(const std::string & filename)
{
	//Open up stream and set to binary mode
	std::ifstream file(filename, std::ios::binary);
	//Older BitMap Header without extra properties
	BITMAPFILEHEADER bmFileHeader;
	//read file as array of chars and get the size of file header 
	file.read(reinterpret_cast<char*>(&bmFileHeader), sizeof(bmFileHeader));

	BITMAPINFOHEADER bmInfoHeader;
	file.read(reinterpret_cast<char*>(&bmInfoHeader), sizeof(bmInfoHeader));

	//only deal with 24 bit bitmaps
	assert(bmInfoHeader.biBitCount == 24 || bmInfoHeader.biBitCount == 32);
	//avoid handling compression
	assert(bmInfoHeader.biCompression == BI_RGB);
	
	const bool is32b = bmInfoHeader.biBitCount == 32;

	//set width and height
	width = bmInfoHeader.biWidth;

	//Testing for backwards row order
	int yStart;
	int yEnd;
	int dy;
	if (bmInfoHeader.biHeight < 0)
	{
		height = -bmInfoHeader.biHeight;
		yStart = 0;
		yEnd = height;
		dy = 1;


	}
	else 
	{
		height = bmInfoHeader.biHeight;
		yStart = height - 1;
		yEnd = -1;
		dy = -1;

	}


	//alocated memory
	pPixels = new Color[width*height];

	//seek to where pixel data starts
	file.seekg(bmFileHeader.bfOffBits);

	//calculate padding to account for RGB offsets 
	const int padding = (4 - (width * 3) % 4) % 4;

	//loop from bottom to top becasue bitmaps load in backwards for some freakin reason!
	for (int y = yStart - 1; y != yEnd; y += dy)
	{
		for (int x = 0; x < width; x++)
		{
			PutPixel(x, y, Color(file.get(), file.get(), file.get()));
			//check for 32 bits
			if (is32b)
			{
				file.seekg(1, std::ios::cur);
			}
		}

		//seek forward offfset padding from current position
		if (!is32b) 
		{
			file.seekg(padding, std::ios::cur);
		}
	}
}

Surface::Surface(int width, int height)
	:
	width(width),
	height(height),
	pPixels(new Color[width*height])

{
}

Surface::Surface(const Surface & rhs)
	:
	Surface (rhs.width,rhs.height)
{
	const int nPixels = width * height;
	for (int i = 0; i < nPixels; i++)
	{
		pPixels[i] = rhs.pPixels[i];
	}
}

Surface::~Surface()
{
	delete[] pPixels;
	pPixels = nullptr;


}

Surface & Surface::operator=(const Surface & rhs)
{
	width = rhs.width;
	height = rhs.height;
	delete [] pPixels;
	pPixels = new Color[width * height];
	
	const int nPixels = width * height;
	for (int i = 0; i < nPixels; i++)
	{
		pPixels[i] = rhs.pPixels[i];
	}

	return *this;
}

void Surface::PutPixel(int x, int y, Color c)
{
	assert(x >= 0);
	assert(x < width);
	assert(y >= 0);
	assert(y < height);
	pPixels[y * width + x] = c;
}

Color Surface::GetPixel(int x, int y) const
{
	assert(x >= 0);
	assert(x < width);
	assert(y >= 0);
	assert(y < height);
	return pPixels[y * width + x];
}

int Surface::GetWidth() const
{
	return width;
}

int Surface::GetHeight() const
{
	return height;
}

RectI Surface::GetRect() const
{
	return{0,width,0,height};
}





