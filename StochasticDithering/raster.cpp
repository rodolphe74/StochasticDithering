#include "raster.hpp"
#include <cmath>
#include <cstdint>
#include <vector>
#include <mathc.h>


// Constructors
Raster::Raster(uint32_t w, uint32_t h)
{
	screen = new uint8_t[w * h]();
	screenHeight = h;
	screenWidth = w;
	contourX = new int32_t[screenHeight * 2]();
}

uint32_t Raster::getIndex(uint32_t x, uint32_t y) { return (x + y * screenWidth); }
uint32_t Raster::getContourIndex(uint32_t x, uint32_t y) { return (x * screenHeight + y); }

void Raster::setPixel(uint32_t x, uint32_t y, uint8_t color)
{
	if ((x < 0) || (x >= screenWidth) || (y < 0) || (y >= screenHeight)) {
		return;
	}

	//   Screen[y][x] = color;
	screen[getIndex(x, y)] = color;
}

// Scans a side of a triangle setting min X and max X in ContourX[][]
// (using the Bresenham's line drawing algorithm).
void Raster::scanLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
	int32_t sx, sy, dx1, dy1, dx2, dy2, x, y, m, n, k, cnt;

	sx = x2 - x1;
	sy = y2 - y1;

	if (sx > 0)
		dx1 = 1;
	else if (sx < 0)
		dx1 = -1;
	else
		dx1 = 0;

	if (sy > 0)
		dy1 = 1;
	else if (sy < 0)
		dy1 = -1;
	else
		dy1 = 0;

	m = abs(sx);
	n = abs(sy);
	dx2 = dx1;
	dy2 = 0;

	if (m < n) {
		m = abs(sy);
		n = abs(sx);
		dx2 = 0;
		dy2 = dy1;
	}

	x = x1;
	y = y1;
	cnt = m + 1;
	k = n / 2;

	while (cnt--) {
		if ((y >= 0) && (y < (int32_t) screenHeight)) {
			//   if (x < ContourX[y][0]) ContourX[y][0] = x;
			//   if (x > ContourX[y][1]) ContourX[y][1] = x;
			if (x < contourX[getContourIndex(0, y)])
				contourX[getContourIndex(0, y)] = x;
			if (x > contourX[getContourIndex(1, y)])
				contourX[getContourIndex(1, y)] = x;
		}

		k += n;
		if (k < m) {
			x += dx2;
			y += dy2;
		} else {
			k -= m;
			x += dx1;
			y += dy1;
		}
	}
}

void Raster::getTrianglePoints(Pixel p0, Pixel p1, Pixel p2, std::vector<Pixel> &vec)
{
	int y;

	for (y = 0; y < (int32_t) screenHeight; y++) {
		// ContourX[y][0] = INT32_MAX; // min X
		// ContourX[y][1] = INT32_MIN; // max X
		contourX[getContourIndex(0, y)] = INT32_MAX; // min X
		contourX[getContourIndex(1, y)] = INT32_MIN; // max X
	}

	scanLine(p0.x, p0.y, p1.x, p1.y);
	scanLine(p1.x, p1.y, p2.x, p2.y);
	scanLine(p2.x, p2.y, p0.x, p0.y);

	for (y = 0; y < (int32_t) screenHeight; y++) {
		// if (ContourX[y][1] >= ContourX[y][0])
		if (contourX[getContourIndex(1, y)] >= contourX[getContourIndex(0, y)]) {
			// long x = ContourX[y][0];
			int32_t x = contourX[getContourIndex(0, y)];
			// long len = 1 + ContourX[y][1] - ContourX[y][0];
			int32_t len = 1 + contourX[getContourIndex(1, y)] - contourX[getContourIndex(0, y)];

			// Can draw a horizontal line instead of individual pixels here
			while (len--) {
				// vec.push_back(Pixel { .x = (uint32_t)x++, .y = (uint32_t)y });
                vec.push_back(Pixel { /*.x =*/ (int32_t)x++, /*.y =*/ (int32_t)y });
			}
		}
	}
}


void Raster::rotatePixel(const float &angle, const Pixel &center, const Pixel &p, Pixel &rp)
{
	mfloat_t p1[VEC3_SIZE] = { (mfloat_t) p.x, (mfloat_t) p.y, 1 };
	mfloat_t mt1[MAT3_SIZE] = { 1, 0, 0, 0, 1, 0, (mfloat_t) center.x, (mfloat_t) center.y, 1 };
	mfloat_t mt2[MAT3_SIZE] = { 1, 0, 0, 0, 1, 0, (mfloat_t)  -center.x, (mfloat_t) -center.y, 1 };
	mfloat_t rt[MAT3_SIZE] = { cos(angle), sin(angle), 0, -sin(angle), cos(angle), 0, 0, 0, 1 };
	vec3_multiply_mat3(p1, p1, mt2);
	vec3_multiply_mat3(p1, p1, rt);
	vec3_multiply_mat3(p1, p1, mt1);
	rp.x = (int32_t) roundf(p1[0]);
	rp.y = (int32_t) roundf(p1[1]);
}

// Destructor
Raster::~Raster()
{
	delete[] screen;
	delete[] contourX;
}

// Getters / Setters
