#include "cairoUtils.hpp"
#include "PDFUtils.hpp"
#include "ImageUtils.hpp"
#include "cairo.h"
#include "vertex.hpp"
#include "voronoiUtils.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iterator>
#include <map>
#include <ostream>
#include <string>
#include <stdint.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


void CairoUtils::thingsToDoBeforeDrawingPixels()
{
	cairo_surface_mark_dirty(cairoSurface);
	cairo_surface_flush(cairoSurface);
}



void CairoUtils::drawPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint32_t x, uint32_t y)
{
	int stride = cairo_image_surface_get_stride(cairoSurface);
	unsigned char *data = cairo_image_surface_get_data(cairoSurface);

	data[y * stride + x * 4] = b;
	data[y * stride + x * 4 + 1] = g;
	data[y * stride + x * 4 + 2] = r;
	data[y * stride + x * 4 + 3] = a;
}

void CairoUtils::drawPixel(Pixel p, Color c) { drawPixel(c.r, c.g, c.b, c.a, p.x, p.y); }

void CairoUtils::drawPixelWithShift(Pixel p, Color c, int32_t shiftx, int32_t shifty)
{
	drawPixel(c.r, c.g, c.b, c.a, p.x + shiftx, p.y + shifty);
}

void CairoUtils::drawBresenham(
	uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
	int x, y;
	int dx, dy;
	int incx, incy;
	int balance;

	if (x2 >= x1) {
		dx = x2 - x1;
		incx = 1;
	}
	else {
		dx = x1 - x2;
		incx = -1;
	}

	if (y2 >= y1) {
		dy = y2 - y1;
		incy = 1;
	}
	else {
		dy = y1 - y2;
		incy = -1;
	}

	x = x1;
	y = y1;

	if (dx >= dy) {
		dy <<= 1;
		balance = dy - dx;
		dx <<= 1;

		while (x != x2) {
			drawPixel(r, g, b, a, x, y);
			if (balance >= 0) {
				y += incy;
				balance -= dx;
			}
			balance += dy;
			x += incx;
		}
		drawPixel(r, g, b, a, x, y);
	}
	else {
		dx <<= 1;
		balance = dx - dy;
		dy <<= 1;

		while (y != y2) {
			drawPixel(r, g, b, a, x, y);
			if (balance >= 0) {
				x += incx;
				balance -= dy;
			}
			balance += dx;
			y += incy;
		}
		drawPixel(r, g, b, a, x, y);
	}
}

void CairoUtils::drawBresenham(Pixel p1, Pixel p2, Color c)
{
	drawBresenham(c.r, c.g, c.b, c.a, p1.x, p1.y, p2.x, p2.y);
}

void CairoUtils::drawTriangle(st::Pixel a, st::Pixel b, st::Pixel c, st::Colorf color)
{
	cairo_set_line_width(cr, 1.0);
	cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);
	cairo_move_to(cr, a.x, a.y);
	cairo_line_to(cr, b.x, b.y);
	cairo_line_to(cr, c.x, c.y);
	cairo_close_path(cr);
	cairo_fill_preserve(cr);
	cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);
	cairo_stroke(cr);
}

void CairoUtils::drawPolygon(std::vector<st::Pixel> pixels, st::Colorf color)
{
	cairo_set_line_width(cr, 1.0);
	cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);
	cairo_move_to(cr, pixels.at(0).x, pixels.at(0).y);
	for (auto p = pixels.begin(); p != pixels.end(); ++p) {
		cairo_line_to(cr, p->x, p->y);
	}
	cairo_close_path(cr);
	cairo_fill_preserve(cr);
	cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);
	cairo_stroke(cr);
}


void CairoUtils::rasterPoints(const std::vector<Pixel> points, Color c)
{
	for (auto p = begin(points); p != end(points); ++p) {
		drawPixel(*p, c);
	}
}

