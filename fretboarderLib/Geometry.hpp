//
//  Geometry.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 07/04/2020.
//  Copyright © 2020 Autodesk. All rights reserved.
//

#ifndef Geometry_hpp
#define Geometry_hpp

#include <math.h>
#include <cassert>
#include <algorithm>

namespace fretboarder {

class Vector;

class Point {
public:
    double x;
    double y;
    double z;

    Point(double x = 0, double y = 0, double z = 0) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
    Point(const Point& p) {
        x = p.x;
        y = p.y;
        z = p.z;
    }
    
    double distanceFrom(const Point& p) const {
        double X = x - p.x;
        double Y = y - p.y;
        double Z = z - p.z;
        return sqrt(X * X + Y * Y + Z * Z);
    }
    
    Point operator+(const Point& p) const {
        return Point(x + p.x, y + p.y, z + p.z);
    }

    Point operator-(const Point& p) const {
        return Point(x - p.x, y - p.y, z - p.z);
    }

    Point operator*(const Point& p) const;

    Point operator*(double v) const {
        return Point(x * v, y * v, z * v);
    }
    

    bool operator==(const Point& p) const {
        return x == p.x && y == p.y && z == p.z;
    }
};

class Vector {
public:
    Point point1;
    Point point2;
    
    Vector(const Point& p1 = Point(), const Point& p2 = Point()) {
        point1 = p1;
        point2 = p2;
    }
    
    Vector(const Vector& v) {
        point1 = v.point1;
        point2 = v.point2;
    }
    
    bool intersection(const Vector& other, Point& result) const;
    Point intersection(const Vector& other) const;

    Vector offset2D(double offset) const;
    Vector offset2D(double offset0, const Vector& vector0, double offset1, const Vector& vector1) const;
    
    Vector unitVector() const;
    Vector sizedVector(double size) const;
};


}

#endif /* Geometry_hpp */
