#include "voronoiUtils.hpp"
#include "vertex.hpp"
#include "ImageUtils.hpp"
#include <PoissonGenerator.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <set>
#include <sys/types.h>
#include <utility>
#include <vector>


VoronoiUtils::VoronoiUtils() {

}


void VoronoiUtils::delaunay(
	std::vector<st::Pixel> seeds, std::vector<st::TrianglePixels> &triangles)
{
	std::vector<double> coords;
	for (auto p = seeds.begin(); p != seeds.end(); ++p) {
		coords.push_back(p->x);
		coords.push_back(p->y);
	}
	delaunator::Delaunator d(coords);

	for (std::size_t i = 0; i < d.triangles.size(); i += 3) {

		st::Pixel a = { /*.x =*/ (int32_t) ceil(d.coords[2 * d.triangles[i]]), /*.y =*/ (int32_t) ceil(d.coords[2 * d.triangles[i] + 1]) };
		st::Pixel b = { /*.x =*/ (int32_t) ceil(d.coords[2 * d.triangles[i + 1]]), /*.y =*/ (int32_t) ceil(d.coords[2 * d.triangles[i + 1] + 1]) };
		st::Pixel c = { /*.x =*/ (int32_t) ceil(d.coords[2 * d.triangles[i + 2]]), /*.y =*/ (int32_t) ceil(d.coords[2 * d.triangles[i + 2] + 1]) };
		st::TrianglePixels trianglePixels = { /*.a =*/ a, /*.b =*/ b, /*.c =*/ c };

		triangles.push_back(trianglePixels);
	}
}




void VoronoiUtils::addVertex(Vertex &v, graph &trianglesMap)
{
	try {
		std::set<Vertex> s = trianglesMap.at(v);
	} catch (const std::out_of_range &/*oor*/) {
		std::set<Vertex> set;
		/*
		insert() takes a reference argument to avoid the copy when passing the argument.
		but it creates a copy when stores the item in the collection.
		*/
		//std::cerr << oor.what() << std::endl;
		trianglesMap.insert(pair(v, set));
	}
}

void VoronoiUtils::addEdge(Vertex &v, Vertex &w, graph &trianglesMap)
{
	try {
		std::set<Vertex> set = trianglesMap.at(v);
		set.insert(w);
		trianglesMap[v] = set;
	} catch (const std::out_of_range &/*oor*/) {
		//std::cerr << oor.what() << std::endl;
		// not found
	}
}

void VoronoiUtils::graphit(std::vector<st::TrianglePixels> &triangles, graph &trianglesMap)
{
	int count = 0;
	for (auto t = triangles.begin(); t != triangles.end(); ++t) {

		Vertex a(t->a.x, t->a.y);
		Vertex b(t->b.x, t->b.y);
		Vertex c(t->c.x, t->c.y);

		addVertex(a, trianglesMap);
		addVertex(b, trianglesMap);
		addVertex(c, trianglesMap);

		addEdge(a, b, trianglesMap);
		addEdge(b, a, trianglesMap);
		addEdge(a, c, trianglesMap);
		addEdge(c, a, trianglesMap);
		addEdge(b, c, trianglesMap);
		addEdge(c, b, trianglesMap);
	}
}


void VoronoiUtils::createPoissonSampling(uint32_t w, uint32_t h, std::vector<st::Pixel> &pixels, int numPoints)
{
	PoissonGenerator::DefaultPRNG PRNG;
	std::vector < PoissonGenerator:: Point > points = PoissonGenerator::generatePoissonPoints(numPoints, PRNG, false);

	std::cout <<  "Poisson:" << points.size() << std::endl;

	for (auto i = points.begin(); i != points.end(); ++i) {
		int x = (int) roundf(i->x * w);
		int y = (int) roundf(i->y * h);
		//if (x < 0 || y < 0 || x >= w || y >= h)
		//	continue;
		Pixel pixel = { x, y };
		pixels.push_back(pixel);
	}
}


