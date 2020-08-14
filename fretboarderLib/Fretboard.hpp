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
#include <string>
#include <algorithm>
#include "String.hpp"
#include "Geometry.hpp"

#include "json.hpp"

#include <fstream>

using nlohmann::json;

namespace fretboarder {

//# X origin is where we have the first perpendicular fret of the fretboard
//# on a standard guitar it's the nut, because all frets are perpendicular to the center line of the fretboard.
//# on a fanned fret guitar, it could be 0, but it could be 5, 7, 12 or anything else.
//# The neck goes along the X axis, +X points toward the body of the instrument.
//# -X points toward the headstock.
//# +Y points to the first string of the instrument (for a right handed guitar it would be high E)
//# -Y points to the last string of the instrument (for a right handed guitar it would be low E)

enum OverhangType {
    single = 0,
    nut_and_last_fret = 1,
    all = 2
};

struct Instrument {
    Instrument(bool right_handed = true,
    int number_of_strings = 6,
    double scale_length_bass = 64.77,
    double scale_length_treble = 63.50,
    double perpendicular_fret_index = 0,
    double inter_string_spacing_at_nut = 0.75,
    double inter_string_spacing_at_bridge = 1.2,
    bool has_zero_fret = true,
    double nut_to_zero_fret_offset = 0.30,
    int number_of_frets = 24,
               double overhang0 = 0.3,
               double overhang1 = 0.3,
               double overhang2 = 0.3,
               double overhang3 = 0.3,
    double hidden_tang_length = 0.2,
    bool draw_strings = true,
    bool draw_frets = true,
    double fret_slots_width = 0.06,
    double fret_slots_height = 0.15,
    double fret_crown_width = 0.234,
    double fret_crown_height = 0.122,
    double last_fret_cut_offset = 0.0,
    bool carve_nut_slot = true,
    double space_before_nut = 1.2,
    double nut_thickness = 0.4,
    double nut_height_under = 0.3,
    double radius_at_nut = 25.40, // 10"
    double radius_at_last_fret = 50.8, // 20"
    double fretboard_thickness = 0.7
               )
    {
        this->right_handed = right_handed;
        this->number_of_strings = number_of_strings;
        this->scale_length[1] = scale_length_bass;
        this->scale_length[0] = scale_length_treble;
        this->perpendicular_fret_index = perpendicular_fret_index;
        this->inter_string_spacing_at_nut = inter_string_spacing_at_nut;
        this->inter_string_spacing_at_bridge = inter_string_spacing_at_bridge;
        this->has_zero_fret = has_zero_fret;
        this->nut_to_zero_fret_offset = nut_to_zero_fret_offset;
        this->number_of_frets = number_of_frets;
        this->overhangs[0] = overhang0;
        this->overhangs[1] = overhang1;
        this->overhangs[2] = overhang2;
        this->overhangs[3] = overhang3;
        this->hidden_tang_length = hidden_tang_length;
        this->draw_strings = draw_strings;
        this->draw_frets = draw_frets;
        this->fret_slots_width = fret_slots_width;
        this->fret_slots_height = fret_slots_height;
        this->fret_crown_width = fret_crown_width;
        this->fret_crown_height = fret_crown_height;
        this->last_fret_cut_offset = last_fret_cut_offset;
        this->carve_nut_slot = carve_nut_slot;
        this->space_before_nut = space_before_nut;
        this->nut_thickness = nut_thickness;
        this->nut_height_under = nut_height_under;
        this->radius_at_nut = radius_at_nut;
        this->radius_at_last_fret = radius_at_last_fret;
        this->fretboard_thickness = fretboard_thickness;
        
        validate();
    }
    
    bool right_handed = true;
    int number_of_strings = 6;

    double scale_length[2] = { 63.50, 64.77 };
    double perpendicular_fret_index = 0;

    double inter_string_spacing_at_nut = 0.75;
    double inter_string_spacing_at_bridge = 1.2;

    double string_spacing_at_nut = 4.30;
    double string_spacing_at_bridge = 7.00;

    double y_at_start = string_spacing_at_nut / 2;
    double y_at_bridge = string_spacing_at_bridge / 2;

    bool has_zero_fret = true;
    double nut_to_zero_fret_offset = 0.30;

    double number_of_frets_per_octave = 12;
    int number_of_frets = 24;
    OverhangType overhang_type = single;
    double overhangs[4] = {0.3, 0.3, 0.3, 0.3};

    double hidden_tang_length = 0.2;
    bool draw_strings = true;
    bool draw_frets = true;
    double fret_slots_width = 0.06;
    double fret_slots_height = 0.15;
    double fret_crown_width = 0.234;
    double fret_crown_height = 0.122;

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

