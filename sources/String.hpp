//
//  String.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 07/04/2020.
//  Copyright © 2020 Autodesk. All rights reserved.
//

#ifndef string_hpp
#define string_hpp

#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <math.h>

namespace fretboarder {

class String {
public:
    int index;
    double scale_length;
    double perpendicular_fret_index;
    double perpendicular_fret_from_start;
    double y_at_start;
    double x_at_bridge;
    double y_at_bridge;
    double number_of_frets_per_octave;
    double nut_to_zero_fret_offset;
    double x_at_start;
    double x_at_nut;
    adsk::core::Ptr<adsk::core::Point3D> startPoint;
    adsk::core::Ptr<adsk::core::Point3D> endPoint;

    String(int index,
           double scale_length,
           double perpendicular_fret_index,
           double y_at_start,
           double y_at_bridge,
           bool has_zero_fret,
           double nut_to_zero_fret_offset,
           double number_of_frets_per_octave=12)
    {
        this->index = index;
        this->scale_length = scale_length;
        this->perpendicular_fret_index = perpendicular_fret_index;
        this->x_at_start = 0;
        this->y_at_start = y_at_start;
        this->x_at_bridge = 0;
        this->y_at_bridge = y_at_bridge;
        this->number_of_frets_per_octave = number_of_frets_per_octave;
        
        if (has_zero_fret) {
            this->nut_to_zero_fret_offset = nut_to_zero_fret_offset;
        }
        else {
            this->nut_to_zero_fret_offset = 0;
        }
                
// init StraightLine
        perpendicular_fret_from_start = distance_from_start(perpendicular_fret_index);
        
        double y_intersect = (
                       y_at_start
                       + perpendicular_fret_from_start
                       * (y_at_bridge - y_at_start)
                       / scale_length
                       );
        
        this->x_at_start = -sqrt( pow(perpendicular_fret_from_start, 2) - pow(y_intersect - y_at_start, 2) );
        this->x_at_nut = x_at_start - nut_to_zero_fret_offset;
        
        double x_at_bridge = pow(scale_length, 2) - pow(y_at_bridge - y_at_start, 2);
        x_at_bridge = sqrt(x_at_bridge);
        x_at_bridge -= abs(x_at_start);
        this->x_at_bridge = x_at_bridge;
        
        startPoint = adsk::core::Point3D::create(x_at_start, y_at_start);
        endPoint = adsk::core::Point3D::create(x_at_bridge, y_at_bridge);
    }
    
    double distance_from_start(int fret_index) {
        return scale_length - distance_from_bridge(fret_index);
    }
    
    double distance_from_bridge(int fret_index) {
        return scale_length / pow(2, fret_index / number_of_frets_per_octave);
    }
    
    double distance_between_frets(int fret_index1, int fret_index2) {
        return abs(distance_from_start(fret_index1) - distance_from_start(fret_index2));
    }
    
    double distance_from_perpendicular_fret(int fret_index) {
        return distance_between_frets(perpendicular_fret_index, fret_index);
    }
    
    adsk::core::Ptr<adsk::core::Point3D> point_at_fret(int fret_index) {
        double d = distance_from_start(fret_index);
        double t = d / scale_length;
        double x = t * (x_at_bridge - x_at_start);
        double y = t * (y_at_bridge - y_at_start);
        return adsk::core::Point3D::create(x_at_start + x, y_at_start + y);
    }
    
    adsk::core::Ptr<adsk::core::Point3D> point_at_nut() {
        return adsk::core::Point3D::create(x_at_start, y_at_start);
    }
    
    adsk::core::Ptr<adsk::core::Point3D> point_at_bridge() {
        return adsk::core::Point3D::create(x_at_bridge, y_at_bridge);
    }
};

} // namespace
    
#endif /* string_hpp */