void VoronoiUtils::createJitteredSampling(uint32_t w, uint32_t h, std::vector<st::Pixel> &pixels, int numPoints)
{
	PoissonGenerator::DefaultPRNG PRNG;
	std::vector < PoissonGenerator::Point > points = PoissonGenerator::generateJitteredGridPoints(numPoints, PRNG, false);

	std::cout << "Poisson:" << points.size() << std::endl;

	for (auto i = points.begin(); i != points.end(); ++i) {
		int x = (int) roundf(i->x * w);
		int y = (int) roundf(i->y * h);
		//if (x < 0 || y < 0 || x >= w || y >= h)
		//	continue;
		Pixel pixel = { x, y };
		pixels.push_back(pixel);
	}
}


void VoronoiUtils::createVogelSampling(uint32_t w, uint32_t h, std::vector<st::Pixel> &pixels, int numPoints)
{
	PoissonGenerator::DefaultPRNG PRNG;
	std::vector < PoissonGenerator::Point > points = PoissonGenerator::generateVogelPoints(numPoints, false);

	std::cout << "Poisson:" << points.size() << std::endl;

	for (auto i = points.begin(); i != points.end(); ++i) {
		int x = (int) roundf(i->x * w);
		int y = (int) roundf(i->y * h);
		//if (x < 0 || y < 0 || x >= w || y >= h)
		//	continue;
		Pixel pixel = { x, y };
		pixels.push_back(pixel);
	}
}



void VoronoiUtils::createUniformSampling(uint32_t w, uint32_t h, std::vector<st::Pixel> &pixels)
{

	// http://extremelearning.com.au/isotropic-blue-noise-point-sets/
	int rowPermutation[] = { 1, 5, 8, 7, 4, 6, 2, 3 };
	int colPermutation[] = { 1, 3, 2, 6, 4, 7, 8, 5 };

	//int rowPermutation[] = { 1, 4, 5, 2, 6, 11, 10, 12, 8, 3, 9, 7 };
	//int colPermutation[] = { 1, 3, 6, 2, 8, 12, 11, 9, 4, 5, 10, 7 };

	// int rowPermutation[] = { 1, 6, 2, 10, 20, 12, 5, 4, 7, 14, 16, 17, 8, 3, 9,
	// 18, 15, 19, 13, 11 };
	// int colPermutation[] = { 1, 9, 15, 7, 10, 3, 5, 6, 13, 4, 14, 19, 18, 16, 20,
	// 17, 12, 8, 2, 11 };
	 //int rowPermutation[] = {1, 5, 4, 18, 2, 7, 10, 12, 28, 13, 3, 14, 29, 16, 9,
	 //21, 17, 30, 22, 32, 23, 20, 26, 27, 15, 24, 31, 25, 11, 19, 8, 6};
	 //int colPermutation[] = {1, 13, 8, 2, 18, 6, 10, 3, 5, 15, 7, 20, 4, 9, 12,
	 //26, 22, 23, 32, 21, 28, 19, 27, 24, 30, 17, 16, 31, 29, 14, 25, 11};

	uint32_t rows = h / (sizeof(rowPermutation) / sizeof(int));
	uint32_t cols = w / (sizeof(colPermutation) / sizeof(int));
	uint32_t size = sizeof(rowPermutation) / sizeof(int);
	int *screen = new int[w * h];

	uint32_t col = 0, row = 0;
	for (uint32_t y = 0; y < rows; y++) {
		for (uint32_t x = 0; x < cols; x++) {
			uint32_t yIdx = y * size + colPermutation[col];
			uint32_t xIdx = x * size + rowPermutation[row];
			if (xIdx < w && yIdx < h) {
				screen[yIdx * w + xIdx] = 1;
			}
			col++;
			if (col == size) {
				col = 0;
			}
		}
		row++;
		col = 0;
		if (row == size) {
			row = 0;
		}
	}

	for (uint32_t y = 0; y < h; y++) {
		for (uint32_t x = 0; x < w; x++) {
			if (screen[y * w + x] == 1) {
                pixels.push_back(st::Pixel { /*.x =*/ (int32_t) x, /*.y =*/ (int32_t) y });
			}
		}
	}

	delete[] screen;
}

