#include <cmath>
#include <numeric> // accumulate
#include <ostream>

#include "Geometry.h"

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

bool in(Point const &point, PolygonalShape const &shape,
        RectangularArea const &area) {
  // TODO use accumulate with ranges
  for (auto const &v : shape) {
    Point shape_point = point + v;
    if (!in(shape_point, area))
      return false;
  }

  return true;
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
