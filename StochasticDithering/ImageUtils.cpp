#include "ImageUtils.hpp"
#include "raster.hpp"
#include <iostream>


#pragma warning(disable : 26451)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include "stb_image_resize.h"

void ImageUtils::loadImageAsRGBA(const std::string &filename, Image *image)
{
	int x, y, c;
	unsigned char *data = stbi_load(filename.c_str(), &x, &y, &c, 4);
	image->data = data;
	image->w = x;
	image->h = y;
	image->comp = 4;
}

void ImageUtils::saveImageAsPNG(const std::string &filename, const Image *image)
{
	stbi_write_png(
		filename.c_str(), image->w, image->h, image->comp, image->data, image->w * image->comp);
}

void ImageUtils::freeImage(Image *image) { stbi_image_free(image->data); }


void ImageUtils::convertImageToGray(const Image *source, Image *target, double gamma, int alpha)
{
	int r, g, b, a;
	int indexImage;
	int grayLevel;
	double lum;
	double gr, gg, gb;
	target->data = (uint8_t *) calloc(source->w * source->h * source->comp, sizeof(uint8_t));
	target->h = source->h;
	target->w = source->w;
	target->comp = source->comp;
	for (uint32_t y = 0; y < source->h; y++) {
		for (uint32_t x = 0; x < source->w; x++) {
			indexImage = y * source->w * 4 + x * 4;
			r = source->data[indexImage];
			g = source->data[indexImage + 1];
			b = source->data[indexImage + 2];
			a = source->data[indexImage + 3];

			// Normalize and gamma correct:
			gr = pow(r / 255.0, gamma);
			gg = pow(g / 255.0, gamma);
			gb = pow(b / 255.0, gamma);

			// Calculate luminance:
			lum = 0.2126 * gr + 0.7152 * gg + 0.0722 * gb;
			grayLevel = (uint8_t) ceil(255.0 * pow(lum, 1.0 / gamma));

			// std::cout << "graylevel:" << "  " << "alpha:" << alpha;

			if (target->data) {
				target->data[indexImage] = grayLevel;
				target->data[indexImage + 1] = grayLevel;
				target->data[indexImage + 2] = grayLevel;
				target->data[indexImage + 3] = alpha;
			}
		}
	}
}


void ImageUtils::getRGBComponents(const Image *source, Image *rTarget, Image *gTarget, Image *bTarget)
{
	int r, g, b, a;
	int indexImage;

	rTarget->data = (uint8_t *) calloc(source->w * source->h * source->comp, sizeof(uint8_t));
	rTarget->h = source->h;
	rTarget->w = source->w;
	rTarget->comp = source->comp;

	gTarget->data = (uint8_t *) calloc(source->w * source->h * source->comp, sizeof(uint8_t));
	gTarget->h = source->h;
	gTarget->w = source->w;
	gTarget->comp = source->comp;

	bTarget->data = (uint8_t *) calloc(source->w * source->h * source->comp, sizeof(uint8_t));
	bTarget->h = source->h;
	bTarget->w = source->w;
	bTarget->comp = source->comp;

	for (uint32_t y = 0; y < source->h; y++) {
		for (uint32_t x = 0; x < source->w; x++) {
			indexImage = y * source->w * 4 + x * 4;
			r = source->data[indexImage];
			g = source->data[indexImage + 1];
			b = source->data[indexImage + 2];
			a = source->data[indexImage + 3];

			if (rTarget->data) {
				rTarget->data[indexImage] = r;
				rTarget->data[indexImage + 1] = r;
				rTarget->data[indexImage + 2] = r;
				rTarget->data[indexImage + 3] = a;
			}

			if (gTarget->data) {
				gTarget->data[indexImage] = g;
				gTarget->data[indexImage + 1] = g;
				gTarget->data[indexImage + 2] = g;
				gTarget->data[indexImage + 3] = a;
			}

			if (bTarget->data) {
				bTarget->data[indexImage] = b;
				bTarget->data[indexImage + 1] = b;
				bTarget->data[indexImage + 2] = b;
				bTarget->data[indexImage + 3] = a;
			}
		}
	}
}