uint8_t VoronoiUtils::getMeanGray(const st::TrianglePixels &t, Raster &r, const Image &im)
{
	int32_t totalGray = 0;
	std::vector<Pixel> p;
	r.getTrianglePoints(t.a, t.b, t.c, p);
	
	int count = 0;

	for (std::vector<Pixel>::const_iterator i = p.begin(); i != p.end(); ++i) {
		//std::cout << "  current pixel:" << (int) im.data[i->x * im.comp + i->y * im.w * im.comp] << std::endl;
		if (i->x > -1 &&  i->x < (int32_t) im.w && i->y > -1  && i->y < (int32_t) im.h) {
			totalGray += im.data[i->x * im.comp + i->y * im.w * im.comp];
			count++;
		}
	}
	return (uint8_t)(totalGray / count);
}

void VoronoiUtils::sortVerticesByAngle(
	const std::set<Vertex> &vertices, std::vector<Vertex> &sortedVertices)
{
	double x = 0;
	double y = 0;
	for (std::set<Vertex>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
		x += i->getX();
		y += i->getY();
	}

	double centerx = x / vertices.size();
	double centery = y / vertices.size();

	std::vector<VertexCompareStruct> sortingVector;
	for (std::set<Vertex>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
		VertexCompareStruct cs(*i, centerx, centery);
		sortingVector.push_back(cs);
	}
	std::sort(sortingVector.begin(), sortingVector.end(), VertexAngleCompare());

	for (std::vector<VertexCompareStruct>::const_iterator i = sortingVector.begin();
		 i != sortingVector.end(); ++i) {
		sortedVertices.push_back(i->key);
	}
}

void VoronoiUtils::sortPixelsByAngle(const std::vector<Pixel> &pixels, std::vector<Pixel> &sortedPixels)
{
	double x = 0;
	double y = 0;
	for (std::vector<Pixel>::const_iterator i = pixels.begin(); i != pixels.end(); ++i) {
		x += i->x;
		y += i->y;
	}

	double centerx = x / pixels.size();
	double centery = y / pixels.size();

	std::vector<st::PixelCompareStruct> sortingVector;
	for (std::vector<Pixel>::const_iterator i = pixels.begin(); i != pixels.end(); ++i) {
		st::PixelCompareStruct cs(*i, centerx, centery);
		sortingVector.push_back(cs);
	}
	std::sort(sortingVector.begin(), sortingVector.end(), st::PixelAngleCompare());

	for (std::vector<st::PixelCompareStruct>::const_iterator i = sortingVector.begin();
		i != sortingVector.end(); ++i) {
		sortedPixels.push_back(i->key);
	}
}



