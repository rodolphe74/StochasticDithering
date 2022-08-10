#ifndef VORONOIUTILS_HPP
#define VORONOIUTILS_HPP

#include "raster.hpp"
#include "structs.hpp"
#include "vertex.hpp"
//#include <delynoi/models/Region.h>
//#include <delynoi/models/basic/Point.h>
//#include <delynoi/models/generator/functions/functions.h>
//#include <delynoi/models/polygon/Triangle.h>
//#include <delynoi/voronoi/TriangleVoronoiGenerator.h>

#include "delaunator.hpp"

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <stdint.h>

#define EPSILON 0.001
typedef std::map<Vertex, std::set<Vertex>> graph;
typedef std::pair<Vertex, std::set<Vertex>> pair;

class VoronoiUtils {
public:

	std::map<int64_t, std::vector<Pixel> *> polygonsC;
	std::map<int64_t, std::vector<Pixel> *> polygonsM;
	std::map<int64_t, std::vector<Pixel> *> polygonsY;
	std::map<int64_t, std::vector<Pixel> *> polygonsK;


	// Constructors
	//VoronoiUtils(st::Pixel a, st::Pixel b, st::Pixel c, st::Pixel d);
	VoronoiUtils();

	void createPoissonSampling(uint32_t w, uint32_t h, std::vector<st::Pixel> &pixels, int numPoints);
	void createJitteredSampling(uint32_t w, uint32_t h, std::vector<st::Pixel> &pixels, int numPoints);
	void createVogelSampling(uint32_t w, uint32_t h, std::vector<st::Pixel> &pixels, int numPoints);
	void createUniformSampling(uint32_t w, uint32_t h, std::vector<st::Pixel> &pixels);
	void delaunay(std::vector<st::Pixel> seeds, std::vector<st::TrianglePixels> &triangles);
	void graphit(std::vector<st::TrianglePixels> &triangles, graph &trianglesMap);
	int equals(st::Pixel v, st::Pixel w) { return v.x == w.x && v.y == w.y; }
    void sortVerticesByAngle(const std::set<Vertex> &vertices, std::vector<Vertex> &sortedVertices);
	static void sortPixelsByAngle(const std::vector<Pixel> &pixels, std::vector<Pixel> &sortedPixels);
	uint8_t getMeanGray(const st::TrianglePixels &t, Raster &r, const Image &im);
	void getDot(const graph &triangleMap, const Vertex &key, Raster &r, const Image &im, std::vector<Pixel> &pixels);
	static void addToPolygons(std::map<int64_t, std::vector<Pixel> *> &polygons, Pixel p, Pixel q, Pixel r);
	static void freePolygons(std::map<int64_t, std::vector<Pixel> *> &polygons);
	static void hull(std::vector<Pixel> a, std::vector<Pixel> &result);
	static void saveDot(const graph &triangleMap, const Vertex &key, Raster &r, const Image &im, VoronoiUtils &vu, std::map<int64_t, std::vector<Pixel> *> &polygons, int32_t maxWidth, int32_t maxHeight, int32_t margin, st::Colorf color, float angle);

	// Destructor
	~VoronoiUtils();

private:
	std::vector<Point> vec;
	//Region square;
	//Point convert(st::Pixel);
	//st::TrianglePixels getTriangle(Triangle, delaunator::Delaunator &);
	void addVertex(Vertex &v, graph &trianglesMap);
	void addEdge(Vertex &v, Vertex &w, graph &trianglesMap);
};

#endif