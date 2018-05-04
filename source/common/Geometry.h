#pragma once

#include <iostream>

#include <common/geometry/Angle.h>
#include <common/geometry/Circle.h>
#include <common/geometry/CircleShape.h>
#include <common/geometry/Object2D.h>
#include <common/geometry/Point.h>
#include <common/geometry/Polygon.h>
#include <common/geometry/PolygonalShape.h>
#include <common/geometry/RectangularArea.h>
#include <common/geometry/Vector.h>

namespace geometry {

Scalar distance(Point const &p1, Point const &p2);

Vector unit(Vector const &v);

Scalar cos(Vector const &a, Vector const &b);

// Size of vector
Scalar size(Vector const &vec);

// Dot product
Scalar dot(Vector const &v1, Vector const &v2);

inline Scalar operator*(Vector const &v1, Vector const &v2) {
  return dot(v1, v2);
}

/**
 * @brief projects v to the direction of s
 */
Vector projection(Vector const &v, Vector const &s);

// @returns 't' for projected point, which is 'A + t*direction'
Scalar projection(Point const &p, Point const &A, Point const &B);

// TODO: implement convex shapes etc.

inline Vector leftNormal(Vector const &v) { return {v.y, -v.x}; }

inline Vector rightNormal(Vector const &v) { return {-v.y, v.x}; }

inline Vector &operator+=(Vector &v1, Vector const &v2) {
  v1.x += v2.x;
  v1.y += v2.y;
  return v1;
}

inline Vector &operator*=(Vector &vector, Scalar scalar) {
  vector.x *= scalar;
  vector.y *= scalar;
  return vector;
}

inline Vector operator*(Scalar scalar, Vector const &vector) {
  return vector * scalar;
}

inline Vector operator/(Vector const &vector, Scalar scalar) {
  return vector * (1.0 / scalar);
}

inline Point &operator+=(Point &p, Vector v) {
  p.x += v.x;
  p.y += v.y;
  return p;
}

inline Point &operator-=(Point &p, Vector v) {
  p.x -= v.x;
  p.y -= v.y;
  return p;
}

inline Point operator+(Point const &p, Vector const &v) {
  Point q = p;
  q += v;
  return q;
}

inline Vector operator+(Vector const &v) { return {+v.x, +v.y}; }

inline Vector operator-(Vector const &v) { return {-v.x, -v.y}; }

inline Vector &operator-=(Vector &v1, Vector const &v2) {
  v1.x -= v2.x;
  v1.y -= v2.y;
  return v1;
}

inline Vector operator-(Point const &p1, Point const &p2) {
  return Vector{p1.x - p2.x, p1.y - p2.y};
}

PolygonalShape &operator+=(PolygonalShape &shape, Vector const &vec);
PolygonalShape &operator*=(PolygonalShape &shape, Scalar scale);

std::ostream &operator<<(std::ostream &o, Point const &point);
std::ostream &operator<<(std::ostream &o, Vector const &point);
std::ostream &operator<<(std::ostream &o, CircleShape const &shape);
std::ostream &operator<<(std::ostream &o, Circle const &c);
std::ostream &operator<<(std::ostream &o, PolygonalShape const &shape);
std::ostream &operator<<(std::ostream &o, Polygon const &p);
std::ostream &operator<<(std::ostream &o, RectangularArea const &area);

bool in(Point const &point, RectangularArea const &area);
bool in(Circle const &circle, RectangularArea const &area);
bool in(Point const &point, CircleShape const &shape,
        RectangularArea const &area);
bool in(Polygon const &polygon, RectangularArea const &area);

bool in(Object2D const &object1, Object2D const &object2);
bool in(Object2D const &object, RectangularArea const &area);

Vector rotate(Vector const &vec, Angle angle);
Point rotate_around(Point const &center, Point const &point, Angle angle);

} // namespace geometry
