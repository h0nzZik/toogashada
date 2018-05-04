#include <cassert>
#include <cmath>
#include <numeric> // accumulate
#include <ostream>
#include <utility>

#include <boost/variant/static_visitor.hpp>

#include "Geometry.h"

using namespace std;

namespace geometry {

Scalar size(Vector const &vec) { return std::hypot(vec.x, vec.y); }

// Dot product
Scalar dot(Vector const &v1, Vector const &v2) {
  return v1.x * v2.x + v1.y * v2.y;
}

Scalar cos(Vector const &a, Vector const &b) {
  return dot(a, b) / (size(a) * size(b));
}

Vector projection(Vector const &v, Vector const &s) {
  return ((v * s) / (s * s)) * s;
}

Scalar projection(Point const &p, Point const &A, Point const &B) {
  // http://stackoverflow.com/a/15187473/6209703
  Vector const direction = B - A;
  Scalar const t = ((p.x - A.x) * (B.x - A.x) + (p.y - A.y) * (B.y - A.y)) /
                   (direction.x * direction.x + direction.y * direction.y);
  // t=[(Cx-Ax)(Bx-Ax)+(Cy-Ay)(By-Ay)]/[(Bx-Ax)^2+(By-Ay)^2]
  return t;
}

Vector unit(Vector const &v) { return v / size(v); }

Scalar distance(Point const &p1, Point const &p2) {
  auto d = p2 - p1;
  return Scalar(std::hypot(d.x, d.y));
}

std::ostream &operator<<(std::ostream &o, Point const &point) {
  o << "[" << point.x << "," << point.y << "]";
  return o;
}

std::ostream &operator<<(std::ostream &o, Vector const &point) {
  o << "(" << point.x << "," << point.y << ")";
  return o;
}

std::ostream &operator<<(std::ostream &o, CircleShape const &shape) {
  o << "{ radius: " << shape.radius << " }";
  return o;
}

std::ostream &operator<<(std::ostream &o, Circle const &c) {
  o << "{ circ at " << c.center << ", r: " << c.radius << " }";
  return o;
}

PolygonalShape &operator+=(PolygonalShape &shape, Vector const &vec) {
  for (auto &v : shape)
    v += vec;
  return shape;
}

PolygonalShape &operator*=(PolygonalShape &shape, Scalar scale) {
  for (auto &v : shape)
    v *= scale;
  return shape;
}

PolygonalShape &operator-=(PolygonalShape &shape, Vector const &vec) {
  for (auto &v : shape)
    v -= vec;
  return shape;
}

std::ostream &operator<<(std::ostream &o, PolygonalShape const &shape) {
  o << "[";
  for (auto const &vec : shape)
    o << " " << vec;
  o << " ]";
  return o;
}

std::ostream &operator<<(std::ostream &o, Polygon const &p) {
  o << "[";
  for (auto const &pt : p)
    o << " " << pt;
  o << " ]";
  return o;
}

std::ostream &operator<<(std::ostream &o, RectangularArea const &area) {
  o << "{ area from " << area.topLeft() << " to " << area.bottomRight() << " }";
  return o;
}

bool in_triangle(Point const &p, Point const &p1, Point const &p2,
                 Point const &p3) {
  auto alpha = ((p2.y - p3.y) * (p.x - p3.x) + (p3.x - p2.x) * (p.y - p3.y)) /
               ((p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y));
  auto beta = ((p3.y - p1.y) * (p.x - p3.x) + (p1.x - p3.x) * (p.y - p3.y)) /
              ((p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y));
  auto gamma = 1.0f - alpha - beta;

  return alpha >= 0 && beta >= 0 && gamma >= 0;
}

Vector perpendicularDistance(Point const &p, Point const &A, Point const &B) {
  Scalar const t = projection(p, A, B);
  Point const projectedCenter = A + t * (B - A);
  return p - projectedCenter;
}

bool onLeftOf(Point const &point, Point const &A, Point const &B) {
  return ((A.x - point.x) * (B.y - point.y) -
          (A.y - point.y) * (B.y - point.x)) > 0;
}

// http://stackoverflow.com/a/21096006/6209703
bool in_polygon(Point const &point, Polygon const &polygon_old) {
  Polygon polygon = polygon_old;

  assert(polygon.size() >= 3);

  while (polygon.size() > 3) {
    // TODO detect automatically polygons rotation/order of verteces
    Polygon newPolygon;
    if (onLeftOf(point, polygon[0], polygon[polygon.size() / 2])) {
      for (size_t i = 0; i <= polygon.size() / 2; i++)
        newPolygon.push_back(polygon[i]);
    } else {
      for (size_t i = polygon.size() / 2; i < polygon.size(); i++)
        newPolygon.push_back(polygon[i]);
      newPolygon.push_back(polygon[0]);
    }

    assert(newPolygon.size() < polygon.size());
    using std::swap;
    swap(newPolygon, polygon);
  }

  return in_triangle(point, polygon[0], polygon[1], polygon[2]);
}

bool in_polygon(Circle const &circle, Polygon const &polygon) {
  for (Point const &p : polygon) {
    Vector const direction = unit(p - circle.center);
    Point const outer = circle.center + circle.radius * direction;
    if (!in_polygon(outer, polygon))
      return false;
  }
  return true;
}

bool in_polygon(Polygon const &polygon1, Polygon const &polygon2) {
  for (Point const &p : polygon1) {
    if (!in_polygon(p, polygon2))
      return false;
  }
  return true;
}

bool in_circle(Point const &point, Circle const &circle) {
  auto vec = point - circle.center;
  return std::hypot(vec.x, vec.y) <= circle.radius;
}

bool in_circle(Circle const & /*circle1*/, Circle const & /*circle2*/) {
  return false;
}

bool in_circle(Polygon const &polygon, Circle const &circle) {
  for (Point const &p : polygon) {
    if (!in_circle(p, circle))
      return false;
  }
  return true;
}

bool in(Point const &point, RectangularArea const &area) {
  if (point.x <= area.topLeft().x)
    return false;
  if (point.x >= area.bottomRight().x)
    return false;
  if (point.y <= area.topLeft().y)
    return false;
  if (point.y >= area.bottomRight().y)
    return false;
  return true;
}

bool in(Circle const &circle, RectangularArea const &area) {
  return in(circle.center, CircleShape{circle.radius}, area);
}

bool in(Point const &point, CircleShape const &shape,
        RectangularArea const &area) {
  if (point.x - shape.radius <= area.topLeft().x)
    return false;
  if (point.x + shape.radius >= area.bottomRight().x)
    return false;
  if (point.y - shape.radius <= area.topLeft().y)
    return false;
  if (point.y + shape.radius >= area.bottomRight().y)
    return false;
  return true;
}

bool in(Polygon const &polygon, RectangularArea const &area) {
  // TODO use accumulate with ranges
  for (auto const &p : polygon) {
    if (!in(p, area))
      return false;
  }

  return true;
}

namespace {

template <typename Object1>
class InVisitor1 : public boost::static_visitor<bool> {
  Object1 const &object1;

public:
  explicit InVisitor1(Object1 const &object1) : object1(object1) {}

