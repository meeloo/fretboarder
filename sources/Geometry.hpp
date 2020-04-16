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


class Point {
public:
    double x;
    double y;
    
    Point(double x = 0, double y = 0) {
        this->x = x;
        this->y = y;
    }
    
    Point(const Point& p) {
        x = p.x;
        y = p.y;
    }
};

class Vector {
public:
    Point point1;
    Point point2;
    
    Vector(Point p1, Point p2) {
        point1 = p1;
        point2 = p2;
    }
    
    Vector(const Vector& v) {
        point1 = v.point1;
        point2 = v.point2;
    }
};

class Line {
//    Represents a 2d line defined by the equation y(x) = a.x + b
//    In this class, a is `slope` and b is `y_intersect`.

public:
    double slope;
    double y_intersect;
    bool is_vertical;
    double vertical_at_x;

    Line(double slope = 0, double y_intersect = 0) {
        this->slope = slope;
        this->y_intersect = y_intersect;
        this->is_vertical = false;
        this->vertical_at_x = NAN;
    }
    
    Line(Point point1, Point point2) {
        // vertical lines
        if (point1.x == point2.x) {
            slope = NAN;
            y_intersect = NAN;
            is_vertical = true;
            vertical_at_x = point1.x;
        }
        else {
            slope = (point2.y - point1.y) / (point2.x - point1.x);
            y_intersect = point1.y - slope * point1.x;
            is_vertical = false;
            vertical_at_x = NAN;
        }
    }
    
    Line(const Line& l) {
        slope = l.slope;
        y_intersect = l.y_intersect;
        is_vertical = l.is_vertical;
        vertical_at_x = l.vertical_at_x;
    }
    
    Point intersection(const Line& other_line) const {
        //        Returns the point where the two straight lines intersect.
        double x = 0, y = 0;
        
        if (is_vertical || other_line.is_vertical) {
            if (is_vertical && other_line.is_vertical) {
                assert(0); // raise RuntimeError("Two vertical lines can't intersect.")
            }
            if (other_line.is_vertical) {
                return other_line.intersection(*this);
            }
            // x is known because the line is vertical, we just have to find y.
            x = vertical_at_x;
            y = other_line.get_y_by_x(x);
        }
        else {
            if (slope == other_line.slope) {
                assert(0); // raise RuntimeError("Two parallel lines can't intersect.")
            }

            //            y = a1.x + b1
            //            y = a2.x + b2
            //
            //            a1.x + b1 = a2.x + b2
            //            a1.x - a2.x = b2 - b1
            //            x.(a1 - a2) = b2 - b1
            //            x = (b2 - b1) / (a1 - a2)

            x = (other_line.y_intersect - y_intersect) / (slope - other_line.slope);
            y = get_y_by_x(x);
        }
        return Point(x, y);
    }
    
    Line offset(double offset) const {
        //        Returns a new line parallel to the current one.
        //        The orthogonal distance between the lines will be abs(offset).
        //        If offset > 0, the new line will be above, otherwise it will be below.
        
        //        The lines have the same slope, and we need to add offset * sqrt(1 + pow(slope, 2)) to the y_intersect.

        if (is_vertical) {
            auto line = Line(NAN, NAN);
            line.is_vertical = true;
            line.vertical_at_x = vertical_at_x + offset;
            return line;
        }
        else {
            return Line(slope, y_intersect + offset * sqrt(1 + pow(slope, 2)));
        }
    }
    
    double get_y_by_x(double x) const {
        return slope * x + y_intersect;
    }

    double get_x_by_y(double y) const {
        return (y - y_intersect) / slope;
    }

//     equation(self):
//        return "y(x) = {}.x + {}".format(self.slope, self.y_intersect)

    bool contains(Point point) const {
        // Return True if the given point is on the line, False otherwise.
        // We use a 10E-6 mm approximation, that is 10E-9 m (1 nanometer)

        return (point.y - get_y_by_x(point.x)) < 10E-6;
    }
};

#endif /* Geometry_hpp */