void CairoUtils::drawGraph(const graph &g)
{
	cairo_set_source_rgb(cr, 1, 0, 0);
	cairo_set_line_width(cr, .5);
	for (graph::const_iterator i = g.begin(); i != g.end(); ++i) {
		Vertex v = i->first;

		cairo_move_to(cr, v.getX(), v.getY());
		cairo_arc(cr, v.getX(), v.getY(), 2.0, 0, 2 * M_PI);

		std::set<Vertex> neighbors = i->second;
		for (std::set<Vertex>::const_iterator j = neighbors.begin(); j != neighbors.end(); ++j) {
			cairo_move_to(cr, v.getX(), v.getY());
			cairo_line_to(cr, j->getX(), j->getY());
		}
		cairo_stroke(cr);
	}
	cairo_fill(cr);
}


void CairoUtils::saveSurfaceAsPNG(const std::string &filename)
{
	// TODO permut r & b
	int w = cairo_image_surface_get_width(cairoSurface);
	int h = cairo_image_surface_get_height(cairoSurface);
	unsigned char *data = cairo_image_surface_get_data(cairoSurface);

	std::cout << std::endl << "Writing " << filename.c_str() << std::endl;

	Image image;
	image.w = w;
	image.h = h;
	image.data = data;
	image.comp = 4;
	ImageUtils::saveImageAsPNG(filename, &image);

}


void CairoUtils::drawImageOnSurface(const Image *image, Pixel p)
{
	int indexImage;
	int indexSurface;
	int stride = cairo_image_surface_get_stride(cairoSurface);
	int sw = cairo_image_surface_get_width(cairoSurface);
	int sh = cairo_image_surface_get_height(cairoSurface);
	unsigned char *data = cairo_image_surface_get_data(cairoSurface);

	// comp = 4
	for (uint32_t y = 0; y < image->h; y++) {
		for (uint32_t x = 0; x < image->w; x++) {
			indexImage = y * image->w * 4 + x * 4;
			indexSurface = (p.y + y) * stride + (p.x + x) * 4;
			if ((p.y + y) < (uint32_t) sh && (p.x + x) < (uint32_t) sw) {
				data[indexSurface + 2] = image->data[indexImage];
				data[indexSurface + 1] = image->data[indexImage + 1];
				data[indexSurface] = image->data[indexImage + 2];
				data[indexSurface + 3] = image->data[indexImage + 3];
			}
		}
	}
}



