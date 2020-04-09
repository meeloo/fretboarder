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
    int number_of_strings = 6;

    double scale_length[2] = { 63.50, 64.77 };
    double perpendicular_fret_index = 0;

    double string_spacing_at_nut = 4.30;
    double string_spacing_at_bridge = 7.00;

    double y_at_start = string_spacing_at_nut / 2;
    double y_at_bridge = string_spacing_at_bridge / 2;

    bool has_zero_fret = true;
    double nut_to_zero_fret_offset = 0.30;

    double number_of_frets_per_octave = 12;
    int number_of_frets = 24;
    double overhang = 0.3;

    double hidden_tang_length = 0.2;
    double fret_slots_width = 0.06;
    double fret_slots_height = 0.15;

    double last_fret_cut_offset = 0.0;

    bool carve_nut_slot = true;
    double space_before_nut = 1.2;
    double nut_thickness = 0.4;
    double nut_height_under = 0.3;

    double radius_at_nut = 25.40; // 10"
    double radius_at_last_fret = 50.8; // 20"
    
    double fretboard_thickness = 0.7;
    
    void scale(double K) {
        scale_length[0] *= K;
        scale_length[1] *= K;

        string_spacing_at_nut *= K;
        string_spacing_at_bridge *= K;

        y_at_start *= K;
        y_at_bridge *= K;

        nut_to_zero_fret_offset *= K;

        overhang *= K;

        hidden_tang_length *= K;
        fret_slots_width *= K;
        fret_slots_height *= K;

        last_fret_cut_offset *= K;

        space_before_nut *= K;
        nut_thickness *= K;
        nut_height_under *= K;

        radius_at_nut *= K;
        radius_at_last_fret *= K;
        
        fretboard_thickness *= K;

        validate();
    }
    void validate() {
        y_at_start = string_spacing_at_nut / 2;
        y_at_bridge = string_spacing_at_bridge / 2;
        if (!has_zero_fret) {
            nut_to_zero_fret_offset = 0;
        }
    }
};

struct Quad {
    Point points[4];
};

class Fretboard {
private:
    std::vector<String> _strings;
    int number_of_frets;
    bool has_zero_fret;
    double nut_to_zero_fret_offset;

    Line first_border;
    Line last_border;

    Line first_tang_border;
    Line last_tang_border;

    std::vector<Vector> _fret_slots;
    std::vector<Quad> _fret_slot_shapes;
    
    int first_fret;
    int last_fret;

    double _construction_distance_at_nut_side;
    double _construction_distance_at_heel;
    double _construction_distance_at_nut;
    double _construction_distance_at_last_fret;
    double _construction_distance_at_12th_fret;
    
