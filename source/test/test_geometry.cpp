#include <common/geometry/collision.h>
#include <iostream>
#include <common/Geometry.h>
#include <common/geometry/Object2D.h>
#include "doctest.h"

using namespace std;

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

	SUBCASE("MinMax projection") {
		auto triangle = Polygon{{1, 1}, {1, 2}, {2, 2}};
		MinMax mm = minmaxProjection(triangle, {0, 1});
		cout << "max: " << mm.max << ", min: " << mm.min << endl;
	}

	SUBCASE("Collision") {
		auto tr1 = Polygon{{1.0, 1.0}, {1.0, 2.0}, {2.0, 1.0}};
		auto tr2 = Polygon{{1.5, 2}, {2.5, 2}, {2.5, 1}};

		SUBCASE("Along given axis") {
			CHECK(true == gapAlongAxis(Vector{1, 1}, tr1, tr2));
			CHECK(false == gapAlongAxis(Vector{-1, 1}, tr1, tr2));
		}

		SUBCASE("Full - simple") {
			CHECK(false == collision(tr1, tr2));
		}

		SUBCASE("Full - real game objects") {
			geometry::PolygonalShape const playerShape = {
				{-10, 0},
				{-20, -10},
				{+20, -10},
				{+10, 0}
			};
			auto obj1 = createObject2D({0, 0}, 0, playerShape);
			auto obj2 = createObject2D({10, 0}, 0, playerShape);
			CHECK(collision(obj1, obj2));



		}
	}
}