void VoronoiUtils::getDot(const graph &triangleMap, const Vertex &key, Raster &r, const Image &im, std::vector<Pixel> &pixels)
{
	std::set<Vertex> neighbors = triangleMap.at(key);
	std::vector<Vertex> sortedNeighbors;
    std::vector<Pixel> trianglePixels, t1, t2, t3;
	sortVerticesByAngle(neighbors, sortedNeighbors);

    // std::cout << "Working on key " << Vertex::hashItAsInt(key) << "  " << key.getX() << "," << key.getY() << std::endl;

    #define MARGIN 20
    if (key.getX() < MARGIN || key.getY() < MARGIN) return;
    if (key.getX() > (int32_t) (im.w - MARGIN) || key.getY() > (int32_t) (im.h - MARGIN)) return;

    

	for (int i = 0; i < sortedNeighbors.size(); i++) {
		Vertex current = sortedNeighbors.at(i);
		Vertex next = sortedNeighbors.at((i + 1) % sortedNeighbors.size());
   		st::Pixel a = Pixel { (int32_t) key.getX(), (int32_t) key.getY() };
		st::Pixel b = Pixel { (int32_t) current.getX(), (int32_t) current.getY() };
		st::Pixel c = Pixel { (int32_t) next.getX(), (int32_t) next.getY() };


        st::TrianglePixels t = {a, b, c};
        uint8_t gray = getMeanGray(t, r, im);

        double g = 255.0 / (double) gray;
        double k = 0;
        if (g <= 0.25) {
            k = sqrt((1.0 - g) / 3.0);
        } else {
            k = (2.0 - sqrt(g)) / 3.0;
        }

        std::cout << "gray:" << (int32_t) gray << " g=" << g << " k=" << k << std::endl;

        st::Pixel abk = Pixel { (int32_t) round(a.x + (b.x - a.x) * k), (int32_t) round(a.y + (b.y - a.y) * k) };
        st::Pixel ack = Pixel { (int32_t) round(a.x + (c.x - a.x) * k), (int32_t) round(a.y + (c.y - a.y) * k) };

        st::Pixel bak = Pixel { (int32_t) round(b.x + (a.x - b.x) * k), (int32_t) round(b.y + (a.y - b.y) * k) };
        st::Pixel bck = Pixel { (int32_t) round(b.x + (c.x - b.x) * k), (int32_t) round(b.y + (c.y - b.y) * k) };

        st::Pixel cak = Pixel { (int32_t) round(c.x + (a.x - c.x) * k), (int32_t) round(c.y + (a.y - c.y) * k) };
        st::Pixel cbk = Pixel { (int32_t) round(c.x + (b.x - c.x) * k), (int32_t) round(c.y + (b.y - c.y) * k) };

        t1.clear(); t2.clear(); t3.clear();
        r.getTrianglePoints(a, abk, ack, t1);
        r.getTrianglePoints(b, bak, bck, t2);
        r.getTrianglePoints(c, cak, cbk, t3);

        // std::cout << "tri1:" << a.x << "," << a.y << " - " << abk.x << "," << abk.y << " - " << ack.x << "," << ack.y << std::endl;
        // std::cout << "tri2:" << b.x << "," << b.y << " - " << bak.x << "," << bak.y << " - " << bck.x << "," << bck.y << std::endl;
        // std::cout << "tri3:" << c.x << "," << c.y << " - " << cak.x << "," << cak.y << " - " << cbk.x << "," << cbk.y << std::endl << std::endl;
        
        pixels.insert(std::end(pixels), std::begin(t1), std::end(t1));
        pixels.insert(std::end(pixels), std::begin(t2), std::end(t2));
        pixels.insert(std::end(pixels), std::begin(t3), std::end(t3));
	}
}

void VoronoiUtils::addToPolygons(std::map<int64_t, std::vector<Pixel> *> &polygons, Pixel p, Pixel q, Pixel r)
{
	int64_t h = st::hashItAsInt(p);
	auto found = polygons.find(h);
	if (found == polygons.end()) {
		std::vector<Pixel> *v = new std::vector<Pixel>();
		v->push_back(q);
		v->push_back(r);
		std::pair<int64_t, std::vector<Pixel> *> pair(h, v);
		polygons.insert(pair);
	}
	else {
		std::vector<Pixel> *v = polygons.at(h);
		v->push_back(q);
		v->push_back(r);
	}
}

void VoronoiUtils::freePolygons(std::map<int64_t, std::vector<Pixel> *> &polygons)
{
	for (auto p = polygons.begin(); p != polygons.end(); ++p) {
		delete p->second;
	}
}


int line_dist(Pixel p1, Pixel p2, Pixel p)
{
	// returns a value proportional to the distance
	// between the point p and the line joining the
	// points p1 and p2
	return abs((p.y - p1.y) * (p2.x - p1.x) -
		(p2.y - p1.y) * (p.x - p1.x));
}

int find_side(Pixel p1, Pixel p2, Pixel p)
{
	// Returns the side of point p with respect to line
	// joining points p1 and p2.
	int val = (p.y - p1.y) * (p2.x - p1.x) -
		(p2.y - p1.y) * (p.x - p1.x);

	if (val > 0)
		return 1;
	if (val < 0)
		return -1;
	return 0;
}

void quick_hull(std::vector<Pixel> a, int n, Pixel p1, Pixel p2, int side, std::vector<Pixel> &hull)
{
	// End points of line L are p1 and p2.  side can have value
	// 1 or -1 specifying each of the parts made by the line L

	int ind = -1;
	int max_dist = 0;

	// finding the point with maximum distance
	// from L and also on the specified side of L.
	for (int i = 0; i < n; i++)
	{
		int temp = line_dist(p1, p2, a[i]);
		if (find_side(p1, p2, a[i]) == side && temp > max_dist)
		{
			ind = i;
			max_dist = temp;
		}
	}

	// If no point is found, add the end points
	// of L to the convex hull.
	if (ind == -1)
	{
		hull.push_back(p1);
		hull.push_back(p2);
		return;
	}

	// Recur for the two parts divided by a[ind]
	quick_hull(a, n, a[ind], p1, -find_side(a[ind], p1, p2), hull);
	quick_hull(a, n, a[ind], p2, -find_side(a[ind], p2, p1), hull);
}