void CairoUtils::ditherOverCairoSurface(const std::string &imageFilename, int drawWidth, int drawHeight, int margin, int points, int dist)
{
	cairo_surface_t *cairoSurface;
	Raster *raster;

	

	Image im, gr, rszgr;
	ImageUtils::loadImageAsRGBA(imageFilename.c_str(), &im);
	std::cout << "image size " << im.w << "x" << im.h << std::endl;
	ImageUtils::convertImageToGray(&im, &gr, 1.6, 255);

	ImageUtils::resizeImage(&gr, &rszgr, drawWidth - 2 * margin, drawHeight - 2 * margin);
	//cu->drawImageOnSurface(&rszgr, st::Pixel{ MARGIN, MARGIN });

	//cairoSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, drawWidth, drawHeight);
	cairoSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, rszgr.w + 1 * margin, rszgr.h + 1 * margin);

	// Draw whatever you want on this back buffered surface
	cairo_t *cr = cairo_create(cairoSurface);
	// cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);

	// clear background
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
	cairo_paint(cr);

	// raster = new Raster(drawWidth, drawHeight);
	raster = new Raster(rszgr.w + 1 * margin, rszgr.h + 1 * margin);

	CairoUtils *cu = new CairoUtils(cr, cairoSurface);
	cu->thingsToDoBeforeDrawingPixels();

	VoronoiUtils voronoiUtils;
	std::vector<st::Pixel> seeds;

	// Choose distribution, best results with Poisson sampling
	if (dist == st::POISSON)
		voronoiUtils.createPoissonSampling(drawWidth, drawHeight, seeds, points);
	else if (dist == st::JITTERED)
		voronoiUtils.createJitteredSampling(drawWidth, drawHeight, seeds, points);
	else if (dist == st::VOGEL)
		voronoiUtils.createVogelSampling(drawWidth, drawHeight, seeds, points);
	else if (dist == st::UNIFORM)
		voronoiUtils.createUniformSampling(drawWidth, drawHeight, seeds);

	std::cout << "delaunay seeds:" << seeds.size() << std::endl;
	std::vector<st::TrianglePixels> trianglesPixels;
	voronoiUtils.delaunay(seeds, trianglesPixels);

	// Graph
	graph trianglesMap;
	voronoiUtils.graphit(trianglesPixels, trianglesMap);
	std::cout << "graph nodes:" << trianglesMap.size() << std::endl;
	// cu->drawGraph(trianglesMap);

	int count = 0;
	size_t lastStringSize = 0;
	size_t size = trianglesMap.size();
	for (auto t = trianglesMap.begin(); t != trianglesMap.end(); ++t) {
		Vertex v = t->first;
		if (count % 100/*0*/ == 0) {
			std::stringstream verticesString;
			verticesString << "processed vertices:" << count << "/" << size;
			lastStringSize = verticesString.str().size();
			for (int i = 0; i < lastStringSize; i++)	std::cout << "\b";
			std::cout << verticesString.str();
		}

		VoronoiUtils::saveDot(trianglesMap, v, *raster, rszgr, voronoiUtils, voronoiUtils.polygonsK, rszgr.w, rszgr.h, margin, { 0.0f, 0.0f, 0.0f, 1.0f }, 0.0f);	// K
		count++;
	}

	
	std::cout << std::endl << "Sorting Black Polygons" << std::endl;
	for (auto p = voronoiUtils.polygonsK.begin(); p != voronoiUtils.polygonsK.end(); ++p) {
		std::vector<Pixel> v = *(p->second);
		std::vector<Pixel> sv;
		std::vector<Pixel> hull;
		VoronoiUtils::sortPixelsByAngle(v, sv);
		VoronoiUtils::hull(sv, hull);
		cu->drawPolygon(hull, { 0.0f, 0.0f, 0.0f, 1.0f });
	}


	std::cout << "Saving result" << std::endl;
	std::string base_filename = imageFilename.substr(imageFilename.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	std::string file_without_extension = base_filename.substr(0, p);
	std::string newFilename = "bw_" + file_without_extension + ".png";
	cairo_surface_write_to_png(cu->cairoSurface, newFilename.c_str());

	ImageUtils::freeImage(&im);
	ImageUtils::freeImage(&gr);
	ImageUtils::freeImage(&rszgr);

	delete(cu);

	cairo_restore(cr);
	cairo_destroy(cr);

	cairo_surface_destroy(cairoSurface);
	delete(raster);
}



