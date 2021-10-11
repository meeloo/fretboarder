//
//  Instruments+Inputs.cpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#include "Fretboarder.h"
#include "Instruments+Inputs.hpp"

Instrument InstrumentFromInputs(const Ptr<CommandInputs>& inputs) {
    Ptr<BoolValueCommandInput> right_handed = inputs->itemById("right_handed");
    Ptr<IntegerSliderCommandInput> number_of_strings = inputs->itemById("number_of_strings");
    Ptr<FloatSpinnerCommandInput> scale_length_treble = inputs->itemById("scale_length_treble");
    Ptr<FloatSpinnerCommandInput> scale_length_bass = inputs->itemById("scale_length_bass");
    Ptr<FloatSpinnerCommandInput> perpendicular_fret_index = inputs->itemById("perpendicular_fret_index");
    Ptr<FloatSpinnerCommandInput> inter_string_spacing_at_nut = inputs->itemById("inter_string_spacing_at_nut");
    Ptr<FloatSpinnerCommandInput> inter_string_spacing_at_bridge = inputs->itemById("inter_string_spacing_at_bridge");
    Ptr<BoolValueCommandInput> has_zero_fret = inputs->itemById("has_zero_fret");
    Ptr<FloatSpinnerCommandInput> nut_to_zero_fret_offset = inputs->itemById("nut_to_zero_fret_offset");
    Ptr<FloatSpinnerCommandInput> space_before_nut = inputs->itemById("space_before_nut");
    Ptr<BoolValueCommandInput> carve_nut_slot = inputs->itemById("carve_nut_slot");
    Ptr<FloatSpinnerCommandInput> nut_thickness = inputs->itemById("nut_thickness");
    Ptr<FloatSpinnerCommandInput> nut_height_under = inputs->itemById("nut_height_under");
    Ptr<FloatSpinnerCommandInput> nut_width = inputs->itemById("nut_width");

    Ptr<FloatSpinnerCommandInput> radius_at_nut = inputs->itemById("radius_at_nut");
    Ptr<FloatSpinnerCommandInput> radius_at_last_fret = inputs->itemById("radius_at_last_fret");
    Ptr<FloatSpinnerCommandInput> fretboard_thickness = inputs->itemById("fretboard_thickness");
    Ptr<IntegerSliderCommandInput> number_of_frets = inputs->itemById("number_of_frets");
    Ptr<BoolValueCommandInput> draw_strings = inputs->itemById("draw_strings");
    Ptr<BoolValueCommandInput> draw_frets = inputs->itemById("draw_frets");
    Ptr<BoolValueCommandInput> carve_fret_slots = inputs->itemById("carve_fret_slots");
    Ptr<DropDownCommandInput> overhang_type = inputs->itemById("overhang_type");
    Ptr<FloatSpinnerCommandInput> overhangSingle = inputs->itemById("overhangSingle");
    Ptr<FloatSpinnerCommandInput> overhangNut = inputs->itemById("overhangNut");
    Ptr<FloatSpinnerCommandInput> overhangLast = inputs->itemById("overhangLast");
    Ptr<FloatSpinnerCommandInput> overhang0 = inputs->itemById("overhang0");
    Ptr<FloatSpinnerCommandInput> overhang1 = inputs->itemById("overhang1");
    Ptr<FloatSpinnerCommandInput> overhang2 = inputs->itemById("overhang2");
    Ptr<FloatSpinnerCommandInput> overhang3 = inputs->itemById("overhang3");
    Ptr<FloatSpinnerCommandInput> hidden_tang_length = inputs->itemById("hidden_tang_length");
    Ptr<FloatSpinnerCommandInput> fret_slots_width = inputs->itemById("fret_slots_width");
    Ptr<FloatSpinnerCommandInput> fret_slots_height = inputs->itemById("fret_slots_height");
    Ptr<FloatSpinnerCommandInput> fret_crown_width = inputs->itemById("fret_crown_width");
    Ptr<FloatSpinnerCommandInput> fret_crown_height = inputs->itemById("fret_crown_height");
    Ptr<FloatSpinnerCommandInput> last_fret_cut_offset = inputs->itemById("last_fret_cut_offset");

    fretboarder::Instrument instrument;

    CHECK(right_handed, instrument);
    CHECK(number_of_strings, instrument);
    CHECK(scale_length_treble, instrument);
    CHECK(scale_length_bass, instrument);
    CHECK(perpendicular_fret_index, instrument);
    CHECK(inter_string_spacing_at_nut, instrument);
    CHECK(inter_string_spacing_at_bridge, instrument);
    CHECK(has_zero_fret, instrument);
    CHECK(nut_to_zero_fret_offset, instrument);
    CHECK(space_before_nut, instrument);
    CHECK(carve_nut_slot, instrument);
    CHECK(nut_thickness, instrument);
    CHECK(nut_height_under, instrument);
    CHECK(nut_width, instrument);
    CHECK(radius_at_nut, instrument);
    CHECK(radius_at_last_fret, instrument);
    CHECK(fretboard_thickness, instrument);
    CHECK(number_of_frets, instrument);
    CHECK(draw_strings, instrument);
    CHECK(draw_frets, instrument);
    CHECK(carve_fret_slots, instrument);
    CHECK(overhang_type, instrument);
    CHECK(overhangSingle, instrument);
    CHECK(overhangNut, instrument);
    CHECK(overhangLast, instrument);
    CHECK(overhang0, instrument);
    CHECK(overhang1, instrument);
    CHECK(overhang2, instrument);
    CHECK(overhang3, instrument);
    CHECK(hidden_tang_length, instrument);
    CHECK(fret_slots_width, instrument);
    CHECK(fret_slots_height, instrument);
    CHECK(fret_crown_width, instrument);
    CHECK(fret_crown_height, instrument);
    CHECK(last_fret_cut_offset, instrument);

    instrument.right_handed = right_handed->value();
    instrument.number_of_strings = number_of_strings->valueOne();
    instrument.scale_length[0] = scale_length_bass->value();
    instrument.scale_length[1] = scale_length_treble->value();
    instrument.draw_strings = draw_strings->value();
    instrument.perpendicular_fret_index = perpendicular_fret_index->value();
    instrument.inter_string_spacing_at_nut = inter_string_spacing_at_nut->value();
    instrument.inter_string_spacing_at_bridge = inter_string_spacing_at_bridge->value();
    instrument.has_zero_fret = has_zero_fret->value();
    instrument.nut_to_zero_fret_offset = nut_to_zero_fret_offset->value();
    instrument.number_of_frets = number_of_frets->valueOne();
    instrument.draw_frets = draw_frets->value();
    instrument.carve_fret_slots = carve_fret_slots->value();
    OverhangType t = single;
    auto selected_item = overhang_type->selectedItem();
    if (selected_item != nullptr) {
        if (selected_item->name() == "single") {
            t = single;
            instrument.overhangs[0] =
            instrument.overhangs[1] =
            instrument.overhangs[2] =
            instrument.overhangs[3] = overhangSingle->value();
        } else if (selected_item->name() == "nut and last fret") {
            t = nut_and_last_fret;
            instrument.overhangs[0] =
            instrument.overhangs[2] = overhangNut->value();
            instrument.overhangs[1] =
            instrument.overhangs[3] = overhangLast->value();
        } else if (selected_item->name() == "all") {
            t = all;
            instrument.overhangs[0] = overhang0->value();
            instrument.overhangs[1] = overhang1->value();
            instrument.overhangs[2] = overhang2->value();
            instrument.overhangs[3] = overhang3->value();
        }
    }
    instrument.overhang_type = t;
    instrument.hidden_tang_length = hidden_tang_length->value();
    instrument.fret_slots_width = fret_slots_width->value();
    instrument.fret_slots_height = fret_slots_height->value();
    instrument.fret_crown_width = fret_crown_width->value();
    instrument.fret_crown_height = fret_crown_height->value();
    instrument.last_fret_cut_offset = last_fret_cut_offset->value();
    instrument.space_before_nut = space_before_nut->value();
    instrument.carve_nut_slot = carve_nut_slot->value();
    instrument.nut_thickness = nut_thickness->value();
    instrument.nut_height_under = nut_height_under->value();
    instrument.radius_at_nut = radius_at_nut->value();
    instrument.radius_at_last_fret = radius_at_last_fret->value();
    instrument.fretboard_thickness = fretboard_thickness->value();
    instrument.validate();
    instrument.scale(10); // cm to mm

    return instrument;
}