int float_comparator(const void *p, const void *q)
{
	float l = ((const PixelWithAngle *) p)->a;
	float r = ((const PixelWithAngle *) q)->a;

	if (l == r) {
		return 0;
	}

	if (l > r) {
		return 1;
	}

	return -1;
}


void VoronoiUtils::hull(std::vector<Pixel> a, std::vector<Pixel> &result)
{
	// a[i].second -> y-coordinate of the ith point
	if (a.size() < 3)
	{
		return;
	}

	// Finding the point with minimum and
	// maximum x-coordinate
	int xMin = 0, xMax = 0;
	for (int i = 1; i < a.size(); i++)
	{
		if (a[i].x < a[xMin].x) {
			xMin = i;
		}
		if (a[i].x > a[xMax].x) {
			xMax = i;
		}
	}

	// Recursively find convex hull points on
	// one side of line joining a[min_x] and
	// a[max_x]
	std::vector<Pixel> hull_points;
	quick_hull(a, (int) a.size(), a[xMin], a[xMax], 1, hull_points);

	// Recursively find convex hull points on
	// other side of line joining a[min_x] and
	// a[max_x]
	quick_hull(a, (int) a.size(), a[xMin], a[xMax], -1, hull_points);


	// find center
	Pixel pt, center;
	int x = 0, y = 0;
	int hull_size = (int) hull_points.size();
	for (int i = 0; i < hull_points.size(); i++) {
		pt = hull_points.at(i);
		x += pt.x;
		y += pt.y;
	}
	center.x = x / hull_size;
	center.y = y / hull_size;

	// reorder point in clockwise order
	PixelWithAngle *pa = new PixelWithAngle[hull_points.size()];

	int count = 0;
	for (int i = 0; i < hull_points.size(); i++) {
		pt = hull_points.at(i);

		float a, b;
		a = (float) (pt.y - center.y);
		b = (float) (pt.x - center.x);
		float angle = atan2(a, b);

		pa[i].a = angle;
		pa[i].p.x = pt.x;
		pa[i].p.y = pt.y;
	}
	
	qsort((void *) pa, hull_size, sizeof(pa[0]), float_comparator);

	for (int i = 0; i < (int) hull_points.size(); i++) {
		int add = 1;
		for (int j = 0; j < (result).size(); j++) {
			Pixel cp;
			cp = result.at(j);

			if (cp.x == pa[i].p.x && cp.y == pa[i].p.y) {
				add = 0;
				break;
			}
		}
		if (add) {
			result.push_back(pa[i].p);
		}
	}
	delete[] pa;
}