    Quad _board_shape;
    Quad _nut_shape;
    Quad _nut_slot_shape;
    Quad _strings_shape;
    
public:
    Fretboard(const Instrument& instrument) {
        _strings.push_back(String(0,
                                 instrument.scale_length[0],
                                 instrument.perpendicular_fret_index,
                                 instrument.y_at_start,
                                 instrument.y_at_bridge,
                                 instrument.has_zero_fret,
                                 instrument.nut_to_zero_fret_offset,
                                 instrument.number_of_frets_per_octave));

        _strings.push_back(String(instrument.number_of_strings - 1,
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

        first_border = _strings.at(0).line().offset(instrument.overhang);
        last_border = _strings.at(1).line().offset(-instrument.overhang);

        first_tang_border = first_border.offset(-instrument.hidden_tang_length);
        last_tang_border = last_border.offset(instrument.hidden_tang_length);
        
        // Make frets slots:
        first_fret = 1;
        if (has_zero_fret) {
            first_fret = 0;
        }
        
        last_fret = instrument.number_of_frets + 1;
        for (int fret_index = first_fret; fret_index < last_fret; fret_index++) {
            Line fret_line = Line(_strings.at(0).point_at_fret(fret_index), _strings.at(1).point_at_fret(fret_index));
            _fret_slots.push_back(Vector(fret_line.intersection(first_tang_border), fret_line.intersection(last_tang_border)));
        }

        for (int fret_index = first_fret; fret_index < last_fret; fret_index++) {
            auto point1 = _strings.at(0).point_at_fret(fret_index);
            auto point2 = _strings.at(1).point_at_fret(fret_index);
            auto fret_line = Line(point1, point2);
            double sign = point1.x <= point2.x ? 1 : -1;
            Line offset_line1 = fret_line.offset(sign * -instrument.fret_slots_width / 2);
            Line offset_line2 = fret_line.offset(sign * instrument.fret_slots_width / 2);

            _fret_slot_shapes.push_back(
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
        auto last_fret_line = Line(_strings.at(0).point_at_fret(last_fret), _strings.at(1).point_at_fret(last_fret));
        double sign = (_strings.at(0).x_at_bridge() <= _strings.at(1).x_at_bridge()) ? 1 : -1;
        Line last_fret_cut = last_fret_line.offset(sign * instrument.last_fret_cut_offset);

        double nut_sign = _strings.at(0).x_at_nut() > _strings.at(1).x_at_nut() ? 1 : -1;
        Line nut_line = Line(Point(_strings.at(0).x_at_nut(), _strings.at(0).y_at_start()), Point(_strings.at(1).x_at_nut(), _strings.at(1).y_at_start()));
        
        // board shape
        Line board_cut_at_nut = nut_line.offset(nut_sign * instrument.space_before_nut);
        _board_shape = {
            board_cut_at_nut.intersection(first_border),
            last_fret_cut.intersection(first_border),
            last_fret_cut.intersection(last_border),
            board_cut_at_nut.intersection(last_border)
        };

        _construction_distance_at_nut_side = std::min(_board_shape.points[0].x, _board_shape.points[3].x);
        _construction_distance_at_heel = std::max(_board_shape.points[1].x, _board_shape.points[2].x);
        _construction_distance_at_nut = (_strings.at(0).x_at_nut() + _strings.at(1).x_at_nut()) / 2;
        _construction_distance_at_last_fret = (_strings.at(0).point_at_fret(number_of_frets).x + _strings.at(1).point_at_fret(number_of_frets).x) / 2;

        if (number_of_frets > 12) {
            _construction_distance_at_12th_fret = (_strings.at(0).point_at_fret(12).x + _strings.at(1).point_at_fret(12).x) / 2;
        }

        // nut shape
        auto nut_line_2 = nut_line.offset(nut_sign * instrument.nut_thickness);
        _nut_shape = {
            nut_line_2.intersection(first_border),
            nut_line.intersection(first_border),
            nut_line.intersection(last_border),
            nut_line_2.intersection(last_border)
        };

        auto external_line_1 = first_border.offset(5);
        auto external_line_2 = last_border.offset(-5);
        _nut_slot_shape = {
            nut_line_2.intersection(external_line_1),
            nut_line.intersection(external_line_1),
            nut_line.intersection(external_line_2),
            nut_line_2.intersection(external_line_2)
        };

        _strings_shape = {
            _strings.at(0).point_at_nut(),
            _strings.at(0).point_at_bridge(),
            _strings.at(1).point_at_bridge(),
            _strings.at(1).point_at_nut(),
        };
    }
    
    const std::vector<Vector>& fret_slots() const { return _fret_slots; }
    const std::vector<Quad>& fret_slot_shapes() const { return _fret_slot_shapes; }
    const std::vector<String>& strings() const { return _strings; }

    double construction_distance_at_nut_side() { return _construction_distance_at_nut_side; }
    double construction_distance_at_heel() { return _construction_distance_at_heel; }
    double construction_distance_at_nut() { return _construction_distance_at_nut; }
    double construction_distance_at_last_fret() { return _construction_distance_at_last_fret; }
    double construction_distance_at_12th_fret() { return _construction_distance_at_12th_fret; }

    const Quad& board_shape() { return _board_shape; }
    const Quad& nut_shape() { return _nut_shape; }
    const Quad& nut_slot_shape() { return _nut_slot_shape; }
    const Quad& strings_shape() { return _strings_shape; }

};

}
#endif /* fretboard_hpp */