  bool operator()(Polygon const &object2) {
    return in_polygon(object1, object2);
  }

  bool operator()(Circle const &object2) { return in_circle(object1, object2); }
};

class InVisitor2 : public boost::static_visitor<bool> {
  Object2D const &object2;

public:
  explicit InVisitor2(Object2D const &object2) : object2(object2) {}

  bool operator()(Polygon const &object1) {
    InVisitor1<Polygon> visitor{object1};
    return boost::apply_visitor(visitor, object2);
  }

  bool operator()(Circle const &object1) {
    InVisitor1<Circle> visitor{object1};
    return boost::apply_visitor(visitor, object2);
  }
};

} // anonymous namespace

bool in(Object2D const &object1, Object2D const &object2) {
  InVisitor2 visitor{object2};
  return boost::apply_visitor(visitor, object1);
}

bool in(Object2D const &object, RectangularArea const &area) {
  class Visitor : public boost::static_visitor<bool> {
  private:
    RectangularArea const &area;

  public:
    Visitor(RectangularArea const &area) : area(area) {}

    bool operator()(Polygon const &polygon) { return in(polygon, area); }

    bool operator()(Circle const &circle) { return in(circle, area); }
  };

  Visitor visitor{area};
  return boost::apply_visitor(visitor, object);
}

Scalar deg2rad(Scalar degrees) { return degrees * 3.141592653589793 / 180.0; }

Vector rotate(Vector const &vec, Angle angle) {
  auto const dangle = deg2rad(angle);
  auto const cs = Scalar(std::cos(dangle));
  auto const sn = Scalar(std::sin(dangle));

  return vec.x * Vector{cs, sn} + vec.y * Vector{Scalar(-sn), cs};
}

Point rotate_around(Point const &center, Point const &point, Angle angle) {
  return center + rotate(point - center, angle);
}

} // namespace geometry