        inter_string_spacing_at_nut *= K;
        inter_string_spacing_at_bridge *= K;
        
//        string_spacing_at_nut *= K;
//        string_spacing_at_bridge *= K;
//
//        y_at_start *= K;
//        y_at_bridge *= K;

        nut_to_zero_fret_offset *= K;

        for (int i = 0; i < 4; i++)
            overhangs[i] *= K;

        hidden_tang_length *= K;
        fret_slots_width *= K;
        fret_slots_height *= K;
        fret_crown_width *= K;
        fret_crown_height *= K;

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
        string_spacing_at_nut = inter_string_spacing_at_nut * (std::max(2, number_of_strings) - 1);
        string_spacing_at_bridge = inter_string_spacing_at_bridge * (std::max(2, number_of_strings) - 1);

        y_at_start = string_spacing_at_nut / 2;
        y_at_bridge = string_spacing_at_bridge / 2;
        if (!has_zero_fret) {
            nut_to_zero_fret_offset = 0;
        }
    }
    
    bool load(const std::string& filename);
    bool save(const std::string& filename) const;
};

void to_json(json& j, const Instrument& i);
void from_json(const json& j, Instrument& i);


static double mmFromInch(double v) { return v * 25.4; }
static double cmFromInch(double v) { return mmFromInch(v) * 0.1; }

struct Preset {
public:
    std::string name;
    Instrument instrument;

    
    static std::vector<Preset> presets() {
        if (_presets.empty()) {

            bool rh = true;
            double bass = 64.77;
            double treble = 64.77;

            double spacing_at_nut = 0.72;
            double spacing_at_bridge = 1.1;
            
            double bass_spacing_at_nut = 1.2;
            double bass_spacing_at_bridge = 1.8;

            double nut_to_zero_fret = 0.30;

            int frets = 24;
            double overhang = 0.3;

            bool drawStrings = true;
            bool drawFrets = true;
            double hidden_tang = 0.2;
            double slots_width = 0.06;
            double slots_height = 0.15;
            double crown_width = 0.3;
            double crown_height = 0.3;

            double last_fret_offset = 0.0;

            bool nut_slot = true;
            double space_before_nut = 1.2;
            double nut_thickness = 0.4;
            double nut_height = 0.3;

            double thickness = 0.7;

            _presets.push_back({"Telecaster", Instrument(rh, 6, bass, treble, 0, spacing_at_nut, spacing_at_bridge, false, nut_to_zero_fret, 22, overhang, hidden_tang, drawStrings, drawFrets, slots_width, slots_height, crown_width, crown_height, last_fret_offset, nut_slot, space_before_nut, nut_thickness, nut_height, cmFromInch(9.5), cmFromInch(9.5), thickness)});
            _presets.push_back({"Stratocaster", Instrument(rh, 6, bass, treble, 0, spacing_at_nut, spacing_at_bridge, false, nut_to_zero_fret, 22, overhang, hidden_tang, drawStrings, drawFrets, slots_width, slots_height, crown_width, crown_height, last_fret_offset, nut_slot, space_before_nut, nut_thickness, nut_height, cmFromInch(10), cmFromInch(10), thickness)});
            _presets.push_back({"Les paul", Instrument(rh, 6, cmFromInch(24.7), cmFromInch(24.7), 0, spacing_at_nut, spacing_at_bridge, false, nut_to_zero_fret, 22, overhang, hidden_tang, drawStrings, drawFrets, slots_width, slots_height, crown_width, crown_height, last_fret_offset, nut_slot, 0, nut_thickness, thickness, cmFromInch(12), cmFromInch(12), thickness)});
            _presets.push_back({"Jazz bass", Instrument(rh, 4, cmFromInch(34), cmFromInch(34), 0, bass_spacing_at_nut, bass_spacing_at_bridge, false, nut_to_zero_fret, frets, overhang, hidden_tang, drawStrings, drawFrets, slots_width, slots_height, crown_width, crown_height, last_fret_offset, nut_slot, space_before_nut, nut_thickness, nut_height, cmFromInch(12), cmFromInch(12), thickness)});
            _presets.push_back({"Precision bass", Instrument(rh, 4, cmFromInch(34), cmFromInch(34), 0, bass_spacing_at_nut, bass_spacing_at_bridge, false, nut_to_zero_fret, frets, overhang, hidden_tang, drawStrings, drawFrets, slots_width, slots_height, crown_width, crown_height, last_fret_offset, nut_slot, space_before_nut, nut_thickness, nut_height, cmFromInch(12), cmFromInch(12), thickness)});
            _presets.push_back({"Boden 6", Instrument(rh, 6, cmFromInch(25.5), cmFromInch(25.0), 0, spacing_at_nut, spacing_at_bridge, true, nut_to_zero_fret, frets, overhang, hidden_tang, drawStrings, drawFrets, slots_width, slots_height, crown_width, crown_height, last_fret_offset, nut_slot, space_before_nut, nut_thickness, nut_height, cmFromInch(12), cmFromInch(20), thickness)});
            _presets.push_back({"Boden 7", Instrument(rh, 7, cmFromInch(25.5), cmFromInch(25.0),  0, spacing_at_nut, spacing_at_bridge, true, nut_to_zero_fret, frets, overhang, hidden_tang, drawStrings, drawFrets, slots_width, slots_height, crown_width, crown_height, last_fret_offset, nut_slot, space_before_nut, nut_thickness, nut_height, cmFromInch(12), cmFromInch(20), thickness)});
            _presets.push_back({"Boden bass", Instrument(rh, 4, cmFromInch(34), cmFromInch(32), /*perp_fret_index*/ 7, bass_spacing_at_nut, bass_spacing_at_bridge, true, nut_to_zero_fret, frets, overhang, hidden_tang, drawStrings, drawFrets, slots_width, slots_height, crown_width, crown_height, last_fret_offset, nut_slot, space_before_nut, nut_thickness, nut_height, cmFromInch(16), cmFromInch(20), thickness)});
            _presets.push_back({"Boden bass 5 strings", Instrument(rh, 5, cmFromInch(34), cmFromInch(32), /*perp_fret_index*/ 7, bass_spacing_at_nut, bass_spacing_at_bridge, true, nut_to_zero_fret, frets, overhang, hidden_tang, drawStrings, drawFrets, slots_width, slots_height, crown_width, crown_height, last_fret_offset, nut_slot, space_before_nut, nut_thickness, nut_height, cmFromInch(16), cmFromInch(20), thickness)});
        }
        
        return _presets;
    }

private:
    static std::vector<Preset> _presets;
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

