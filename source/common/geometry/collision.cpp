/**
 * See https://gamedevelopment.tutsplus.com/tutorials/collision-detection-using-the-separating-axis-theorem--gamedev-169
 */

#include <limits>
#include <algorithm>
#include <set>

#include <boost/variant/static_visitor.hpp>

#include <common/Geometry.h>
#include <common/geometry/Circle.h>

#include <common/geometry/collision.h>

using namespace std;

namespace geometry {

bool collision(Circle const &c1, Circle const &c2) {
	return distance(c1.center, c2.center) <= (c1.radius + c2.radius);
}

MinMax minmaxProjection(Polygon const &polygon, Vector axis) {
	MinMax minmax = {numeric_limits<Scalar>::max(), numeric_limits<Scalar>::lowest()};
	for(Point const & point : polygon) {
		auto d = (point - Point{0,0}) * axis;
		minmax.min = min(minmax.min, d);
		minmax.max = max(minmax.max, d);
	}
	return minmax;
}


bool gapAlongAxis(Vector axis, Polygon const &p1, Polygon const &p2) {
	MinMax r1 = minmaxProjection(p1, axis);
	MinMax r2 = minmaxProjection(p2, axis);

	return (r1.max < r2.min) || (r2.max < r1.min);
}

void getNormals(std::set<Vector> &normals, Polygon const &polygon) {
	if (polygon.empty())
		return;

	Point prev = polygon.back();
	for(auto const & curr : polygon) {
		auto v1 = unit(curr - prev);
		Vector v2 = {-v1.y, v1.x};
		//if (v2.x < 0)
		//	v2 = -v2;

		//normals.insert(rightNormal(unit(curr - prev)));
		normals.insert(leftNormal(unit(curr - prev)));
		prev = curr;
	}
}

bool collision(Polygon const &p1, Polygon const &p2) {
	std::set<Vector> normals;
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

namespace {

template < typename Object1 >
class CollisionVisitor1 : public boost::static_visitor<bool>{
	Object1 const & object1;
public:
	explicit CollisionVisitor1(Object1 const & object1) :
		object1(object1) {
	}

	template < typename Object2 >
	bool operator()(Object2 const & object2) {
		return collision(object1, object2);
	}
};

}

bool collision(Polygon const &polygon, Object2D const &object) {
	CollisionVisitor1<Polygon> visitor{polygon};
	return boost::apply_visitor(visitor, object);
}

bool collision(Circle const &circle, Object2D const &object) {
	CollisionVisitor1<Circle> visitor{circle};
	return boost::apply_visitor(visitor, object);
}

namespace {
class CollisionVisitor2 : public boost::static_visitor<bool>{
	Object2D const & object1;
public:
	explicit CollisionVisitor2(Object2D const & object1) :
		object1(object1) {
	}

	template < typename Object2 >
	bool operator()(Object2 const & object2) {
		CollisionVisitor1<Object2> visitor{object2};
		return boost::apply_visitor(visitor, object1);
	}
};
}

bool collision(Object2D const &object1, Object2D const &object2) {
	CollisionVisitor2 visitor{object1};
	return boost::apply_visitor(visitor, object2);
}

} // namespace geometry
