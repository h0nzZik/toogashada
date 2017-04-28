/**
 * See https://gamedevelopment.tutsplus.com/tutorials/collision-detection-using-the-separating-axis-theorem--gamedev-169
 */
#include <common/geometry/collision.h>
#include <limits>
#include <algorithm>

#include <common/Geometry.h>
#include <common/geometry/Circle.h>

using namespace std;

namespace geometry {

bool collision(Circle const &c1, Circle const &c2) {
	return distance(c1.center, c2.center) <= (c1.radius + c2.radius);
}

MinMax minmaxProjection(Polygon const &polygon, Vector axis) {
	MinMax minmax = {Scalar(0), numeric_limits<Scalar>::max()};
	for(Point const & point : polygon) {
		Vector v = point - Point{0,0};
		minmax.min = min(minmax.min, dot(v, axis));
		minmax.max = min(minmax.max, dot(v, axis));
	}
	return minmax;
}


bool gapAlongAxis(Vector axis, Polygon const &p1, Polygon const &p2) {
	MinMax r1 = minmaxProjection(p1, axis);
	MinMax r2 = minmaxProjection(p2, axis);

	return (r1.max < r2.min) || (r2.max < r1.min);
}

void getNormals(std::vector<Vector> &normals, Polygon const &polygon) {
	if (polygon.empty())
		return;

	Point prev = polygon.back();
	for(auto const & curr : polygon) {
		normals.push_back(unit(curr - prev));
		prev = curr;
	}
}

bool collision(Polygon const &p1, Polygon const &p2) {
	std::vector<Vector> normals;
	getNormals(normals, p1);
	getNormals(normals, p2);

	for (auto const &n : normals) {
		if (gapAlongAxis(n, p1, p2))
			return false;
	}

	return true;
}

bool collision(Polygon const &polygon, Circle const &circle) {
	return false;
}

bool collision(Circle const &circle, Polygon const &polygon) {
	return collision(polygon, circle);
}

} // namespace geometry
