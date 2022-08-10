#include <cinttypes>
#include <cstdint>
#include <math.h>

#ifndef STRUCTS_HPP
#define STRUCTS_HPP


namespace st {

	enum distribution { POISSON, JITTERED, VOGEL, UNIFORM };

	typedef struct Point {
		double x;
		double y;
	} Point;

	typedef struct Triangle {
		Point a;
		Point b;
		Point c;
	} Triangle;

	typedef struct Rectangle {
		int32_t x1;
		int32_t y1;
		int32_t x2;
		int32_t y2;
	} Rectangle;


	typedef struct Pixel {
		int32_t x;
		int32_t y;
	} Pixel;

	typedef struct PixelWithAngle {
		Pixel p;
		float a;
	} PixelWithAngle;

	typedef struct TrianglePixels {
		Pixel a;
		Pixel b;
		Pixel c;
	} TrianglePixels;

	typedef struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	} Color;

	typedef struct Colorf {
		float r;
		float g;
		float b;
		float a;
	} Colorf;

	typedef struct Image {
		uint8_t *data;
		uint32_t h, w;
		uint8_t comp;
	} Image;



	static int64_t hashItAsInt(const Pixel &p) {
		return ((int64_t)  (((int64_t) p.x) << 32) | (((int32_t) p.y)));
	}

	struct PixelCompareStruct
	{
		Pixel key;
		double centerx, centery;

		st::PixelCompareStruct(Pixel k, const double cx, const double cy) : key(k), centerx(cx), centery(cy) {}
	};

	struct PixelAngleCompare
	{
		inline bool operator() (const st::PixelCompareStruct &struct1, const st::PixelCompareStruct &struct2)
		{
			double a1 = atan2(struct1.key.x - struct1.centerx, struct1.key.y - struct1.centery);
			double a2 = atan2(struct2.key.x - struct2.centerx, struct2.key.y - struct2.centery);

			// std::cout << "c=(" << struct2.centerx << "," << struct2.centery << ") " << a1 << "(" << struct1.key.getX() << "," << struct1.key.getY() << ")" << " vs " << a2 << "(" << struct2.key.getX() << "," << struct2.key.getY() << ")" << std::endl;
			return (a1 < a2);
		}
	};
}

#endif