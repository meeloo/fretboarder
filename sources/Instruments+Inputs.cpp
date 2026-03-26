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

// ---------------------------------------------------------------------------
// BuildFretboardDialogInputs
// Creates all tab groups and command inputs for the fretboard dialog.
// Extracted so it can be shared between the create and edit commands.
// ---------------------------------------------------------------------------
void BuildFretboardDialogInputs(const Ptr<CommandInputs>& inputs) {
    CHECK2(inputs);

    auto group = inputs->addTabCommandInput("strings", "Strings")->children();
    CHECK2(group);

    auto presetCombo = group->addDropDownCommandInput("presets", "Presets", TextListDropDownStyle);
    CHECK2(presetCombo);
    auto presets = presetCombo->listItems();
    CHECK2(presets);
    auto allPresets = Preset::presets();
    for (size_t i = 0; i < allPresets.size(); i++) {
        presets->add(allPresets[i].name, false);
    }

    group->addBoolValueInput("Load", "Load preset", false);
    group->addBoolValueInput("Save", "Save preset", false);

    auto rightHanded = group->addBoolValueInput("right_handed", "Right handed", true, "", true);
    rightHanded->tooltip("Switch in between right and left handed string layout.");
    rightHanded->tooltipDescription("The left handed version just mirrors the layout of the strings.");

    auto number_of_strings_slider = group->addIntegerSliderCommandInput("number_of_strings", "Count", 1, 15);
    number_of_strings_slider->tooltip("Select the number of strings for your instrument.");
    number_of_strings_slider->tooltipDescription("Minimum is 1 string, maximum 15.");
    CHECK2(number_of_strings_slider);
    number_of_strings_slider->valueOne(6);

    auto inter_string_spacing_at_nut = group->addFloatSpinnerCommandInput("inter_string_spacing_at_nut", "Spacing at nut", "mm", 0.1, 40, 0.1, 7.5);
    inter_string_spacing_at_nut->tooltip("This is the distance in between each string on the nut (or the zero string if you have one)");

    auto inter_string_spacing_at_bridge = group->addFloatSpinnerCommandInput("inter_string_spacing_at_bridge", "Spacing at bridge", "mm", 0.1, 40, 0.1, 12.0);
    inter_string_spacing_at_bridge->tooltip("This is the distance in between each string on the bridge.");
    inter_string_spacing_at_bridge->tooltipDescription("This may be dictated by the design of your bridge.");

    auto overhang_type = group->addDropDownCommandInput("overhang_type", "Overhang type", TextListDropDownStyle);
    auto overhang_items = overhang_type->listItems();
    overhang_items->add("single", true);
    overhang_items->add("nut and last fret", false);
    overhang_items->add("all", false);

    auto overhangSingle = group->addFloatSpinnerCommandInput("overhangSingle", "Fret overhang", "mm", 0, 50, 0.1, 3.0);
    overhangSingle->tooltip("This is the distance in between the outer strings and the border of the fretboard");

    auto overhangNut = group->addFloatSpinnerCommandInput("overhangNut", "Fret overhang at the nut", "mm", 0, 50, 0.1, 3.0);
    overhangNut->tooltip("This is the distance in between the outer string and the border of the fretboard at the nut");
    overhangNut->isVisible(false);

    auto overhangLast = group->addFloatSpinnerCommandInput("overhangLast", "Fret overhang at last fret", "mm", 0, 50, 0.1, 3.0);
    overhangLast->tooltip("This is the distance in between the outer strings and the border of the fretboard at the last fret");
    overhangLast->isVisible(false);

    auto overhang0 = group->addFloatSpinnerCommandInput("overhang0", "Fret overhang at nut (bass)", "mm", 0, 50, 0.1, 3.0);
    overhang0->tooltip("This is the distance in between the first string and the border of the fretboard at the nut");
    overhang0->isVisible(false);

    auto overhang1 = group->addFloatSpinnerCommandInput("overhang1", "Fret overhang at last fret (bass)", "mm", 0, 50, 0.1, 3.0);
    overhang1->tooltip("This is the distance in between the first string and the border of the fretboard at the last fret");
    overhang1->isVisible(false);

    auto overhang2 = group->addFloatSpinnerCommandInput("overhang2", "Fret overhang at nut (treble)", "mm", 0, 50, 0.1, 3.0);
    overhang2->tooltip("This is the distance in between the last string and the border of the fretboard at the nut");
    overhang2->isVisible(false);

    auto overhang3 = group->addFloatSpinnerCommandInput("overhang3", "Fret overhang at last fret (treble)", "mm", 0, 50, 0.1, 3.0);
    overhang3->tooltip("This is the distance in between the last string and the border of the fretboard at last fret");
    overhang3->isVisible(false);

    auto draw_strings = group->addBoolValueInput("draw_strings", "Draw strings", true, "", true);
    draw_strings->tooltip("Enabling this will create a sketch for the position of the strings following the ideal radius of the fretboard and the bridge.");

    group = inputs->addTabCommandInput("scale_length", "Scale length")->children();
    auto scale_length_bass = group->addFloatSpinnerCommandInput("scale_length_bass", "Bass side", "in", 1, 10000, 0.1, 25.5);
    scale_length_bass->tooltip("Enter the scale length of the bass string");
    scale_length_bass->tooltipDescription("This is independent of the handedness of the instrument you are building. Use the 'right handed' switch above to control that instead.");

    auto scale_length_treble = group->addFloatSpinnerCommandInput("scale_length_treble", "Treble side", "in", 1, 10000, 0.1, 25.0);
    scale_length_treble->tooltip("Enter the scale length of the treble string");
    scale_length_treble->tooltipDescription("This is independent of the handedness of the instrument you are building. Use the 'right handed' switch above to control that instead.");

    group = inputs->addTabCommandInput("fretboard_radius", "Fretboard")->children();
    auto radius_at_nut = group->addFloatSpinnerCommandInput("radius_at_nut", "Radius at nut", "in", 0, 10000, 0.1, 9.5);
    radius_at_nut->tooltip("Enter the desired fretboard radius at the nut.");
    radius_at_nut->tooltipDescription("You can choose a different radius at the nut and at the last fret to create a compound radius.");

    auto radius_at_last_fret = group->addFloatSpinnerCommandInput("radius_at_last_fret", "Radius at last fret", "in", 0, 10000, 0.1, 20.0);
    radius_at_last_fret->tooltip("Enter the desired fretboard radius at the last fret.");
    radius_at_last_fret->tooltipDescription("You can choose a different radius at the nut and at the last fret to create a compound radius.");

    auto fretboard_thickness = group->addFloatSpinnerCommandInput("fretboard_thickness", "Thickness", "mm", 0, 100, 0.1, 7.0);
    fretboard_thickness->tooltip("This is the thickness of your fretboard.");
    fretboard_thickness->tooltipDescription("This has to be less than thickness of the stock plank you will use.");

    group = inputs->addTabCommandInput("frets", "Frets")->children();
    auto number_of_frets_slider = group->addIntegerSliderCommandInput("number_of_frets", "Number of frets", 0, 60);
    number_of_frets_slider->tooltip("This is the number of frets not counting the zero fret.");
    CHECK2(number_of_frets_slider);
    number_of_frets_slider->valueOne(24);

    auto perpendicular_fret_index = group->addFloatSpinnerCommandInput("perpendicular_fret_index", "Perpendicular Fret", "", -36, 100, 0.1, 0.0);
    perpendicular_fret_index->tooltip("This is the position of the perpendicular fret for multiscale instruments. Use 100 to have the bridge be perpendicular to the strings (i.e. not slanted)");
    perpendicular_fret_index->tooltipDescription("This number is a floating point representing the position of the fretboard. If you choose, say, 7.5, the perpandicular position will be calculated as if it was right in between the 7th and the 8th fret. You can also use a negative number to completely offset the slanting of the string.");

    auto has_zero_fret = group->addBoolValueInput("has_zero_fret", "Zero fret", true, "", true);
    has_zero_fret->tooltip("Enable or disable the use of a zero-fret.");
    has_zero_fret->tooltipDescription("Seriously, why would you disable the zero fret? Are you nuts?");

    auto nut_to_zero_fret_offset = group->addFloatSpinnerCommandInput("nut_to_zero_fret_offset", "Distance from nut to zero fret", "mm", 0, 200, 0.1, 3.0);
    nut_to_zero_fret_offset->tooltip("This is the distance in between the zero fret's slot and the nut.");
    nut_to_zero_fret_offset->tooltipDescription("Is it ignored if you don't have a zero fret or if you choose not to carve the nut.");

    auto draw_frets = group->addBoolValueInput("draw_frets", "Draw frets", true, "", true);
    draw_frets->tooltip("Enabling this will enable the creation of bodies that represent the frets.");
    draw_frets->tooltipDescription("This is purely for esthetics, if you want to create renders for a client with a fretboard that looks finished.");

    auto carve_fret_slots = group->addBoolValueInput("carve_fret_slots", "Carve fret slots", true, "", true);
    carve_fret_slots->tooltip("Enabling this will enable the carving of fret slots in the fretboard.");

    auto hidden_tang_length = group->addFloatSpinnerCommandInput("hidden_tang_length", "Blind tang length", "mm", 0, 50, 0.1, 2.0);
    hidden_tang_length->tooltip("This is the distance in between the tang of the frets and the border of the fretboard plank");
    hidden_tang_length->tooltipDescription("If you want to have the fret tangs appearing on the border of the fretboard, use 0mm. Any other number will create blind/hidden frets tangs.");

    auto fret_slots_width = group->addFloatSpinnerCommandInput("fret_slots_width", "Fret slots width", "mm", 0, 2, 0.1, 0.6);
    fret_slots_width->tooltip("This is the kerf of the tool you will use to cut the fret slots and the thickness of the fret's tang. 0.5 to 0.6mm is generally a good range.");
    fret_slots_width->tooltipDescription("This is also used to model the fret tang bodies if you choose to draw the frets");

    auto fret_slots_height = group->addFloatSpinnerCommandInput("fret_slots_height", "Fret slots height", "mm", 0, 10, 0.1, 1.5);
    fret_slots_height->tooltip("This is the depth of the fret slots.");
    fret_slots_height->tooltipDescription("This should be greater of equal than the height of the tang of the frets you are going to use.");

    auto fret_crown_width = group->addFloatSpinnerCommandInput("fret_crown_width", "Fret crown width", "mm", 0, 10, 0.1, 2.34);
    fret_crown_width->tooltip("This is use to model the crown of the frets should you choose to draw them.");
    fret_crown_width->tooltipDescription("This is purely esthetic.");

    auto fret_crown_height = group->addFloatSpinnerCommandInput("fret_crown_height", "Fret crown height", "mm", 0, 10, 0.1, 1.22);
    fret_crown_height->tooltip("This is use to model the crown of the frets should you choose to draw them.");
    fret_crown_height->tooltipDescription("This is purely esthetic.");

    auto last_fret_cut_offset = group->addFloatSpinnerCommandInput("last_fret_cut_offset", "Last fret cut offset", "mm", 0, 10, 0.1, 0);
    last_fret_cut_offset->tooltip("This is an offset that is added after the last fret to choose the length of the fretboard.");
    last_fret_cut_offset->tooltipDescription("Using 0mm here will have your fretboard stopping one half fret after the last fret.");

    group = inputs->addTabCommandInput("nut", "Nut")->children();
    auto space_before_nut = group->addFloatSpinnerCommandInput("space_before_nut", "Space before nut", "mm", 0, 100, 0.1, 7.0);
    space_before_nut->tooltip("This is the amount of fretboard you need before in between it's start and the nut.");
    space_before_nut->tooltipDescription("This is probably 0 if you are using a Gibson type nut. Fender nuts typically need 3 to 5 mm here.");

    auto nut_thickness = group->addFloatSpinnerCommandInput("nut_thickness", "Thickness", "mm", 0, 100, 0.1, 4.0);
    nut_thickness->tooltip("This is the thickness/width of the nut (as seen from above).");

    auto carve_nut_slot = group->addBoolValueInput("carve_nut_slot", "Carve the nut slot", true, "", true);
    carve_nut_slot->tooltip("You can disable carving the nut.");
    carve_nut_slot->tooltipDescription("Some headless instruments use the their mini head pieces as a nut/string guide.");

    auto nut_height_under = group->addFloatSpinnerCommandInput("nut_height_under", "Slot depth", "mm", 0, 100, 0.1, 3.0);
    nut_height_under->tooltip("This is the depth of the nut cavity from the top of the fretboard.");

    auto computed = inputs->addGroupCommandInput("global_values", "Computed values");
    computed->isEnabled(false);
    auto nut_width = computed->children()->addFloatSpinnerCommandInput("nut_width", "Computed nut width", "mm", 0, 1000, 0.1, 45);
    nut_width->tooltip("This is calculated for you.");
    nut_width->tooltipDescription("It indicates the actual width of the nut/fretboard depending on the values you have used for the other parameters");
    CHECK2(nut_width);
    nut_width->isEnabled(false);
}