void ImageUtils::getCMYKComponents(const st::Image *source, st::Image *cTarget, st::Image *mTarget, st::Image *yTarget, st::Image *kTarget)
{
	uint8_t r, g, b, a;
	float fc, fm, fy, fk, fw;
	int indexImage;

	cTarget->data = (uint8_t *) calloc(source->w * source->h * source->comp, sizeof(uint8_t));
	cTarget->h = source->h;
	cTarget->w = source->w;
	cTarget->comp = source->comp;

	mTarget->data = (uint8_t *) calloc(source->w * source->h * source->comp, sizeof(uint8_t));
	mTarget->h = source->h;
	mTarget->w = source->w;
	mTarget->comp = source->comp;

	yTarget->data = (uint8_t *) calloc(source->w * source->h * source->comp, sizeof(uint8_t));
	yTarget->h = source->h;
	yTarget->w = source->w;
	yTarget->comp = source->comp;

	kTarget->data = (uint8_t *) calloc(source->w * source->h * source->comp, sizeof(uint8_t));
	kTarget->h = source->h;
	kTarget->w = source->w;
	kTarget->comp = source->comp;

	for (uint32_t y = 0; y < source->h; y++) {
		for (uint32_t x = 0; x < source->w; x++) {
			indexImage = y * source->w * 4 + x * 4;
			r = source->data[indexImage];
			g = source->data[indexImage + 1];
			b = source->data[indexImage + 2];
			a = source->data[indexImage + 3];

			fw = std::max(r / 255.0f, std::max(g / 255.0f, b / 255.0f));
			fc = (fw - (r / 255.0f)) / fw;
			fm = (fw - (g / 255.0f)) / fw;
			fy = (fw - (b / 255.0f)) / fw;
			fk = 1 - fw;

			//std::cout << fc << "," << fm << "," << fy << "," << fk << std::endl;

			if (cTarget->data) {
				cTarget->data[indexImage] = 255 - (uint8_t) roundf(fc * 255.0f);
				cTarget->data[indexImage + 1] = 255 - (uint8_t) roundf(fc * 255.0f);
				cTarget->data[indexImage + 2] = 255 - (uint8_t) roundf(fc * 255.0f);
				cTarget->data[indexImage + 3] = a;
			}

			if (mTarget->data) {
				mTarget->data[indexImage] = 255 - (uint8_t) roundf(fm * 255.0f);
				mTarget->data[indexImage + 1] = 255 - (uint8_t) roundf(fm * 255.0f);
				mTarget->data[indexImage + 2] = 255 - (uint8_t) roundf(fm * 255.0f);
				mTarget->data[indexImage + 3] = a;
			}

			if (yTarget->data) {
				yTarget->data[indexImage] = 255 - (uint8_t) roundf(fy * 255.0f);
				yTarget->data[indexImage + 1] = 255 - (uint8_t) roundf(fy * 255.0f);
				yTarget->data[indexImage + 2] = 255 - (uint8_t) roundf(fy * 255.0f);
				yTarget->data[indexImage + 3] = a;
			}

			if (kTarget->data) {
				kTarget->data[indexImage] = 255 - (uint8_t) roundf(fk * 255.0f);
				kTarget->data[indexImage + 1] = 255 - (uint8_t) roundf(fk * 255.0f);
				kTarget->data[indexImage + 2] = 255 - (uint8_t) roundf(fk * 255.0f);
				kTarget->data[indexImage + 3] = a;
			}
		}
	}

}




void ImageUtils::resizeImage(const Image *source, Image *target, int32_t newSizeX, int32_t newSizeY)
{
	double ratiow = (double) newSizeX / (double) source->w;
	double ratioh = (double) newSizeY / (double) source->h;
	double ratio = std::min(ratiow, ratioh);

	std::cout << "resizing image" << std::endl;
	std::cout << "image ratio:" << ratio << std::endl;
	std::cout << "image new size:" << (int32_t) round(source->w * ratio) << "x" << (int32_t) round(source->h * ratio) << std::endl;

	double w = round(source->w * ratio);
	double h = round(source->h * ratio);
	size_t size = (size_t) (w * h);

	// target->data = (uint8_t *) calloc(w * h * source->comp, sizeof(uint8_t));
	target->data = (uint8_t *) calloc(size * source->comp, sizeof(uint8_t));

	stbir_resize_uint8(source->data, source->w, source->h, 0, target->data, (int) w, (int) h, 0, source->comp);

	target->comp = source->comp;
	target->w = (int) round(w);
	target->h = (int) round(h);
}


