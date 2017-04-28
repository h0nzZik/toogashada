#include <common/Geometry.h>
#include "doctest.h"

TEST_CASE("Basic geometry") {
	SUBCASE("Distance of points") {
		CHECK(distance({1, 1}, {4, 5}) == 5);
	}
}