// ---------------------------------------------------------------------------
// CustomFeature serialization helpers
//
// Unit convention
// ---------------
// The Instrument struct, AFTER instrument.scale(10), holds all length fields
// in mm.  Fusion stores CustomFeatureParameter values in internal units (cm
// for lengths).  So for every length field we:
//   write: mm_value / 10  -> cm  (stored in Fusion)
//   read:  param->value() * 10  -> mm  (given back to createFretboard)
//
// Non-length fields (integer counts, booleans stored as 0/1, the perpendicular
// fret index and overhang_type enum) are dimensionless and stored/read as-is.
// ---------------------------------------------------------------------------

// Helper: value() returns 0 for a missing parameter (safe read)
static double cfParamVal(const Ptr<CustomFeature>& feature, const std::string& id) {
    auto params = feature->parameters();
    if (!params) return 0.0;
    auto p = params->itemById(id);
    return p ? p->value() : 0.0;
}

void InstrumentToCustomFeatureInput(const Ptr<CustomFeatureInput>& cfInput,
                                    const Instrument& instrument) {
    // instrument values are in mm (post scale(10)).
    // Fusion internal unit for length = cm, so divide by 10.
    auto L = [](double mm) { return ValueInput::createByReal(mm / 10.0); };
    auto D = [](double v)  { return ValueInput::createByReal(v); };

    auto addP = [&](const std::string& id, const std::string& label,
                    const Ptr<ValueInput>& val, const std::string& units) {
        cfInput->addCustomParameter(id, label, val, units);
    };

    addP("scale_length_bass",              "Bass Scale",            L(instrument.scale_length[0]),              "mm");
    addP("scale_length_treble",            "Treble Scale",          L(instrument.scale_length[1]),              "mm");
    addP("inter_string_spacing_at_nut",    "String Spacing Nut",    L(instrument.inter_string_spacing_at_nut),  "mm");
    addP("inter_string_spacing_at_bridge", "String Spacing Bridge", L(instrument.inter_string_spacing_at_bridge),"mm");
    addP("nut_to_zero_fret_offset",        "Nut-Zero Offset",       L(instrument.nut_to_zero_fret_offset),      "mm");
    addP("overhangs_0",                    "Overhang Bass Nut",     L(instrument.overhangs[0]),                 "mm");
    addP("overhangs_1",                    "Overhang Bass Last",    L(instrument.overhangs[1]),                 "mm");
    addP("overhangs_2",                    "Overhang Treble Nut",   L(instrument.overhangs[2]),                 "mm");
    addP("overhangs_3",                    "Overhang Treble Last",  L(instrument.overhangs[3]),                 "mm");
    addP("hidden_tang_length",             "Hidden Tang",           L(instrument.hidden_tang_length),           "mm");
    addP("fret_slots_width",               "Slot Width",            L(instrument.fret_slots_width),             "mm");
    addP("fret_slots_height",              "Slot Height",           L(instrument.fret_slots_height),            "mm");
    addP("fret_crown_width",               "Crown Width",           L(instrument.fret_crown_width),             "mm");
    addP("fret_crown_height",              "Crown Height",          L(instrument.fret_crown_height),            "mm");
    addP("last_fret_cut_offset",           "Last Fret Offset",      L(instrument.last_fret_cut_offset),         "mm");
    addP("space_before_nut",               "Space Before Nut",      L(instrument.space_before_nut),             "mm");
    addP("nut_thickness",                  "Nut Thickness",         L(instrument.nut_thickness),                "mm");
    addP("nut_height_under",               "Nut Slot Depth",        L(instrument.nut_height_under),             "mm");
    addP("radius_at_nut",                  "Radius Nut",            L(instrument.radius_at_nut),                "mm");
    addP("radius_at_last_fret",            "Radius Last Fret",      L(instrument.radius_at_last_fret),          "mm");
    addP("fretboard_thickness",            "Thickness",             L(instrument.fretboard_thickness),          "mm");

    // Dimensionless / integer / boolean parameters (stored as plain real, no units).
    addP("perpendicular_fret_index", "Perp Fret",        D(instrument.perpendicular_fret_index),      "");
    addP("number_of_strings",        "String Count",     D((double)instrument.number_of_strings),     "");
    addP("number_of_frets",          "Fret Count",       D((double)instrument.number_of_frets),       "");
    addP("overhang_type",            "Overhang Type",    D((double)instrument.overhang_type),          "");
    addP("right_handed",             "Right Handed",     D(instrument.right_handed   ? 1.0 : 0.0),    "");
    addP("has_zero_fret",            "Has Zero Fret",    D(instrument.has_zero_fret  ? 1.0 : 0.0),    "");
    addP("carve_nut_slot",           "Carve Nut Slot",   D(instrument.carve_nut_slot ? 1.0 : 0.0),    "");
    addP("draw_strings",             "Draw Strings",     D(instrument.draw_strings   ? 1.0 : 0.0),    "");
    addP("draw_frets",               "Draw Frets",       D(instrument.draw_frets     ? 1.0 : 0.0),    "");
    addP("carve_fret_slots",         "Carve Fret Slots", D(instrument.carve_fret_slots ? 1.0 : 0.0),  "");
}

