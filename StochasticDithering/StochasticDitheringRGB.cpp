#pragma warning( disable : 6011)
#pragma warning( disable : 4996)


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <argparse.h>
#include "cairoUtils.hpp"


// A4 600 dpi
#define DRAW_WIDTH 4956
#define DRAW_HEIGHT 7014

// bypass strange border triangles
#define MARGIN 20



static const char *const usages[] = {
	"sd [options] [[--] args]",
	"sd [options]",
	NULL,
};



int main(int argc, const char **argv)
{
	const char *path = NULL;
	int color = 0;
	int bw = 0;
	int pdf = 0;
	int png = 0;
	int points = 50000;
	const char *distribution = NULL;
	int dist = st::POISSON;

	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_GROUP("Stochastic dithering options"),
		OPT_STRING('f', "filename", &path, "path of the image to dither", NULL, 0, 0),
		OPT_BOOLEAN('c', "color", &color, "output in cmyk color dithering", NULL, 0, 0),
		OPT_BOOLEAN('b', "bw", &bw, "output in black and white dithering", NULL, 0, 0),
		OPT_BOOLEAN('d', "pdf", &pdf, "output format in pdf", NULL, 0, 0),
		OPT_BOOLEAN('g', "png", &png, "output format in png", NULL, 0, 0),
		OPT_INTEGER('p', "points", &points, "how many distribution points", NULL, 0, 0),
		OPT_STRING('t', "distribution", &distribution, "which points distribution", NULL, 0, 0),
		OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usages, 0);
	argparse_describe(&argparse, "\n-=- Stochastic dithering -=-", "-=-");
	argc = argparse_parse(&argparse, argc, argv);

	if (path)
		std::cout << "Processing " << path << " ..." << std::endl;
	else {
		std::cout << "Empty path, can't go further." << std::endl << std::endl;

		argparse_usage(&argparse);

		return 0;
	}

	if (!bw && !color) {
		bw = 1;
		color = 0;
	}

	if (bw && color) {
		std::cout << "Incompatible values. Defaulting to black and white." << std::endl;
		bw = 1;
		color = 0;
	}

	if (!pdf && !png) {
		png = 1;
		pdf = 0;
	}

	if (pdf && png) {
		std::cout << "Incompatible values. Defaulting to png." << std::endl;
		png = 1;
		pdf = 0;
	}

	if (distribution) {
		std::string diststr = std::string(distribution);
		if (diststr == "p") {
			dist = st::POISSON;
		}
		else if (diststr == "v") {
			dist = st::VOGEL;
		}
		else if (diststr == "u") {
			dist = st::UNIFORM;
		}
		else if (diststr == "j") {
			dist = st::JITTERED;
		}
	}

	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << path << " not found." << std::endl;
		return 0;
	}

	if (bw && pdf)
		PDFUtils::ditherOverPdf(
			path,
			DRAW_WIDTH,
			DRAW_HEIGHT,
			MARGIN,
			points,
			dist
		);

	else if (bw && png)
		CairoUtils::ditherOverCairoSurface(
			path,
			DRAW_WIDTH,
			DRAW_HEIGHT,
			MARGIN,
			points,
			dist
		);

	else if (color && pdf)
		PDFUtils::ditherOverPdfRGB(
			path,
			DRAW_WIDTH,
			DRAW_HEIGHT,
			MARGIN,
			points,
			dist
		);

	else if (color && png)
		CairoUtils::ditherOverCairoSurfaceRGB(
			path,
			DRAW_WIDTH,
			DRAW_HEIGHT,
			MARGIN,
			points,
			dist
		);

	return 0;
}