    Vector first_border;
    Vector last_border;

    Vector first_tang_border;
    Vector last_tang_border;

    std::vector<Vector> _fret_slots;
    std::vector<Vector> _fret_lines;
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
        double first = instrument.scale_length[instrument.right_handed? 1 : 0];
        double last = instrument.scale_length[instrument.right_handed? 0 : 1];

        double max = instrument.number_of_strings - 1;
        double diff = (last - first);
        double ydiff_start = -instrument.y_at_start * 2;
        double ydiff_bridge = -instrument.y_at_bridge * 2;

        for (int i = 0; i < instrument.number_of_strings; i++) {
            double ratio;
            
            if (max == 0) {
                ratio = 0.5;
            } else {
                ratio = (double)i / max;
            }
            double length = first + ratio * diff;
            double ystart = instrument.y_at_start + ratio * ydiff_start;
            double ybridge = instrument.y_at_bridge + ratio * ydiff_bridge;
            _strings.push_back(fretboarder::String(i,
                                     length,
                                     instrument.perpendicular_fret_index,
                                     ystart,
                                     ybridge,
                                     instrument.has_zero_fret,
                                     instrument.nut_to_zero_fret_offset,
                                     instrument.number_of_frets_per_octave));
        }

        // Compute global X offset (average of the X offsets of each string)
        double offsetSum = 0;
        for (int i = 0; i < instrument.number_of_strings; i++) {
            offsetSum += _strings[i].x_at_start();
        }
        double offset = offsetSum / (double)instrument.number_of_strings;
        for (int i = 0; i < instrument.number_of_strings; i++) {
            _strings[i].set_x_offset(-offset);
        }


        number_of_frets = instrument.number_of_frets;
        has_zero_fret = instrument.has_zero_fret;
        nut_to_zero_fret_offset = instrument.nut_to_zero_fret_offset;

        String first_string(_strings.front());
        String last_string(_strings.back());
        
        if (instrument.number_of_strings == 1) {
            // Create false first and last string
            first_string = fretboarder::String(0,
                                               _strings[0].scale_length(),
                                               instrument.perpendicular_fret_index,
                                               instrument.overhangs[0],
                                               instrument.overhangs[1],
                                               instrument.has_zero_fret,
                                               instrument.nut_to_zero_fret_offset,
                                               instrument.number_of_frets_per_octave);

            last_string = fretboarder::String(0,
                                               _strings[0].scale_length(),
                                               instrument.perpendicular_fret_index,
                                               -instrument.overhangs[2],
                                               -instrument.overhangs[3],
                                               instrument.has_zero_fret,
                                               instrument.nut_to_zero_fret_offset,
                                               instrument.number_of_frets_per_octave);
        }