void VoronoiUtils::saveDot(const graph &triangleMap, const Vertex &key, Raster &r, const Image &im, VoronoiUtils &vu, std::map<int64_t, std::vector<Pixel> *> &polygons, int32_t maxWidth, int32_t maxHeight, int32_t margin, st::Colorf color, float angle)
{
	std::set<Vertex> neighbors = triangleMap.at(key);
	std::vector<Vertex> sortedNeighbors;
	std::vector<Pixel> trianglePixels, t1, t2, t3;
	vu.sortVerticesByAngle(neighbors, sortedNeighbors);

	Color black = { 0, 0, 0, 255 };
	st::Pixel a, b, c, _a, _b, _c;
	st::Pixel abk, ack, bak, bck, cak, cbk;
	st::TrianglePixels t;

	// if rotation
	Pixel ra, rabk, rack;
	Pixel rb, rbak, rbck;
	Pixel rc, rcak, rcbk;

	if (!ImageUtils::checkCoordinatesAfterMargin(st::Pixel{ key.getX(), key.getY() }, margin, im))
		return;

	int xshift = (maxWidth - im.w) / 2;
	int yshift = (maxHeight - im.h) / 2;

	for (int i = 0; i < sortedNeighbors.size(); i++) {
		Vertex current = sortedNeighbors.at(i);
		Vertex next = sortedNeighbors.at((i + 1) % sortedNeighbors.size());
		a = Pixel{ (int32_t) key.getX(), (int32_t) key.getY() };
		b = Pixel{ (int32_t) current.getX(), (int32_t) current.getY() };
		c = Pixel{ (int32_t) next.getX(), (int32_t) next.getY() };

		ImageUtils::getImageCoordinatesAfterMargin(a, _a, margin);
		ImageUtils::getImageCoordinatesAfterMargin(b, _b, margin);
		ImageUtils::getImageCoordinatesAfterMargin(c, _c, margin);

		t = { _a, _b, _c };
		uint8_t gray = vu.getMeanGray(t, r, im);

		double g = (double) ((gray + 1.0) / 256.0);
		double k = 0;
		//if (g <= 0.25) {
		k = sqrt((1.0 - g) / 3.0);
		//} else {
		//	k = (2.0 - sqrt(g)) / 3.0;
		//}

		// simple
		//k = 1.0 - g;


		abk = Pixel{ (int32_t) round(a.x + (b.x - a.x) * k),
			(int32_t) round(a.y + (b.y - a.y) * k) };
		ack = Pixel{ (int32_t) round(a.x + (c.x - a.x) * k),
			(int32_t) round(a.y + (c.y - a.y) * k) };

		bak = Pixel{ (int32_t) round(b.x + (a.x - b.x) * k),
			(int32_t) round(b.y + (a.y - b.y) * k) };
		bck = Pixel{ (int32_t) round(b.x + (c.x - b.x) * k),
			(int32_t) round(b.y + (c.y - b.y) * k) };

		cak = Pixel{ (int32_t) round(c.x + (a.x - c.x) * k),
			(int32_t) round(c.y + (a.y - c.y) * k) };
		cbk = Pixel{ (int32_t) round(c.x + (b.x - c.x) * k),
			(int32_t) round(c.y + (b.y - c.y) * k) };


		a = { a.x + xshift, a.y + yshift };
		abk = { abk.x + xshift, abk.y + yshift };
		ack = { ack.x + xshift, ack.y + yshift };
		b = { b.x + xshift, b.y + yshift };
		bak = { bak.x + xshift, bak.y + yshift };
		bck = { bck.x + xshift, bck.y + yshift };
		c = { c.x + xshift, c.y + yshift };
		cak = { cak.x + xshift, cak.y + yshift };
		cbk = { cbk.x + xshift, cbk.y + yshift };

		Raster::rotatePixel(angle, { maxWidth / 2, maxHeight / 2 }, a, ra);
		Raster::rotatePixel(angle, { maxWidth / 2, maxHeight / 2 }, abk, rabk);
		Raster::rotatePixel(angle, { maxWidth / 2, maxHeight / 2 }, ack, rack);

		Raster::rotatePixel(angle, { maxWidth / 2, maxHeight / 2 }, b, rb);
		Raster::rotatePixel(angle, { maxWidth / 2, maxHeight / 2 }, bak, rbak);
		Raster::rotatePixel(angle, { maxWidth / 2, maxHeight / 2 }, bck, rbck);

		Raster::rotatePixel(angle, { maxWidth / 2, maxHeight / 2 }, c, rc);
		Raster::rotatePixel(angle, { maxWidth / 2, maxHeight / 2 }, cak, rcak);
		Raster::rotatePixel(angle, { maxWidth / 2, maxHeight / 2 }, cbk, rcbk);

		// create a polygon from triangle (save sides in a map, key is center)
		VoronoiUtils::addToPolygons(polygons, ra, rabk, rack);
		VoronoiUtils::addToPolygons(polygons, rb, rbak, rbck);
		VoronoiUtils::addToPolygons(polygons, rc, rcak, rcbk);
	}
}


// Destructor
VoronoiUtils::~VoronoiUtils() { 
	VoronoiUtils::freePolygons(polygonsC);
	VoronoiUtils::freePolygons(polygonsM);
	VoronoiUtils::freePolygons(polygonsY);
	VoronoiUtils::freePolygons(polygonsK);
}