void InstrumentToInputs(const Ptr<CommandInputs>& inputs, const Instrument& i) {
    Instrument instrument = i;

    Ptr<BoolValueCommandInput> right_handed = inputs->itemById("right_handed");
    Ptr<IntegerSliderCommandInput> number_of_strings = inputs->itemById("number_of_strings");
    Ptr<FloatSpinnerCommandInput> scale_length_treble = inputs->itemById("scale_length_treble");
    Ptr<FloatSpinnerCommandInput> scale_length_bass = inputs->itemById("scale_length_bass");
    Ptr<FloatSpinnerCommandInput> perpendicular_fret_index = inputs->itemById("perpendicular_fret_index");
    Ptr<FloatSpinnerCommandInput> inter_string_spacing_at_nut = inputs->itemById("inter_string_spacing_at_nut");
    Ptr<FloatSpinnerCommandInput> inter_string_spacing_at_bridge = inputs->itemById("inter_string_spacing_at_bridge");
    Ptr<BoolValueCommandInput> has_zero_fret = inputs->itemById("has_zero_fret");
    Ptr<FloatSpinnerCommandInput> nut_to_zero_fret_offset = inputs->itemById("nut_to_zero_fret_offset");
    Ptr<FloatSpinnerCommandInput> space_before_nut = inputs->itemById("space_before_nut");
    Ptr<BoolValueCommandInput> carve_nut_slot = inputs->itemById("carve_nut_slot");
    Ptr<FloatSpinnerCommandInput> nut_thickness = inputs->itemById("nut_thickness");
    Ptr<FloatSpinnerCommandInput> nut_height_under = inputs->itemById("nut_height_under");
    Ptr<FloatSpinnerCommandInput> nut_width = inputs->itemById("nut_width");
    Ptr<FloatSpinnerCommandInput> radius_at_nut = inputs->itemById("radius_at_nut");
    Ptr<FloatSpinnerCommandInput> radius_at_last_fret = inputs->itemById("radius_at_last_fret");
    Ptr<FloatSpinnerCommandInput> fretboard_thickness = inputs->itemById("fretboard_thickness");
    Ptr<IntegerSliderCommandInput> number_of_frets = inputs->itemById("number_of_frets");
    Ptr<DropDownCommandInput> overhang_type = inputs->itemById("overhang_type");
    Ptr<FloatSpinnerCommandInput> overhangSingle = inputs->itemById("overhangSingle");
    Ptr<FloatSpinnerCommandInput> overhangNut = inputs->itemById("overhangNut");
    Ptr<FloatSpinnerCommandInput> overhangLast = inputs->itemById("overhangLast");
    Ptr<FloatSpinnerCommandInput> overhang0 = inputs->itemById("overhang0");
    Ptr<FloatSpinnerCommandInput> overhang1 = inputs->itemById("overhang1");
    Ptr<FloatSpinnerCommandInput> overhang2 = inputs->itemById("overhang2");
    Ptr<FloatSpinnerCommandInput> overhang3 = inputs->itemById("overhang3");
    Ptr<FloatSpinnerCommandInput> hidden_tang_length = inputs->itemById("hidden_tang_length");
    Ptr<FloatSpinnerCommandInput> fret_slots_width = inputs->itemById("fret_slots_width");
    Ptr<FloatSpinnerCommandInput> fret_slots_height = inputs->itemById("fret_slots_height");
    Ptr<FloatSpinnerCommandInput> fret_crown_width = inputs->itemById("fret_crown_width");
    Ptr<FloatSpinnerCommandInput> fret_crown_height = inputs->itemById("fret_crown_height");
    Ptr<FloatSpinnerCommandInput> last_fret_cut_offset = inputs->itemById("last_fret_cut_offset");

    CHECK2(right_handed);
    CHECK2(number_of_strings);
    CHECK2(scale_length_treble);
    CHECK2(scale_length_bass);
    CHECK2(perpendicular_fret_index);
    CHECK2(inter_string_spacing_at_nut);
    CHECK2(inter_string_spacing_at_bridge);
    CHECK2(has_zero_fret);
    CHECK2(nut_to_zero_fret_offset);
    CHECK2(space_before_nut);
    CHECK2(carve_nut_slot);
    CHECK2(nut_thickness);
    CHECK2(nut_height_under);
    CHECK2(nut_width);
    CHECK2(radius_at_nut);
    CHECK2(radius_at_last_fret);
    CHECK2(fretboard_thickness);
    CHECK2(number_of_frets);
    CHECK2(overhang_type);
    CHECK2(overhangSingle);
    CHECK2(overhangNut);
    CHECK2(overhangLast);
    CHECK2(overhang0);
    CHECK2(overhang1);
    CHECK2(overhang2);
    CHECK2(overhang3);
    CHECK2(hidden_tang_length);
    CHECK2(fret_slots_width);
    CHECK2(fret_slots_height);
    CHECK2(fret_crown_width);
    CHECK2(fret_crown_height);
    CHECK2(last_fret_cut_offset);

    right_handed->value(instrument.right_handed);
    number_of_strings->valueOne(instrument.number_of_strings);
    scale_length_treble->value(instrument.scale_length[1]);
    scale_length_bass->value(instrument.scale_length[0]);
    perpendicular_fret_index->value(instrument.perpendicular_fret_index);
    inter_string_spacing_at_nut->value(instrument.inter_string_spacing_at_nut);
    inter_string_spacing_at_bridge->value(instrument.inter_string_spacing_at_bridge);
    has_zero_fret->value(instrument.has_zero_fret);
    nut_to_zero_fret_offset->value(instrument.nut_to_zero_fret_offset);
    space_before_nut->value(instrument.space_before_nut);
    carve_nut_slot->value(instrument.carve_nut_slot);
    nut_thickness->value(instrument.nut_thickness);
    nut_height_under->value(instrument.nut_height_under);
    radius_at_nut->value(instrument.radius_at_nut);
    radius_at_last_fret->value(instrument.radius_at_last_fret);
    fretboard_thickness->value(instrument.fretboard_thickness);
    number_of_frets->valueOne(instrument.number_of_frets);
    OverhangType t = single;
    auto selected_item = overhang_type->selectedItem();
    if (selected_item != nullptr) {
        if (overhang_type->selectedItem()->name() == "single") {
            t = single;
            overhangSingle->isVisible(true);
            overhangNut->isVisible(false);
            overhangLast->isVisible(false);
            overhang0->isVisible(false);
            overhang1->isVisible(false);
            overhang2->isVisible(false);
            overhang3->isVisible(false);
        } else if (selected_item->name() == "nut and last fret") {
            overhangSingle->isVisible(false);
            overhangNut->isVisible(true);
            overhangLast->isVisible(true);
            overhang0->isVisible(false);
            overhang1->isVisible(false);
            overhang2->isVisible(false);
            overhang3->isVisible(false);
            t = nut_and_last_fret;
        } else if (selected_item->name() == "all") {
            overhangSingle->isVisible(false);
            overhangNut->isVisible(false);
            overhangLast->isVisible(false);
            overhang0->isVisible(true);
            overhang1->isVisible(true);
            overhang2->isVisible(true);
            overhang3->isVisible(true);
            t = all;
        }
    }
    instrument.overhang_type = t;
    overhangSingle->value(instrument.overhangs[0]);
    overhangNut->value(instrument.overhangs[0]);
    overhangLast->value(instrument.overhangs[1]);
    overhang0->value(instrument.overhangs[0]);
    overhang1->value(instrument.overhangs[1]);
    overhang2->value(instrument.overhangs[2]);
    overhang3->value(instrument.overhangs[3]);
    hidden_tang_length->value(instrument.hidden_tang_length);
    fret_slots_width->value(instrument.fret_slots_width);
    fret_slots_height->value(instrument.fret_slots_height);
    fret_crown_width->value(instrument.fret_crown_width);
    fret_crown_height->value(instrument.fret_crown_height);
    last_fret_cut_offset->value(instrument.last_fret_cut_offset);

    nut_width->value(std::max(number_of_strings->valueOne() - 1, 1) * inter_string_spacing_at_nut->value() + overhang0->value() + overhang2->value());
}