void ImageUtils::_rotateImage(const Image *source, Image *target, float angle)
{
	std::cout << "rotating image" << std::endl;
	std::cout << "size:" << source->w << "*" << source->h << std::endl;
	std::cout << "angle:" << angle << std::endl;

	Pixel rp;
	int32_t minx = INT32_MAX, maxx = INT32_MIN, miny = INT32_MAX, maxy = INT32_MIN;
	for (uint32_t y = 0; y < source->h; y++) {
		for (uint32_t x = 0; x < source->w; x++) {
			Raster::rotatePixel(angle, { (int32_t) source->w / 2, (int32_t) source->h / 2 }, { (int32_t) x, (int32_t) y }, rp);

			if (rp.x < minx) minx = rp.x;
			if (rp.x > maxx) maxx = rp.x;
			if (rp.y < miny) miny = rp.y;
			if (rp.y > maxy) maxy = rp.y;
		}
	}

	std::cout << "rotated image coordinates:(" << minx << "," << miny << ") - (" << maxx << "," << maxy << ")" << std::endl;
	std::cout << "image new size:" << (maxx - minx) << "*" << (maxy - miny) << std::endl;

	target->data = (uint8_t *) calloc((maxx - minx + 1) * (maxy - miny + 1) * source->comp, sizeof(uint8_t));
	target->w = (maxx - minx);
	target->h = (maxy - miny);
	target->comp = source->comp;

	for (uint32_t y = 0; y < source->h; y++) {
		for (uint32_t x = 0; x < source->w; x++) {
			Raster::rotatePixel(angle, { (int32_t) source->w / 2, (int32_t) source->h / 2 }, { (int32_t) x, (int32_t) y }, rp);
			// rp = { (int32_t) x, (int32_t) y };

			//if (((rp.x - minx) + (rp.y - miny) * target->w) * target->comp >= (maxx - minx + 1) * (maxy - miny + 1) * source->comp ) {
			//	std::cout << "center:" << (int32_t) source->w / 2 << "," << (int32_t) source->h / 2 << "  (" << x << "," << y << ")" << "->" << "(" << rp.x << "," << rp.y << ")" << std::endl;
			//	std::cout << "   (" << (rp.x - minx) << "," << (rp.y - miny) << ")" << std::endl;
			//	std::cout << ((rp.x - minx) + (rp.y - miny) * target->w) * target->comp << " vs " << (maxx - minx) * (maxy - miny) * source->comp << std::endl;
			//	continue;
			//}

			memcpy(
				target->data + (((rp.x - minx) + (rp.y - miny) * target->w)) * target->comp,
				source->data + ((x + y * source->w) * source->comp),
				source->comp);
		}
	}

}


