#include "vertex.hpp"

// Constructors
Vertex::Vertex()
{
	x = 0;
	y = 0;
}

Vertex::Vertex(const Vertex &copy)
{
	x = copy.getX();
	y = copy.getY();
}

Vertex::Vertex(int32_t x, int32_t y)
{
	this->x = x;
	this->y = y;
}


// Destructor
Vertex::~Vertex()
{
}


// Operators
Vertex & Vertex::operator=(const Vertex &assign)
{
	x = assign.getX();
	y = assign.getY();
	return *this;
}




// Getters / Setters
int32_t Vertex::getX() const
{
	return x;
}
int32_t Vertex::getY() const
{
	return y;
}
