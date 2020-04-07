//
//  Fretboard.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 07/04/2020.
//  Copyright © 2020 Autodesk. All rights reserved.
//

#ifndef fretboard_hpp
#define fretboard_hpp

#include <vector>
#include "String.hpp"
#include "Geometry.hpp"

namespace fretboarder {

//# X origin is where we have the first perpendicular fret of the fretboard
//# on a standard guitar it's the nut, because all frets are perpendicular to the center line of the fretboard.
//# on a fanned fret guitar, it could be 0, but it could be 5, 7, 12 or anything else.
//# The neck goes along the X axis, +X points toward the body of the instrument.
//# -X points toward the headstock.
//# +Y points to the first string of the instrument (for a right handed guitar it would be high E)
//# -Y points to the last string of the instrument (for a right handed guitar it would be low E)


struct Instrument {
    int number_of_strings;
    double scale_length[2];
    double perpendicular_fret_index;
    double string_spacing_at_nut;
    double string_spacing_at_bridge;
    double y_at_start = string_spacing_at_nut / 2;
    double y_at_bridge = string_spacing_at_bridge / 2;
    bool has_zero_fret;
    double nut_to_zero_fret_offset;
    double number_of_frets_per_octave = 12;
    int number_of_frets;
    double overhang;
    double hidden_tang_length;
    double fret_slots_width;
    double last_fret_cut_offset;
    double space_before_nut;
    double nut_thickness;
};

struct Quad {
    Point points[4];
};

class Fretboard {
private:
    std::vector<String> strings;
    int number_of_frets;
    bool has_zero_fret;
    double nut_to_zero_fret_offset;

    Line first_border;
    Line last_border;

    Line first_tang_border;
    Line last_tang_border;

    std::vector<Vector> fret_slots;
    std::vector<Quad> fret_slot_shapes;
    
    int first_fret;
    int last_fret;

    double construction_distance_at_nut_side;
    double construction_distance_at_heel;
    double construction_distance_at_nut;
    double construction_distance_at_last_fret;
    double construction_distance_at_12th_fret;
    
    Quad board_shape;
    Quad nut_shape;
    Quad nut_slot_shape;
    Quad strings_shape;
    
public:
    Fretboard(const Instrument& instrument) {
        strings.push_back(String(0,
                                 instrument.scale_length[0],
                                 instrument.perpendicular_fret_index,
                                 instrument.y_at_start,
                                 instrument.y_at_bridge,
                                 instrument.has_zero_fret,
                                 instrument.nut_to_zero_fret_offset,
                                 instrument.number_of_frets_per_octave));

        strings.push_back(String(instrument.number_of_strings - 1,
                                 instrument.scale_length[1],
                                 instrument.perpendicular_fret_index,
                                 -instrument.y_at_start,
                                 -instrument.y_at_bridge,
                                 instrument.has_zero_fret,
                                 instrument.nut_to_zero_fret_offset,
                                 instrument.number_of_frets_per_octave));
        
        number_of_frets = instrument.number_of_frets;
        has_zero_fret = instrument.has_zero_fret;
        nut_to_zero_fret_offset = instrument.nut_to_zero_fret_offset;

        first_border = strings[0].line().offset(instrument.overhang);
        last_border = strings[1].line().offset(-instrument.overhang);

        first_tang_border = first_border.offset(-instrument.hidden_tang_length);
        last_tang_border = last_border.offset(instrument.hidden_tang_length);
        
        // Make frets slots:
        first_fret = 1;
        if (has_zero_fret) {
            first_fret = 0;
        }
        
        last_fret = instrument.number_of_frets + 1;
        for (int fret_index = first_fret; fret_index < last_fret; fret_index++) {
            Line fret_line = Line(strings[0].point_at_fret(fret_index), strings[1].point_at_fret(fret_index));
            fret_slots.push_back(Vector(fret_line.intersection(first_tang_border), fret_line.intersection(last_tang_border)));
        }

        for (int fret_index = first_fret; fret_index < last_fret; fret_index++) {
            auto point1 = strings[0].point_at_fret(fret_index);
            auto point2 = strings[1].point_at_fret(fret_index);
            auto fret_line = Line(point1, point2);
            double sign = point1.x <= point2.x ? 1 : -1;
            Line offset_line1 = fret_line.offset(sign * -instrument.fret_slots_width / 2);
            Line offset_line2 = fret_line.offset(sign * instrument.fret_slots_width / 2);

            fret_slot_shapes.push_back(
                {
                    offset_line1.intersection(first_tang_border),
                    offset_line2.intersection(first_tang_border),
                    offset_line2.intersection(last_tang_border),
                    offset_line1.intersection(last_tang_border)
                }
            );
        }
        
        // last fret cut is like a last+1th fret. If you have 22 frets, the cut is at a virtual 23th fret.
        // you can use last_fret_cut_offset to add an X offset to the cut.
        auto last_fret_line = Line(strings[0].point_at_fret(last_fret), strings[1].point_at_fret(last_fret));
        double sign = (strings[0].x_at_bridge() <= strings[1].x_at_bridge()) ? 1 : -1;
        Line last_fret_cut = last_fret_line.offset(sign * instrument.last_fret_cut_offset);

        double nut_sign = strings[0].x_at_nut() > strings[1].x_at_nut() ? 1 : -1;
        Line nut_line = Line(Point(strings[0].x_at_nut(), strings[0].y_at_start()), Point(strings[1].x_at_nut(), strings[1].y_at_start()));
        
        // board shape
        Line board_cut_at_nut = nut_line.offset(nut_sign * instrument.space_before_nut);
        Quad board_shape = {
            board_cut_at_nut.intersection(first_border),
            last_fret_cut.intersection(first_border),
            last_fret_cut.intersection(last_border),
            board_cut_at_nut.intersection(last_border)
        };

        construction_distance_at_nut_side = std::min(board_shape.points[0].x, board_shape.points[3].x);
        construction_distance_at_heel = std::max(board_shape.points[1].x, board_shape.points[2].x);
        construction_distance_at_nut = (strings[0].x_at_nut() + strings[1].x_at_nut()) / 2;
        construction_distance_at_last_fret = (strings[0].point_at_fret(number_of_frets).x + strings[1].point_at_fret(number_of_frets).x) / 2;

        if (number_of_frets > 12) {
            construction_distance_at_12th_fret = (strings[0].point_at_fret(12).x + strings[1].point_at_fret(12).x) / 2;
        }

        // nut shape
        auto nut_line_2 = nut_line.offset(nut_sign * instrument.nut_thickness);
        nut_shape = {
            nut_line_2.intersection(first_border),
            nut_line.intersection(first_border),
            nut_line.intersection(last_border),
            nut_line_2.intersection(last_border)
        };

        auto external_line_1 = first_border.offset(5);
        auto external_line_2 = last_border.offset(-5);
        nut_slot_shape = {
            nut_line_2.intersection(external_line_1),
            nut_line.intersection(external_line_1),
            nut_line.intersection(external_line_2),
            nut_line_2.intersection(external_line_2)
        };

        strings_shape = {
            strings[0].point_at_nut(),
            strings[0].point_at_bridge(),
            strings[1].point_at_bridge(),
            strings[1].point_at_nut(),
        };
    }
};

}
#endif /* fretboard_hpp */