        {
            // Compute fretboard area by extending the strings with the overhangs at first and last frets
            Vector first_fret = Vector(first_string.point_at_fret(0), last_string.point_at_fret(0));
            Vector last_fret = Vector(first_string.point_at_fret(instrument.number_of_frets), last_string.point_at_fret(instrument.number_of_frets));

            first_border = first_string.line().offset2D(instrument.overhangs[0], first_fret, instrument.overhangs[1], last_fret);
            last_border = last_string.line().offset2D(-instrument.overhangs[2], first_fret, -instrument.overhangs[3], last_fret);
        }
        
        first_tang_border = first_border.offset2D(-instrument.hidden_tang_length);
        last_tang_border = last_border.offset2D(instrument.hidden_tang_length);
        
        // Make frets slots:
        first_fret = 1;
        if (has_zero_fret) {
            first_fret = 0;
        }
        
        last_fret = instrument.number_of_frets + 1;
        for (int fret_index = first_fret; fret_index < last_fret; fret_index++) {
            Vector fret_line = Vector(first_string.point_at_fret(fret_index), last_string.point_at_fret(fret_index));
            _fret_slots.push_back(Vector(fret_line.intersection(first_tang_border), fret_line.intersection(last_tang_border)));
            _fret_lines.push_back(Vector(fret_line.intersection(first_border), fret_line.intersection(last_border)));
        }

        for (int fret_index = first_fret; fret_index < last_fret; fret_index++) {
            auto point1 = first_string.point_at_fret(fret_index);
            auto point2 = last_string.point_at_fret(fret_index);
            auto fret_line = Vector(point1, point2);
            double sign = point1.x <= point2.x ? 1 : -1;
            Vector offset_line1 = fret_line.offset2D(sign * -instrument.fret_slots_width / 2);
            Vector offset_line2 = fret_line.offset2D(sign * instrument.fret_slots_width / 2);

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
        auto p0 = first_string.point_at_fret(last_fret);
        auto p1 = last_string.point_at_fret(last_fret);
        auto last_fret_line = Vector(p0, p1);
        double sign = (first_string.x_at_bridge() <= last_string.x_at_bridge()) ? 1 : -1;
        Vector last_fret_cut = last_fret_line.offset2D(sign * instrument.last_fret_cut_offset);

        Vector nut_line = Vector(Point(first_string.x_at_nut(), first_string.y_at_start()), Point(last_string.x_at_nut(), last_string.y_at_start()));
        
        // board shape
        Vector board_cut_at_nut = nut_line.offset2D(instrument.space_before_nut);
        _board_shape = {
            board_cut_at_nut.intersection(first_border),
            last_fret_cut.intersection(first_border),
            last_fret_cut.intersection(last_border),
            board_cut_at_nut.intersection(last_border)
        };

        _construction_distance_at_nut_side = std::min(_board_shape.points[0].x, _board_shape.points[3].x);
        _construction_distance_at_heel = std::max(_board_shape.points[1].x, _board_shape.points[2].x);
        _construction_distance_at_nut = (first_string.x_at_nut() + last_string.x_at_nut()) / 2;
        _construction_distance_at_last_fret = (first_string.point_at_fret(number_of_frets).x + last_string.point_at_fret(number_of_frets).x) / 2;

        if (number_of_frets > 12) {
            _construction_distance_at_12th_fret = (first_string.point_at_fret(12).x + last_string.point_at_fret(12).x) / 2;
        }

        // nut shape
        auto nut_line_2 = nut_line.offset2D(instrument.nut_thickness);
        _nut_shape = {
            nut_line_2.intersection(first_border),
            nut_line.intersection(first_border),
            nut_line.intersection(last_border),
            nut_line_2.intersection(last_border)
        };

        auto external_line_1 = first_border.offset2D(5);
        auto external_line_2 = last_border.offset2D(-5);
        _nut_slot_shape = {
            nut_line_2.intersection(external_line_1),
            nut_line.intersection(external_line_1),
            nut_line.intersection(external_line_2),
            nut_line_2.intersection(external_line_2)
        };

        _strings_shape = {
            first_string.point_at_nut(),
            first_string.point_at_bridge(),
            last_string.point_at_bridge(),
            last_string.point_at_nut(),
        };
        
    }
    
    const std::vector<Vector>& fret_slots() const { return _fret_slots; }
    const std::vector<Vector>& fret_lines() const { return _fret_lines; }
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
