#include "PDFUtils.hpp"
#include "ImageUtils.hpp"
#include "voronoiUtils.hpp"
#include "raster.hpp"
#include <cstdint>
#include <sstream>
#include <algorithm>



// Constructors

void PDFUtils::drawTriangle(st::Pixel a, st::Pixel b, st::Pixel c, st::Colorf color)
{
	HPDF_Page_SetRGBStroke(page, color.r, color.g, color.b);
	HPDF_Page_SetRGBFill(page, color.r, color.g, color.b);
	HPDF_Page_MoveTo(page, (HPDF_REAL) a.x, (HPDF_REAL) height - a.y);
	HPDF_Page_LineTo(page, (HPDF_REAL) b.x, (HPDF_REAL) height - b.y);
	HPDF_Page_LineTo(page, (HPDF_REAL) c.x, (HPDF_REAL) height - c.y);
	HPDF_Page_FillStroke(page);
}


void PDFUtils::drawTriangle(st::Pixel a, st::Pixel b, st::Pixel c, st::Colorf color, const Rectangle &r)
{
	if (a.x >= r.x1 && a.x <= r.x2 &&
		b.x >= r.x1 && b.x <= r.x2 &&
		c.x >= r.x1 && c.x <= r.x2 &&
		a.y >= r.y1 && a.y <= r.y2 &&
		b.y >= r.y1 && b.y <= r.y2 &&
		c.y >= r.y1 && c.y <= r.y2) {

		//std::cout << "(" << a.x << "," << a.y << ")" << std::endl;
		//std::cout << "(" << b.x << "," << b.y << ")" << std::endl;
		//std::cout << "(" << c.x << "," << c.y << ")" << std::endl << std::endl;

		HPDF_Page_SetRGBStroke(page, color.r, color.g, color.b);
		HPDF_Page_SetRGBFill(page, color.r, color.g, color.b);
		HPDF_Page_MoveTo(page, (HPDF_REAL) a.x, (HPDF_REAL) height - a.y);
		HPDF_Page_LineTo(page, (HPDF_REAL) b.x, (HPDF_REAL) height - b.y);
		HPDF_Page_LineTo(page, (HPDF_REAL) c.x, (HPDF_REAL) height - c.y);
		HPDF_Page_FillStroke(page);
	}
}



void PDFUtils::drawPolygon(std::vector<st::Pixel> pixels, st::Colorf color)
{

		HPDF_Page_SetRGBStroke(page, color.r, color.g, color.b);
		HPDF_Page_SetRGBFill(page, color.r, color.g, color.b);

		HPDF_Page_MoveTo(page, (HPDF_REAL) pixels.at(0).x, (HPDF_REAL) height - pixels.at(0).y);
		

		for (auto p = pixels.begin(); p != pixels.end(); ++p) {
			HPDF_Page_LineTo(page, (HPDF_REAL) p->x, (HPDF_REAL) height - p->y);
		}

		HPDF_Page_FillStroke(page);

}




void PDFUtils::ditherOverPdf(const std::string &imageFilename, int drawWidth, int drawHeight, int margin, int points, int dist)
{
	Raster *raster;

	Image im, gr, rszgr;
	ImageUtils::loadImageAsRGBA(imageFilename.c_str(), &im);
	std::cout << "image size " << im.w << "x" << im.h << std::endl;
	ImageUtils::convertImageToGray(&im, &gr, 1.6, 255);

	ImageUtils::resizeImage(&gr, &rszgr, drawWidth - 2 * margin, drawHeight - 2 * margin);

	// backed pdf
	PDFUtils *pu = new PDFUtils(rszgr.w + 1 * margin, rszgr.h + 1 * margin);
	raster = new Raster(rszgr.w + 1 * margin, rszgr.h + 1 * margin);

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
		pu->drawPolygon(hull, { 0.0f, 0.0f, 0.0f, 1.0f });
	}

	ImageUtils::freeImage(&im);
	ImageUtils::freeImage(&gr);
	ImageUtils::freeImage(&rszgr);

	std::string base_filename = imageFilename.substr(imageFilename.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	std::string file_without_extension = base_filename.substr(0, p);

	pu->saveIt("bw_" + file_without_extension + ".pdf");
	delete(pu);
	delete(raster);
}


void PDFUtils::ditherOverPdfRGB(const std::string &imageFilename, int drawWidth, int drawHeight, int margin, int points, int dist)
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

	Raster *raster;
	raster = new Raster(maxWidth, maxHeight);

	// Result in PDF
	PDFUtils *pu = new PDFUtils(maxWidth, maxHeight);


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

	// Graph
	graph trianglesMap;
	voronoiUtils.graphit(trianglesPixels, trianglesMap);
	std::cout << "graph nodes:" << trianglesMap.size() << std::endl;

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
		pu->drawPolygon(hull, { 0.0f, 1.0f, 1.0f, 1.0f });
	}

	std::cout << "Sorting Magenta Polygons" << std::endl;
	for (auto p = voronoiUtils.polygonsM.begin(); p != voronoiUtils.polygonsM.end(); ++p) {
		std::vector<Pixel> v = *(p->second);
		std::vector<Pixel> sv;
		std::vector<Pixel> hull;
		VoronoiUtils::sortPixelsByAngle(v, sv);
		VoronoiUtils::hull(sv, hull);
		pu->drawPolygon(hull, { 1.0f, 0.0f, 1.0f, 1.0f });
	}

	std::cout << "Sorting Yellow Polygons" << std::endl;
	for (auto p = voronoiUtils.polygonsY.begin(); p != voronoiUtils.polygonsY.end(); ++p) {
		std::vector<Pixel> v = *(p->second);
		std::vector<Pixel> sv;
		std::vector<Pixel> hull;
		VoronoiUtils::sortPixelsByAngle(v, sv);
		VoronoiUtils::hull(sv, hull);
		pu->drawPolygon(hull, { 1.0f, 1.0f, 0.0f, 1.0f });
	}

	std::cout << "Sorting Black Polygons" << std::endl;
	for (auto p = voronoiUtils.polygonsK.begin(); p != voronoiUtils.polygonsK.end(); ++p) {
		std::vector<Pixel> v = *(p->second);
		std::vector<Pixel> sv;
		std::vector<Pixel> hull;
		VoronoiUtils::sortPixelsByAngle(v, sv);
		VoronoiUtils::hull(sv, hull);
		pu->drawPolygon(hull, { 0.0f, 0.0f, 0.0f, 1.0f });
	}


	ImageUtils::freeImage(&rcszr);
	ImageUtils::freeImage(&rmszr);
	ImageUtils::freeImage(&ryszr);
	ImageUtils::freeImage(&rkszr);

	std::string base_filename = imageFilename.substr(imageFilename.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	std::string file_without_extension = base_filename.substr(0, p);

	pu->saveIt("cmyk_" + file_without_extension + ".pdf");

	delete(pu);
	delete(raster);
}

// Operators
PDFUtils &PDFUtils::operator=(const PDFUtils &assign)
{
	(void) assign;
	return *this;
}
