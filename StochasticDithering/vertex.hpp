#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <cmath>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>


#define N 9999


class Vertex {
public:
	// Constructors
	Vertex();
	Vertex(const Vertex &copy);
	Vertex(int32_t x, int32_t y);

	static void padTo(std::string &str, const size_t num, const char paddingChar = '*')
	{
		if (num > str.size())
			str.insert(0, num - str.size(), paddingChar);
	}

	static void hashItAsString(const Vertex &v, std::string &h) {
		std::string sx = std::to_string(v.x);
		Vertex::padTo(sx, 11);

		h.append(sx);
		h.append(std::to_string(v.y));
	}

	static int64_t hashItAsInt(const Vertex &v) {

		// std::cout << ((int64_t) v.x) + (N * (int64_t) v.y) << std::endl;

		return ((int64_t) v.x) + (N * (int64_t) v.y);
	}

	// Destructor
	~Vertex();

	// Operators
	Vertex &operator=(const Vertex &assign);

	bool operator<(const Vertex &v) const
	{
		int64_t h1 = Vertex::hashItAsInt(*this);
		int64_t h2 = Vertex::hashItAsInt(v);
		return h1 < h2;
	}

	friend bool operator==(const Vertex &p1, const Vertex &p2)
	{
		return p1.x == p2.x && p1.y == p2.y;
	}

	// Getters / Setters
	int32_t getX() const;
	int32_t getY() const;

private:
	int32_t x;
	int32_t y;
};


struct VertexCompareStruct
{
	Vertex key;
	double centerx, centery;

	VertexCompareStruct(Vertex k, const double cx, const double cy) : key(k), centerx(cx), centery(cy) {}
};

struct VertexAngleCompare
{
	inline bool operator() (const VertexCompareStruct &struct1, const VertexCompareStruct &struct2)
	{
		double a1 = atan2(struct1.key.getX() - struct1.centerx, struct1.key.getY() - struct1.centery);
		double a2 = atan2(struct2.key.getX() - struct2.centerx, struct2.key.getY() - struct2.centery);

		// std::cout << "c=(" << struct2.centerx << "," << struct2.centery << ") " << a1 << "(" << struct1.key.getX() << "," << struct1.key.getY() << ")" << " vs " << a2 << "(" << struct2.key.getX() << "," << struct2.key.getY() << ")" << std::endl;
		return (a1 < a2);
	}
};



#endif