void ImageUtils::rotateImage(const Image *source, Image *target, float angle)
{
	std::cout << "rotating image" << std::endl;
	std::cout << "size:" << source->w << "*" << source->h << std::endl;
	std::cout << "angle:" << angle << std::endl;

	Pixel rp;
	int32_t minx = INT32_MAX, maxx = INT32_MIN, miny = INT32_MAX, maxy = INT32_MIN;
	for (uint32_t y = 0; y < source->h; y++) {
		for (uint32_t x = 0; x < source->w; x++) {
			Raster::rotatePixel(angle, { (int32_t) source->w / 2, (int32_t) source->h / 2 }, { (int32_t) x, (int32_t) y }, rp);

			if (rp.x < minx) minx = rp.x;
			if (rp.x > maxx) maxx = rp.x;
			if (rp.y < miny) miny = rp.y;
			if (rp.y > maxy) maxy = rp.y;
		}
	}

	std::cout << "rotated image coordinates:(" << minx << "," << miny << ") - (" << maxx << "," << maxy << ")" << std::endl;
	int32_t newW = 1 + maxx - minx, newH = 1 + maxy - miny;
	// std::cout << "image new size:" << (maxx - minx) << "*" << (maxy - miny) << std::endl;
	std::cout << "image new size:" << newW << "*" << newH << std::endl;

	newW = std::max(newW, (int32_t) source->w);
	newH = std::max(newH, (int32_t) source->h);
	//newW = std::max(newW, maxx);
	//newH = std::max(newH, maxy);
	std::cout << "image new size:" << newW << "*" << newH << std::endl;

	Image newSource;
	// newSource.data = (uint8_t *) calloc((maxx - minx + 1) * (maxy - miny + 1) * source->comp, sizeof(uint8_t));
	// newSource.data = (uint8_t *) calloc((maxx - minx) * (maxy - miny) * source->comp, sizeof(uint8_t));
	newSource.data = (uint8_t *) calloc(newW * newH * source->comp, sizeof(uint8_t));
	newSource.w = newW;
	newSource.h = newH;
	newSource.comp = 4;

	
	
	ImageUtils::copy(source, &newSource, (newSource.w - source->w) / 2, (newSource.h - source->h) / 2);

	//ImageUtils::saveImageAsPNG("newSource.png", &newSource);

	//ImageUtils::copy(source, &newSource, 
	//	newSource.w > source->w ? (newSource.w - source->w) / 2 : 0, 
	//	newSource.h > source->h ? (newSource.h - source->h) / 2 : 0);

	// target->data = (uint8_t *) calloc((maxx - minx) * (maxy - miny) * source->comp, sizeof(uint8_t));
	target->data = (uint8_t *) calloc(newW * newH * source->comp, sizeof(uint8_t));
	target->w = newW;
	target->h = newH;
	target->comp = source->comp;
	//for (uint32_t i = 0; i < target->w * target->h * target->comp; i++) target->data[i] = 255;

	for (uint32_t y = 0; y < target->h; y++) {
		for (uint32_t x = 0; x < target->w; x++) {
			Raster::rotatePixel(-angle, { (int32_t) target->w / 2, (int32_t) target->h / 2 }, { (int32_t) x, (int32_t) y }, rp);
			if (rp.x >= 0 && rp.x < (int32_t) newSource.w && rp.y >= 0 && rp.y < (int32_t) newSource.h) {
				memcpy(
					target->data + ((x + y * target->w) * target->comp),
					newSource.data + ((rp.x + rp.y * newSource.w) * newSource.comp),
					newSource.comp);
			}
		}
	}

	 
	//for (uint32_t y = 0; y < std::max(source->h, target->h); y++) {
	//	for (uint32_t x = 0; x < std::max(source->w, target->w); x++) {
	//		Raster::rotatePixel(-angle, { (int32_t) target->w / 2, (int32_t) target->h / 2 }, { (int32_t) x, (int32_t) y }, rp);
	//		if (rp.x >= 0 && rp.x < (int32_t) newSource.w && rp.y >= 0 && rp.y < (int32_t) newSource.h) {
	//			memcpy(
	//				target->data + ((x + y * target->w) * target->comp),
	//				newSource.data + ((rp.x + rp.y * newSource.w) * newSource.comp),
	//				newSource.comp);
	//		}
	//	}
	//}

	//ImageUtils::saveImageAsPNG("temp.png", target);

	freeImage(&newSource);

}

bool ImageUtils::checkCoordinatesAfterMargin(const Pixel &p, int32_t margin, const Image &im)
{
	if (p.x < margin || p.y < margin)
		return false;
	if (p.x > (int32_t) im.w /*- margin*/ || p.y > (int32_t) im.h /*- margin*/)
		return false;

	return true;
}

void ImageUtils::getImageCoordinatesAfterMargin(const Pixel &p, Pixel &q, int32_t margin)
{
	q = { p.x - margin, p.y - margin };
}


void ImageUtils::copy(const Image *source, Image *target, int32_t x, int32_t y) {
	for (uint32_t j = 0; j < source->h; j++) {
		for (uint32_t i = 0; i < source->w; i++) {
			memcpy(
				target->data + (((x + i) + (y + j) * target->w)) * target->comp,
				source->data + ((i + j * source->w) * source->comp),
				source->comp);
		}
	}
}