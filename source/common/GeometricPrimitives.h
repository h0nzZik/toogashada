#pragma once

#include <vector>
#include "Scalar.h"

struct Point {
	Scalar x;
	Scalar y;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(x,y);
	}
};

inline bool operator==(Point const &l, Point const &r) {
	return (l.x == r.x) && (l.y == r.y);
}

inline bool operator!=(Point const &l, Point const &r) {
	return !(l == r);
}

struct Vector {
	Scalar x;
	Scalar y;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(x,y);
	}
};

inline bool operator==(Vector const &l, Vector const &r) {
	return (l.x == r.x) && (l.y == r.y);
}

inline bool operator!=(Vector const &l, Vector const &r) {
	return !(l == r);
}

using PolygonalShape = std::vector<Point>;

struct CircleShape {
	Scalar radius;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(radius);
	}
};