Instrument InstrumentFromCustomFeature(const Ptr<CustomFeature>& feature) {
    // Length params are stored in cm; multiply by 10 to restore mm.
    auto L = [&](const std::string& id) { return cfParamVal(feature, id) * 10.0; };
    auto D = [&](const std::string& id) { return cfParamVal(feature, id); };
    auto B = [&](const std::string& id) { return (int)cfParamVal(feature, id) != 0; };
    auto I = [&](const std::string& id) { return (int)cfParamVal(feature, id); };

    Instrument instrument;
    instrument.scale_length[0]              = L("scale_length_bass");
    instrument.scale_length[1]              = L("scale_length_treble");
    instrument.inter_string_spacing_at_nut  = L("inter_string_spacing_at_nut");
    instrument.inter_string_spacing_at_bridge = L("inter_string_spacing_at_bridge");
    instrument.nut_to_zero_fret_offset      = L("nut_to_zero_fret_offset");
    instrument.overhangs[0]                 = L("overhangs_0");
    instrument.overhangs[1]                 = L("overhangs_1");
    instrument.overhangs[2]                 = L("overhangs_2");
    instrument.overhangs[3]                 = L("overhangs_3");
    instrument.hidden_tang_length           = L("hidden_tang_length");
    instrument.fret_slots_width             = L("fret_slots_width");
    instrument.fret_slots_height            = L("fret_slots_height");
    instrument.fret_crown_width             = L("fret_crown_width");
    instrument.fret_crown_height            = L("fret_crown_height");
    instrument.last_fret_cut_offset         = L("last_fret_cut_offset");
    instrument.space_before_nut             = L("space_before_nut");
    instrument.nut_thickness                = L("nut_thickness");
    instrument.nut_height_under             = L("nut_height_under");
    instrument.radius_at_nut                = L("radius_at_nut");
    instrument.radius_at_last_fret          = L("radius_at_last_fret");
    instrument.fretboard_thickness          = L("fretboard_thickness");

    instrument.perpendicular_fret_index     = D("perpendicular_fret_index");
    instrument.number_of_strings            = I("number_of_strings");
    instrument.number_of_frets              = I("number_of_frets");
    instrument.overhang_type                = (OverhangType)I("overhang_type");
    instrument.right_handed                 = B("right_handed");
    instrument.has_zero_fret                = B("has_zero_fret");
    instrument.carve_nut_slot               = B("carve_nut_slot");
    instrument.draw_strings                 = B("draw_strings");
    instrument.draw_frets                   = B("draw_frets");
    instrument.carve_fret_slots             = B("carve_fret_slots");

    instrument.validate();
    // Already in mm — do NOT call scale(10) again.
    return instrument;
}

