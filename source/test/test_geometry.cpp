#include <common/Geometry.h>
#include "doctest.h"

TEST_CASE("Basic geometry") {
	using namespace geometry;
	SUBCASE("Distance of points") {
		CHECK(distance({1, 1}, {4, 5}) == 5);
	}

	SUBCASE("Dot product") {
		CHECK(dot({2, 1}, {1, 2}) == 4);
	}

	SUBCASE("Projection") {
		CHECK((project(Vector{4, 4}, Vector{0, 1}) == Vector{0, 4}));
		CHECK((project({3, 3}, {0.5, 0}) == Vector{3, 0}));
	}
}
