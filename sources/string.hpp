//
//  string.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 07/04/2020.
//  Copyright © 2020 Autodesk. All rights reserved.
//

#ifndef string_hpp
#define string_hpp

#include <stdio.h>

namespace fretboarder {
    class string {
    public:
        int index;
        double scale_length;
        double perpendicular_fret_index;
        double y_at_start;
        double x_at_bridge;
        double y_at_bridge;
        double number_of_frets_per_octave;
        double nut_to_zero_fret_offset;
        double x_at_start;
        double x_at_nut;
        line

        string(int index,
               double scale_length,
               double perpendicular_fret_index,
               double y_at_start,
               double y_at_bridge,
               bool has_zero_fret,
               double nut_to_zero_fret_offset,
               double number_of_frets_per_octave=12)
        {
            self.index = index
            self.scale_length = Decimal(scale_length)
            self.perpendicular_fret_index = perpendicular_fret_index
            self.x_at_start = None
            self.y_at_start = Decimal(y_at_start)
            self.x_at_bridge = None
            self.y_at_bridge = Decimal(y_at_bridge)
            self.number_of_frets_per_octave = number_of_frets_per_octave

            if has_zero_fret:
                self.nut_to_zero_fret_offset = nut_to_zero_fret_offset
            else:
                self.nut_to_zero_fret_offset = 0

            # init StraightLine
            perpendicular_fret_from_start = self.distance_from_start(
                self.perpendicular_fret_index
            )

            y_intersect = (
                self.y_at_start
                + perpendicular_fret_from_start
                * (self.y_at_bridge - self.y_at_start)
                / self.scale_length
            )

            self.x_at_start = -(
                pow(perpendicular_fret_from_start, 2) - pow(y_intersect - self.y_at_start, 2)
            ).sqrt()
            self.x_at_nut = self.x_at_start - self.nut_to_zero_fret_offset

            x_at_bridge = pow(self.scale_length, 2) - pow(
                self.y_at_bridge - self.y_at_start, 2
            )
            x_at_bridge = x_at_bridge.sqrt()
            x_at_bridge -= abs(self.x_at_start)
            self.x_at_bridge = x_at_bridge

            self.line = StraightLine.from_points(
                Point(self.x_at_start, self.y_at_start), Point(x_at_bridge, self.y_at_bridge)
            )
        }

    def distance_from_start(self, fret_index):
        return self.scale_length - self.distance_from_bridge(fret_index)

    def distance_from_bridge(self, fret_index):
        return self.scale_length / pow(
            2, fret_index / Decimal(self.number_of_frets_per_octave)
        )

    def distance_between_frets(self, fret_index1, fret_index2):
        return abs(
            self.distance_from_start(fret_index1) - self.distance_from_start(fret_index2)
        )

    def distance_from_perpendicular_fret(self, fret_index):
        return self.distance_between_frets(self.perpendicular_fret_index, fret_index)

    def point_at_fret(self, fret_index):
        distance_from_start = self.distance_from_start(fret_index)
        t = distance_from_start / self.scale_length
        x = t * (self.x_at_bridge - self.x_at_start)
        y = t * (self.y_at_bridge - self.y_at_start)
        point = Point(self.x_at_start + x, self.y_at_start + y)
        assert point in self.line
        return point

    def point_at_nut(self):
        return Point(self.x_at_start, self.y_at_start)

    def point_at_bridge(self):
        return Point(self.x_at_bridge, self.y_at_bridge)
}


#endif /* string_hpp */
