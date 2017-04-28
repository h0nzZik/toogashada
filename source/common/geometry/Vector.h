#pragma once

#include <common/Scalar.h>

namespace geometry {

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

} // namespace geometry
