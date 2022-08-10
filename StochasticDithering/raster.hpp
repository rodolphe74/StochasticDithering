#ifndef RASTER_HPP
#define RASTER_HPP

#ifdef _DEBUG
#define _ITERATOR_DEBUG_LEVEL 2
#endif // DEBUG



#include "structs.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using namespace st;

class Raster {
public:
	// Constructors
	Raster(uint32_t w, uint32_t h);

	void setPixel(uint32_t x, uint32_t y, uint8_t color);
	void scanLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    void getTrianglePoints(Pixel p0, Pixel p1, Pixel p2, std::vector<Pixel> &vec);
	static void rotatePixel(const float &angle, const Pixel &center, const Pixel &p, Pixel &rp);

	// Destructor
	~Raster();

private:
	uint8_t *screen;
	uint32_t screenHeight, screenWidth;
	int32_t *contourX;

	uint32_t getIndex(uint32_t x, uint32_t y);
    uint32_t getContourIndex(uint32_t x, uint32_t y);
};

#endif