void CairoUtils::ditherOverCairoSurfaceRGB(const std::string &imageFilename, int drawWidth, int drawHeight, int margin, int points, int dist)
{
	const float ANGLES[] = { 0.262f, 1.309f, 0, 0.785f };

	Image im;
	Image cim, yim, mim, kim;
	Image cszr, mszr, yszr, kszr;
	Image rcszr, rmszr, ryszr, rkszr;

	ImageUtils::loadImageAsRGBA(imageFilename.c_str(), &im);
	std::cout << "image size " << im.w << "x" << im.h << std::endl;
	ImageUtils::getCMYKComponents(&im, &cim, &mim, &yim, &kim);

	ImageUtils::resizeImage(&cim, &cszr, drawWidth - 2 * margin, drawHeight - 2 * margin);
	ImageUtils::resizeImage(&mim, &mszr, drawWidth - 2 * margin, drawHeight - 2 * margin);
	ImageUtils::resizeImage(&yim, &yszr, drawWidth - 2 * margin, drawHeight - 2 * margin);
	ImageUtils::resizeImage(&kim, &kszr, drawWidth - 2 * margin, drawHeight - 2 * margin);

	ImageUtils::rotateImage(&cszr, &rcszr, ANGLES[0]);
	ImageUtils::rotateImage(&mszr, &rmszr, ANGLES[1]);
	ImageUtils::rotateImage(&yszr, &ryszr, ANGLES[2]);
	ImageUtils::rotateImage(&kszr, &rkszr, ANGLES[3]);

	// freeing memory from useless images
	ImageUtils::freeImage(&cim);
	ImageUtils::freeImage(&yim);
	ImageUtils::freeImage(&mim);
	ImageUtils::freeImage(&kim);
	ImageUtils::freeImage(&cszr);
	ImageUtils::freeImage(&mszr);
	ImageUtils::freeImage(&yszr);
	ImageUtils::freeImage(&kszr);

	int maxWidth = std::max(rkszr.w, std::max(rmszr.w, std::max(rcszr.w, ryszr.w)));
	int maxHeight = std::max(rkszr.h, std::max(rmszr.h, std::max(rcszr.h, ryszr.h)));
	std::cout << "max width:" << maxWidth << "  " << "max height:" << maxHeight << std::endl;


	cairo_surface_t *cairoSurface;
	Raster *raster;

	cairoSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, maxWidth, maxHeight);

	// Draw whatever you want on this back buffered surface
	cairo_t *cr = cairo_create(cairoSurface);

	// clear background
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
	cairo_paint(cr);

	raster = new Raster(maxWidth, maxHeight);


	CairoUtils *cu = new CairoUtils(cr, cairoSurface);
	cu->thingsToDoBeforeDrawingPixels();



	VoronoiUtils voronoiUtils;
	std::vector<st::Pixel> seeds;

	// Choose distribution, best results with Poisson sampling
	if (dist == st::POISSON)
		voronoiUtils.createPoissonSampling(maxWidth, maxHeight, seeds, points);
	else if (dist == st::JITTERED)
		voronoiUtils.createJitteredSampling(maxWidth, maxHeight, seeds, points);
	else if (dist == st::VOGEL)
		voronoiUtils.createVogelSampling(maxWidth, maxHeight, seeds, points);
	else if (dist == st::UNIFORM)
		voronoiUtils.createUniformSampling(maxWidth, maxHeight, seeds);

	std::cout << "delaunay seeds:" << seeds.size() << std::endl;
	std::vector<st::TrianglePixels> trianglesPixels;
	voronoiUtils.delaunay(seeds, trianglesPixels);

	// Create a graph from the delaunay triangulation diagram
	graph trianglesMap;
	voronoiUtils.graphit(trianglesPixels, trianglesMap);
	std::cout << "graph nodes:" << trianglesMap.size() << std::endl;
	//cu->drawGraph(trianglesMap);

	cairo_set_operator(cr, CAIRO_OPERATOR_MULTIPLY);

	int count = 0;
	size_t lastStringSize = 0;
	size_t size = trianglesMap.size();
	for (auto t = trianglesMap.begin(); t != trianglesMap.end(); ++t) {
		Vertex v = t->first;
		if (count % 100/*0*/ == 0) {
			std::stringstream verticesString;
			verticesString << "processed vertices:" << count << "/" << size;
			lastStringSize = verticesString.str().size();
			for (int i = 0; i < lastStringSize; i++)	std::cout << "\b";
			std::cout << verticesString.str();
		}

		VoronoiUtils::saveDot(trianglesMap, v, *raster, rcszr, voronoiUtils, voronoiUtils.polygonsC, maxWidth, maxHeight, margin, { 0.0f, 1.0f, 1.0f, 1.0f }, -ANGLES[0]);	// Y
		VoronoiUtils::saveDot(trianglesMap, v, *raster, rmszr, voronoiUtils, voronoiUtils.polygonsM, maxWidth, maxHeight, margin, { 1.0f, 0.0f, 1.0f, 1.0f }, -ANGLES[1]);	// C
		VoronoiUtils::saveDot(trianglesMap, v, *raster, ryszr, voronoiUtils, voronoiUtils.polygonsY, maxWidth, maxHeight, margin, { 1.0f, 1.0f, 0.0f, 1.0f }, -ANGLES[2]);	// M
		VoronoiUtils::saveDot(trianglesMap, v, *raster, rkszr, voronoiUtils, voronoiUtils.polygonsK, maxWidth, maxHeight, margin, { 0.0f, 0.0f, 0.0f, 1.0f }, -ANGLES[3]);	// K

		count++;
	}

	std::cout << std::endl;
	std::cout << "Size C:" << voronoiUtils.polygonsC.size() << std::endl;
	std::cout << "Size M:" << voronoiUtils.polygonsM.size() << std::endl;
	std::cout << "Size Y:" << voronoiUtils.polygonsY.size() << std::endl;
	std::cout << "Size K:" << voronoiUtils.polygonsK.size() << std::endl;

	std::cout << "Sorting Cyan Polygons" << std::endl;
	for (auto p = voronoiUtils.polygonsC.begin(); p != voronoiUtils.polygonsC.end(); ++p) {
		std::vector<Pixel> v = *(p->second);
		std::vector<Pixel> sv;
		std::vector<Pixel> hull;
		VoronoiUtils::sortPixelsByAngle(v, sv);
		VoronoiUtils::hull(sv, hull);
		cu->drawPolygon(hull, { 0.0f, 1.0f, 1.0f, 1.0f });
	}

	std::cout << "Sorting Magenta Polygons" << std::endl;
	for (auto p = voronoiUtils.polygonsM.begin(); p != voronoiUtils.polygonsM.end(); ++p) {
		std::vector<Pixel> v = *(p->second);
		std::vector<Pixel> sv;
		std::vector<Pixel> hull;
		VoronoiUtils::sortPixelsByAngle(v, sv);
		VoronoiUtils::hull(sv, hull);
		cu->drawPolygon(hull, { 1.0f, 0.0f, 1.0f, 1.0f });
	}

	std::cout << "Sorting Yellow Polygons" << std::endl;
	for (auto p = voronoiUtils.polygonsY.begin(); p != voronoiUtils.polygonsY.end(); ++p) {
		std::vector<Pixel> v = *(p->second);
		std::vector<Pixel> sv;
		std::vector<Pixel> hull;
		VoronoiUtils::sortPixelsByAngle(v, sv);
		VoronoiUtils::hull(sv, hull);
		cu->drawPolygon(hull, { 1.0f, 1.0f, 0.0f, 1.0f });
	}

	std::cout << "Sorting Black Polygons" << std::endl;
	for (auto p = voronoiUtils.polygonsK.begin(); p != voronoiUtils.polygonsK.end(); ++p) {
		std::vector<Pixel> v = *(p->second);
		std::vector<Pixel> sv;
		std::vector<Pixel> hull;
		VoronoiUtils::sortPixelsByAngle(v, sv);
		VoronoiUtils::hull(sv, hull);
		cu->drawPolygon(hull, { 0.0f, 0.0f, 0.0f, 1.0f });
	}


	std::cout << "Saving result" << std::endl;
	std::string base_filename = imageFilename.substr(imageFilename.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	std::string file_without_extension = base_filename.substr(0, p);
	std::string newFilename = "cmyk_" + file_without_extension + ".png";
	cairo_surface_write_to_png(cu->cairoSurface, newFilename.c_str());

	ImageUtils::freeImage(&rcszr);
	ImageUtils::freeImage(&rmszr);
	ImageUtils::freeImage(&ryszr);
	ImageUtils::freeImage(&rkszr);

	delete(cu);
	cairo_restore(cr);
	cairo_destroy(cr);
	cairo_surface_destroy(cairoSurface);
	delete(raster);
}



// Destructor
CairoUtils::~CairoUtils() { }
