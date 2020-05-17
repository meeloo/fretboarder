//
//  String.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 07/04/2020.
//  Copyright © 2020 Autodesk. All rights reserved.
//

#ifndef string_hpp
#define string_hpp

#include <math.h>
#include "Geometry.hpp"

namespace fretboarder {

class String {
private:
    int _index;
    double _scale_length;
    double _perpendicular_fret_index;
    double _perpendicular_fret_from_start;
    double _x_at_start;
    double _x_at_nut;
    double _y_at_start;
    double _x_at_bridge;
    double _y_at_bridge;
    double _number_of_frets_per_octave;
    double _nut_to_zero_fret_offset;

public:
    String(int index,
           double scale_length,
           double perpendicular_fret_index,
           double y_at_start,
           double y_at_bridge,
           bool has_zero_fret,
           double nut_to_zero_fret_offset,
           double number_of_frets_per_octave=12)
    {
        _index = index;
        _scale_length = scale_length;
        _perpendicular_fret_index = perpendicular_fret_index;
        _x_at_start = 0;
        _y_at_start = y_at_start;
        _x_at_bridge = 0;
        _y_at_bridge = y_at_bridge;
        _number_of_frets_per_octave = number_of_frets_per_octave;
        
        if (has_zero_fret) {
            _nut_to_zero_fret_offset = nut_to_zero_fret_offset;
        }
        else {
            _nut_to_zero_fret_offset = 0;
        }
                
        // init StraightLine
        _perpendicular_fret_from_start = distance_from_start(perpendicular_fret_index);
        
        double y_intersect = (
                       y_at_start
                       + _perpendicular_fret_from_start
                       * (y_at_bridge - y_at_start)
                       / scale_length
                       );
        
        double sign = _perpendicular_fret_from_start < 0 ? -1 : 1;
        _x_at_start = -sign * sqrt( pow(_perpendicular_fret_from_start, 2) - pow(y_intersect - y_at_start, 2) );
        _x_at_nut = _x_at_start - nut_to_zero_fret_offset;
        
        double xab = pow(scale_length, 2) - pow(y_at_bridge - y_at_start, 2);
        xab = sqrt(xab);
        xab -= abs(_x_at_start);
        _x_at_bridge = xab;
    }
    
    double distance_from_start(double fret_index) const {
        return _scale_length - distance_from_bridge(fret_index);
    }
    
    double distance_from_bridge(double fret_index) const {
        if (fret_index == 100) {
            return 0;
        }

        double l = _scale_length;
        double i = fret_index;
        if (i < 0) {
            // We need to change the scale and recompute the index relative to the new scale
            int s = (1 + int(-i / 12)) ;
            l = l * (1 << s);
            i = 12 * s + i;
        }
        return l / pow(2, i / _number_of_frets_per_octave);
    }
    
//    double distance_between_frets(double fret_index1, double fret_index2) const {
//        return abs(distance_from_start(fret_index1) - distance_from_start(fret_index2));
//    }
//
//    double distance_from_perpendicular_fret(double fret_index) const {
//        return distance_between_frets(_perpendicular_fret_index, fret_index);
//    }
//
    Point point_at_fret(double fret_index) const {
        double d = distance_from_start(fret_index);
        double t = d / _scale_length;
        double x = t * (_x_at_bridge - _x_at_start);
        double y = t * (_y_at_bridge - _y_at_start);
        return Point(_x_at_start + x, _y_at_start + y);
    }
    
    Point point_at_nut() const {
        return Point(_x_at_start, _y_at_start);
    }
    
    Point point_at_bridge() const {
        return Point(_x_at_bridge, _y_at_bridge);
    }
    
    Line line() const {
        return Line(point_at_bridge(), point_at_nut());
    }


    // Accessors:
    int index() const { return _index; }
    double scale_length() const { return _scale_length; }
    double perpendicular_fret_index() const { return _perpendicular_fret_index; }
    double perpendicular_fret_from_start() const { return _perpendicular_fret_from_start; }
    double x_at_start() const { return _x_at_start; }
    double x_at_nut() const { return _x_at_nut; }
    double y_at_start() const { return _y_at_start; }
    double x_at_bridge() const { return _x_at_bridge; }
    double y_at_bridge() const { return _y_at_bridge; }
    double number_of_frets_per_octave() const { return _number_of_frets_per_octave; }
    double nut_to_zero_fret_offset() const { return _nut_to_zero_fret_offset; }

};

} // namespace
    
#endif /* string_hpp */
