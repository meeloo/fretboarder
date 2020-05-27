//
//  Geometry.cpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 07/04/2020.
//  Copyright © 2020 Autodesk. All rights reserved.
//

#include "Geometry.hpp"

using namespace fretboarder;

static inline double dot(const Point& u, const Point& v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

static inline double norm2( const Point& v )
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static inline double norm( const Point& v )
{
    return sqrt(norm2(v));
}

static inline Point cross( const Point& b, const Point& c) // cross product
{
    return Point(b.y * c.z - c.y * b.z,
                 b.z * c.x - c.z * b.x,
                 b.x *  c.y - c.x * b.y);
}

Point Point::operator*(const Point& p) const {
    return cross(*this, p);
}


static double _intersection(const Vector& a, const Vector& b, Point& ip)
// http://mathworld.wolfram.com/Line-LineIntersection.html
// in 3d; will also work in 2d if z components are 0
{
    Point da = a.point2 - a.point1;
    Point db = b.point2 - b.point1;
    Point dc = b.point1 - a.point1;
    
    if (dot(dc, cross(da,db)) != 0.0) // lines are not coplanar
        return false;
    
    double s = dot(cross(dc,db),cross(da,db)) / norm2(cross(da,db));
    ip = a.point1 + da * s;
    
    return true;
}

bool Vector::intersection(const Vector& other, Point& result) const {
    return _intersection(*this, other, result);
}

Point Vector::intersection(const Vector& other) const {
    Point result;
    bool res = _intersection(*this, other, result);
    assert(res);
    return result;
}

Vector Vector::offset2D(double offset) const {
    //        Returns a new line parallel to the current one.
    //        The orthogonal distance between the lines will be abs(offset).
    //        If offset > 0, the new line will be above, otherwise it will be below.

    //        The lines have the same slope, and we need to add offset * sqrt(1 + pow(slope, 2)) to the y_intersect.
    double x = point2.x - point1.x;
    double y = point2.y - point1.y;
    double X = y;
    double Y = -x;
    Point perp = Point(X, Y, 0);
    double n = norm(perp);
    
    assert(n != 0);
    
    double off = offset / n;
    perp.x *= off;
    perp.y *= off;
    
    return Vector(point1 + perp, point2 + perp);
}
