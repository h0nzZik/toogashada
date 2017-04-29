#include <common/geometry/collision.h>
#include <iostream>
#include <common/Geometry.h>
#include <common/geometry/Object2D.h>
#include "doctest.h"

using namespace std;

template < typename T1, typename T2 >
bool testCollision(T1 const & x1, T2 const &x2) {
	bool result = collision(x1, x2);
	CHECK(result == collision(x2, x1));
	return result;
}

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

#if 0
	SUBCASE("MinMax projection") {
		auto triangle = Polygon{{1, 1}, {1, 2}, {2, 2}};
		MinMax mm = minmaxProjection(triangle, {0, 1});
		cout << "max: " << mm.max << ", min: " << mm.min << endl;
	}
#endif

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
			auto const obj1 = createObject2D({0, 0}, 0, playerShape);
			auto const obj2 = createObject2D({10, 0}, 0, playerShape);
			auto const obj3 = createObject2D({5, 5}, 0, playerShape);
			auto const obj4 = createObject2D({50, 5}, 0, playerShape);

			CHECK(testCollision(obj1, obj2));
			CHECK(testCollision(obj1, obj3));
			CHECK(testCollision(obj2, obj3));
			CHECK(!testCollision(obj4, obj1));
			CHECK(!testCollision(obj4, obj2));
			CHECK(!testCollision(obj4, obj3));

		}

		SUBCASE("My killer object") {
			//Polygon p1{{34.2399,24.84}, {24.2399,14.84}, {64.2399,14.84}, {54.2399,24.84}};
//			Polygon p1{{10,20}, {0,10}, {40,10}, {30,20}};
//			Polygon p2{{42,10}, {32,0}, {72,0}, {42,10}};
//			CHECK(!testCollision(p1, p2));

			//Polygon p3{{34.2399,24.84}, {24.2399,14.84}, {64.2399,14.84}, {54.2399,24.84}};
			//Polygon p4{{67,15}, {57,5}, {97,5}, {87,15}};
			Polygon p3{{10,24.84}, {0,14.84}, {40,14.84}, {30,24.84}};
			Polygon p4{{43,15}, {33,5}, {73,5}, {63,15}};
			CHECK(!testCollision(p3, p4));
		}
	}
}
