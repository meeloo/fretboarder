//
//  Fretboard.cpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 07/04/2020.
//  Copyright © 2020 Autodesk. All rights reserved.
//

#include "Fretboard.hpp"

namespace fretboarder {
std::vector<Preset> Preset::_presets;

void to_json(json& j, const Instrument& i) {
    j = json{
        { "number_of_strings", i.number_of_strings },
        { "scale_length", i.scale_length },
        { "perpendicular_fret_index", i.perpendicular_fret_index },
        { "inter_string_spacing_at_nut", i.inter_string_spacing_at_nut },
        { "inter_string_spacing_at_bridge", i.inter_string_spacing_at_bridge },
        { "has_zero_fret", i.has_zero_fret },
        { "nut_to_zero_fret_offset", i.nut_to_zero_fret_offset },
        { "number_of_frets", i.number_of_frets },
        { "overhang", i.overhang },
        { "hidden_tang_length", i.hidden_tang_length },
        { "draw_strings", i.draw_strings },
        { "draw_frets", i.draw_frets },
        { "fret_slots_width", i.fret_slots_width },
        { "fret_slots_height", i.fret_slots_height },
        { "fret_crown_width", i.fret_crown_width },
        { "fret_crown_height", i.fret_crown_height },
        { "last_fret_cut_offset", i.last_fret_cut_offset },
        { "carve_nut_slot", i.carve_nut_slot },
        { "space_before_nut", i.space_before_nut },
        { "nut_thickness", i.nut_thickness },
        { "nut_height_under", i.nut_height_under },
        { "radius_at_nut", i.radius_at_nut },
        { "radius_at_last_fret", i.radius_at_last_fret },
        { "fretboard_thickness", i.fretboard_thickness }
    };
}

void from_json(const json& j, Instrument& i) {
    j.at("number_of_strings").get_to(i.number_of_strings);
    j.at("scale_length").get_to(i.scale_length);
    j.at("perpendicular_fret_index").get_to(i.perpendicular_fret_index);
    j.at("inter_string_spacing_at_nut").get_to(i.inter_string_spacing_at_nut);
    j.at("inter_string_spacing_at_bridge").get_to(i.inter_string_spacing_at_bridge);
    j.at("has_zero_fret").get_to(i.has_zero_fret);
    j.at("nut_to_zero_fret_offset").get_to(i.nut_to_zero_fret_offset);
    j.at("number_of_frets").get_to(i.number_of_frets);
    j.at("overhang").get_to(i.overhang);
    j.at("hidden_tang_length").get_to(i.hidden_tang_length);
    j.at("draw_strings").get_to(i.draw_strings);
    j.at("draw_frets").get_to(i.draw_frets);
    j.at("fret_slots_width").get_to(i.fret_slots_width);
    j.at("fret_slots_height").get_to(i.fret_slots_height);
    j.at("fret_crown_width").get_to(i.fret_crown_width);
    j.at("fret_crown_height").get_to(i.fret_crown_height);
    j.at("last_fret_cut_offset").get_to(i.last_fret_cut_offset);
    j.at("carve_nut_slot").get_to(i.carve_nut_slot);
    j.at("space_before_nut").get_to(i.space_before_nut);
    j.at("nut_thickness").get_to(i.nut_thickness);
    j.at("nut_height_under").get_to(i.nut_height_under);
    j.at("radius_at_nut").get_to(i.radius_at_nut);
    j.at("radius_at_last_fret").get_to(i.radius_at_last_fret);
    j.at("fretboard_thickness").get_to(i.fretboard_thickness);
    j.at("number_of_strings").get_to(i.number_of_strings);
    
    i.validate();
    
}


}
