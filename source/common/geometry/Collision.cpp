#include <common/Geometry.h>
#include "Collision.h"

namespace geometry {

bool collision(
		Point const &p1, CircleShape const &c1,
		Point const &p2, CircleShape const &c2) {

	return distance(p1, p2) <= (c1.radius + c2.radius);

}

} // namespace geometry