void InstrumentToCustomFeature(const Ptr<CustomFeature>& feature,
                               const Instrument& instrument) {
    // instrument is in mm; Fusion stores in cm, so divide by 10.
    auto params = feature->parameters();
    if (!params) return;

    auto setL = [&](const std::string& id, double mm) {
        auto p = params->itemById(id);
        if (p) p->value(mm / 10.0);
    };
    auto setD = [&](const std::string& id, double v) {
        auto p = params->itemById(id);
        if (p) p->value(v);
    };

    setL("scale_length_bass",              instrument.scale_length[0]);
    setL("scale_length_treble",            instrument.scale_length[1]);
    setL("inter_string_spacing_at_nut",    instrument.inter_string_spacing_at_nut);
    setL("inter_string_spacing_at_bridge", instrument.inter_string_spacing_at_bridge);
    setL("nut_to_zero_fret_offset",        instrument.nut_to_zero_fret_offset);
    setL("overhangs_0",                    instrument.overhangs[0]);
    setL("overhangs_1",                    instrument.overhangs[1]);
    setL("overhangs_2",                    instrument.overhangs[2]);
    setL("overhangs_3",                    instrument.overhangs[3]);
    setL("hidden_tang_length",             instrument.hidden_tang_length);
    setL("fret_slots_width",               instrument.fret_slots_width);
    setL("fret_slots_height",              instrument.fret_slots_height);
    setL("fret_crown_width",               instrument.fret_crown_width);
    setL("fret_crown_height",              instrument.fret_crown_height);
    setL("last_fret_cut_offset",           instrument.last_fret_cut_offset);
    setL("space_before_nut",               instrument.space_before_nut);
    setL("nut_thickness",                  instrument.nut_thickness);
    setL("nut_height_under",               instrument.nut_height_under);
    setL("radius_at_nut",                  instrument.radius_at_nut);
    setL("radius_at_last_fret",            instrument.radius_at_last_fret);
    setL("fretboard_thickness",            instrument.fretboard_thickness);

    setD("perpendicular_fret_index",       instrument.perpendicular_fret_index);
    setD("number_of_strings",              (double)instrument.number_of_strings);
    setD("number_of_frets",                (double)instrument.number_of_frets);
    setD("overhang_type",                  (double)instrument.overhang_type);
    setD("right_handed",                   instrument.right_handed  ? 1.0 : 0.0);
    setD("has_zero_fret",                  instrument.has_zero_fret ? 1.0 : 0.0);
    setD("carve_nut_slot",                 instrument.carve_nut_slot ? 1.0 : 0.0);
    setD("draw_strings",                   instrument.draw_strings  ? 1.0 : 0.0);
    setD("draw_frets",                     instrument.draw_frets    ? 1.0 : 0.0);
    setD("carve_fret_slots",               instrument.carve_fret_slots ? 1.0 : 0.0);
}
