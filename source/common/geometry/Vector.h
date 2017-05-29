#pragma once

#include <common/Scalar.h>
#include <common/Arith.h>

namespace geometry {

struct Vector : Arith {
	Scalar x;
	Scalar y;

	Vector() = default;
	Vector(Scalar x, Scalar y) : x(x), y(y) {}
	Vector(Vector const & /* orig */) = default;
	Vector(Vector && /* old */) = default;
	Vector & operator=(Vector const & /* orig */) = default;
	Vector & operator=(Vector && /* orig */) = default;

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

// To be able to insert them to set
inline bool operator<(Vector const &l, Vector const &r) {
	return (l.x < r.x) || ((l.x == r.x)&& (l.y < r.y));
}


} // namespace geometry
