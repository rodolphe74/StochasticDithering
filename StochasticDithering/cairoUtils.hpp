#ifndef CAIROUTILS_HPP
#define CAIROUTILS_HPP

#include <cairo.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <cinttypes>
#include <vector>
#include "PDFUtils.hpp"
#include "structs.hpp"
#include "voronoiUtils.hpp"

using namespace st;

class CairoUtils {
private:
	int stride = 0;
	cairo_surface_t *cairoSurface;
	cairo_t *cr;
public:
	CairoUtils()
	{
		cairoSurface = NULL;
		cr = NULL;
	}

	CairoUtils(cairo_t *cr, cairo_surface_t *cs) : cairoSurface(cs), cr(cr) {
		stride = cairo_image_surface_get_stride(cairoSurface);
	}

	static void ditherOverCairoSurface(const std::string &imageFilename, int drawWidth, int drawHeight, int margin, int points, int dist);
	static void ditherOverCairoSurfaceRGB(const std::string &imageFilename, int drawWidth, int drawHeight, int margin, int points, int dist);
	void thingsToDoBeforeDrawingPixels();
	void drawPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint32_t x, uint32_t y);
	void drawBresenham(uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
	void drawPixel(Pixel p, Color c);
	void drawPixelWithShift(Pixel p, Color c, int32_t shiftx, int32_t shifty);
	void drawBresenham(Pixel p1, Pixel p2, Color c);
	void drawTriangle(st::Pixel a, st::Pixel b, st::Pixel c, st::Colorf color);
	void drawPolygon(std::vector<st::Pixel> pixels, st::Colorf color);
	void rasterPoints(const std::vector<Pixel> points, Color c);
	void drawGraph(const graph &);
	void saveSurfaceAsPNG(const std::string &filename);
	void drawImageOnSurface(const Image *image, Pixel p);

	~CairoUtils();

private:
};

#endif