#pragma once

#include <string>
#include "structs.hpp"

using namespace st;

class ImageUtils
{
public:
	static void loadImageAsRGBA(const std::string &filename, st::Image *image);
	static void saveImageAsPNG(const std::string &filename, const st::Image *image);
	static void freeImage(st::Image *image);
	static void convertImageToGray(const st::Image *source, st::Image *target, double gamma, int alpha);
	static void getRGBComponents(const st::Image *source, st::Image *r, st::Image *g, st::Image *b);
	static void getCMYKComponents(const st::Image *source, st::Image *c, st::Image *m, st::Image *y, st::Image *k);
	static void resizeImage(const Image *source, Image *target, int32_t newSizeX, int32_t newSizeY);
	static void rotateImage(const Image *source, Image *target, float angle);
	static void _rotateImage(const Image *source, Image *target, float angle);
	static bool checkCoordinatesAfterMargin(const Pixel &p, int32_t margin, const Image &im);
	static void getImageCoordinatesAfterMargin(const Pixel &p, Pixel &q, int32_t margin);
	static void copy(const Image *source, Image *target, int32_t x, int32_t y);
};

