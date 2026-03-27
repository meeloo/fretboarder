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
    Ptr<BoolValueCommandInput>    right_handed                   = inputs->itemById(Param::right_handed);
    Ptr<IntegerSliderCommandInput>number_of_strings              = inputs->itemById(Param::number_of_strings);
    Ptr<ValueCommandInput>        scale_length_treble            = inputs->itemById(Param::scale_length_treble);
    Ptr<ValueCommandInput>        scale_length_bass              = inputs->itemById(Param::scale_length_bass);
    Ptr<ValueCommandInput>        perpendicular_fret_index       = inputs->itemById(Param::perpendicular_fret_index);
    Ptr<ValueCommandInput>        inter_string_spacing_at_nut    = inputs->itemById(Param::inter_string_spacing_at_nut);
    Ptr<ValueCommandInput>        inter_string_spacing_at_bridge = inputs->itemById(Param::inter_string_spacing_at_bridge);
    Ptr<BoolValueCommandInput>    has_zero_fret                  = inputs->itemById(Param::has_zero_fret);
    Ptr<ValueCommandInput>        nut_to_zero_fret_offset        = inputs->itemById(Param::nut_to_zero_fret_offset);
    Ptr<ValueCommandInput>        space_before_nut               = inputs->itemById(Param::space_before_nut);
    Ptr<BoolValueCommandInput>    carve_nut_slot                 = inputs->itemById(Param::carve_nut_slot);
    Ptr<ValueCommandInput>        nut_thickness                  = inputs->itemById(Param::nut_thickness);
    Ptr<ValueCommandInput>        nut_height_under               = inputs->itemById(Param::nut_height_under);
    Ptr<ValueCommandInput>        nut_width                      = inputs->itemById(Param::nut_width);

    Ptr<ValueCommandInput>        radius_at_nut                  = inputs->itemById(Param::radius_at_nut);
    Ptr<ValueCommandInput>        radius_at_last_fret            = inputs->itemById(Param::radius_at_last_fret);
    Ptr<ValueCommandInput>        fretboard_thickness            = inputs->itemById(Param::fretboard_thickness);
    Ptr<ValueCommandInput>        number_of_frets                = inputs->itemById(Param::number_of_frets);
    Ptr<BoolValueCommandInput>    draw_strings                   = inputs->itemById(Param::draw_strings);
    Ptr<BoolValueCommandInput>    draw_frets                     = inputs->itemById(Param::draw_frets);
    Ptr<BoolValueCommandInput>    carve_fret_slots               = inputs->itemById(Param::carve_fret_slots);
    Ptr<DropDownCommandInput>     overhang_type                  = inputs->itemById(Param::overhang_type);
    Ptr<ValueCommandInput>        overhangSingle                 = inputs->itemById(Param::overhangSingle);
    Ptr<ValueCommandInput>        overhangNut                    = inputs->itemById(Param::overhangNut);
    Ptr<ValueCommandInput>        overhangLast                   = inputs->itemById(Param::overhangLast);
    Ptr<ValueCommandInput>        overhang0                      = inputs->itemById(Param::overhang0);
    Ptr<ValueCommandInput>        overhang1                      = inputs->itemById(Param::overhang1);
    Ptr<ValueCommandInput>        overhang2                      = inputs->itemById(Param::overhang2);
    Ptr<ValueCommandInput>        overhang3                      = inputs->itemById(Param::overhang3);
    Ptr<ValueCommandInput>        hidden_tang_length             = inputs->itemById(Param::hidden_tang_length);
    Ptr<ValueCommandInput>        fret_slots_width               = inputs->itemById(Param::fret_slots_width);
    Ptr<ValueCommandInput>        fret_slots_height              = inputs->itemById(Param::fret_slots_height);
    Ptr<ValueCommandInput>        fret_crown_width               = inputs->itemById(Param::fret_crown_width);
    Ptr<ValueCommandInput>        fret_crown_height              = inputs->itemById(Param::fret_crown_height);
    Ptr<ValueCommandInput>        last_fret_cut_offset           = inputs->itemById(Param::last_fret_cut_offset);

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
    instrument.number_of_frets = (int)round(number_of_frets->value());
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

    Ptr<BoolValueCommandInput>    right_handed                   = inputs->itemById(Param::right_handed);
    Ptr<IntegerSliderCommandInput>number_of_strings              = inputs->itemById(Param::number_of_strings);
    Ptr<ValueCommandInput>        scale_length_treble            = inputs->itemById(Param::scale_length_treble);
    Ptr<ValueCommandInput>        scale_length_bass              = inputs->itemById(Param::scale_length_bass);
    Ptr<ValueCommandInput>        perpendicular_fret_index       = inputs->itemById(Param::perpendicular_fret_index);
    Ptr<ValueCommandInput>        inter_string_spacing_at_nut    = inputs->itemById(Param::inter_string_spacing_at_nut);
    Ptr<ValueCommandInput>        inter_string_spacing_at_bridge = inputs->itemById(Param::inter_string_spacing_at_bridge);
    Ptr<BoolValueCommandInput>    has_zero_fret                  = inputs->itemById(Param::has_zero_fret);
    Ptr<ValueCommandInput>        nut_to_zero_fret_offset        = inputs->itemById(Param::nut_to_zero_fret_offset);
    Ptr<ValueCommandInput>        space_before_nut               = inputs->itemById(Param::space_before_nut);
    Ptr<BoolValueCommandInput>    carve_nut_slot                 = inputs->itemById(Param::carve_nut_slot);
    Ptr<ValueCommandInput>        nut_thickness                  = inputs->itemById(Param::nut_thickness);
    Ptr<ValueCommandInput>        nut_height_under               = inputs->itemById(Param::nut_height_under);
    Ptr<ValueCommandInput>        nut_width                      = inputs->itemById(Param::nut_width);
    Ptr<ValueCommandInput>        radius_at_nut                  = inputs->itemById(Param::radius_at_nut);
    Ptr<ValueCommandInput>        radius_at_last_fret            = inputs->itemById(Param::radius_at_last_fret);
    Ptr<ValueCommandInput>        fretboard_thickness            = inputs->itemById(Param::fretboard_thickness);
    Ptr<ValueCommandInput>        number_of_frets                = inputs->itemById(Param::number_of_frets);
    Ptr<DropDownCommandInput>     overhang_type                  = inputs->itemById(Param::overhang_type);
    Ptr<ValueCommandInput>        overhangSingle                 = inputs->itemById(Param::overhangSingle);
    Ptr<ValueCommandInput>        overhangNut                    = inputs->itemById(Param::overhangNut);
    Ptr<ValueCommandInput>        overhangLast                   = inputs->itemById(Param::overhangLast);
    Ptr<ValueCommandInput>        overhang0                      = inputs->itemById(Param::overhang0);
    Ptr<ValueCommandInput>        overhang1                      = inputs->itemById(Param::overhang1);
    Ptr<ValueCommandInput>        overhang2                      = inputs->itemById(Param::overhang2);
    Ptr<ValueCommandInput>        overhang3                      = inputs->itemById(Param::overhang3);
    Ptr<ValueCommandInput>        hidden_tang_length             = inputs->itemById(Param::hidden_tang_length);
    Ptr<ValueCommandInput>        fret_slots_width               = inputs->itemById(Param::fret_slots_width);
    Ptr<ValueCommandInput>        fret_slots_height              = inputs->itemById(Param::fret_slots_height);
    Ptr<ValueCommandInput>        fret_crown_width               = inputs->itemById(Param::fret_crown_width);
    Ptr<ValueCommandInput>        fret_crown_height              = inputs->itemById(Param::fret_crown_height);
    Ptr<ValueCommandInput>        last_fret_cut_offset           = inputs->itemById(Param::last_fret_cut_offset);

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
    number_of_frets->value((double)instrument.number_of_frets);
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

    auto rightHanded = group->addBoolValueInput(Param::right_handed, "Right handed", true, "", true);
    rightHanded->tooltip("Switch in between right and left handed string layout.");
    rightHanded->tooltipDescription("The left handed version just mirrors the layout of the strings.");

    auto number_of_strings_slider = group->addIntegerSliderCommandInput(Param::number_of_strings, "Count", 1, 15);
    number_of_strings_slider->tooltip("Select the number of strings for your instrument.");
    number_of_strings_slider->tooltipDescription("Minimum is 1 string, maximum 15.");
    CHECK2(number_of_strings_slider);
    number_of_strings_slider->valueOne(6);

    auto inter_string_spacing_at_nut = group->addValueInput(Param::inter_string_spacing_at_nut, "Spacing at nut", "mm", ValueInput::createByString("7.5 mm"));
    inter_string_spacing_at_nut->tooltip("This is the distance in between each string on the nut (or the zero string if you have one)");

    auto inter_string_spacing_at_bridge = group->addValueInput(Param::inter_string_spacing_at_bridge, "Spacing at bridge", "mm", ValueInput::createByString("12 mm"));
    inter_string_spacing_at_bridge->tooltip("This is the distance in between each string on the bridge.");
    inter_string_spacing_at_bridge->tooltipDescription("This may be dictated by the design of your bridge.");

    auto overhang_type = group->addDropDownCommandInput(Param::overhang_type, "Overhang type", TextListDropDownStyle);
    auto overhang_items = overhang_type->listItems();
    overhang_items->add("single", true);
    overhang_items->add("nut and last fret", false);
    overhang_items->add("all", false);

    auto overhangSingle = group->addValueInput(Param::overhangSingle, "Fret overhang", "mm", ValueInput::createByString("3 mm"));
    overhangSingle->tooltip("This is the distance in between the outer strings and the border of the fretboard");

    auto overhangNut = group->addValueInput(Param::overhangNut, "Fret overhang at the nut", "mm", ValueInput::createByString("3 mm"));
    overhangNut->tooltip("This is the distance in between the outer string and the border of the fretboard at the nut");
    overhangNut->isVisible(false);

    auto overhangLast = group->addValueInput(Param::overhangLast, "Fret overhang at last fret", "mm", ValueInput::createByString("3 mm"));
    overhangLast->tooltip("This is the distance in between the outer strings and the border of the fretboard at the last fret");
    overhangLast->isVisible(false);

    auto overhang0 = group->addValueInput(Param::overhang0, "Fret overhang at nut (bass)", "mm", ValueInput::createByString("3 mm"));
    overhang0->tooltip("This is the distance in between the first string and the border of the fretboard at the nut");
    overhang0->isVisible(false);

    auto overhang1 = group->addValueInput(Param::overhang1, "Fret overhang at last fret (bass)", "mm", ValueInput::createByString("3 mm"));
    overhang1->tooltip("This is the distance in between the first string and the border of the fretboard at the last fret");
    overhang1->isVisible(false);

    auto overhang2 = group->addValueInput(Param::overhang2, "Fret overhang at nut (treble)", "mm", ValueInput::createByString("3 mm"));
    overhang2->tooltip("This is the distance in between the last string and the border of the fretboard at the nut");
    overhang2->isVisible(false);

    auto overhang3 = group->addValueInput(Param::overhang3, "Fret overhang at last fret (treble)", "mm", ValueInput::createByString("3 mm"));
    overhang3->tooltip("This is the distance in between the last string and the border of the fretboard at last fret");
    overhang3->isVisible(false);

    auto draw_strings = group->addBoolValueInput(Param::draw_strings, "Draw strings", true, "", true);
    draw_strings->tooltip("Enabling this will create a sketch for the position of the strings following the ideal radius of the fretboard and the bridge.");

    group = inputs->addTabCommandInput("scale_length", "Scale length")->children();
    auto scale_length_bass = group->addValueInput(Param::scale_length_bass, "Bass side", "in", ValueInput::createByString("25.5 in"));
    scale_length_bass->tooltip("Enter the scale length of the bass string");
    scale_length_bass->tooltipDescription("This is independent of the handedness of the instrument you are building. Use the 'right handed' switch above to control that instead.");

    auto scale_length_treble = group->addValueInput(Param::scale_length_treble, "Treble side", "in", ValueInput::createByString("25 in"));
    scale_length_treble->tooltip("Enter the scale length of the treble string");
    scale_length_treble->tooltipDescription("This is independent of the handedness of the instrument you are building. Use the 'right handed' switch above to control that instead.");

    group = inputs->addTabCommandInput("fretboard_radius", "Fretboard")->children();
    auto radius_at_nut = group->addValueInput(Param::radius_at_nut, "Radius at nut", "in", ValueInput::createByString("9.5 in"));
    radius_at_nut->tooltip("Enter the desired fretboard radius at the nut.");
    radius_at_nut->tooltipDescription("You can choose a different radius at the nut and at the last fret to create a compound radius.");

    auto radius_at_last_fret = group->addValueInput(Param::radius_at_last_fret, "Radius at last fret", "in", ValueInput::createByString("20 in"));
    radius_at_last_fret->tooltip("Enter the desired fretboard radius at the last fret.");
    radius_at_last_fret->tooltipDescription("You can choose a different radius at the nut and at the last fret to create a compound radius.");

    auto fretboard_thickness = group->addValueInput(Param::fretboard_thickness, "Thickness", "mm", ValueInput::createByString("7 mm"));
    fretboard_thickness->tooltip("This is the thickness of your fretboard.");
    fretboard_thickness->tooltipDescription("This has to be less than thickness of the stock plank you will use.");

    group = inputs->addTabCommandInput("frets", "Frets")->children();
    auto number_of_frets_input = group->addValueInput(Param::number_of_frets, "Number of frets", "", ValueInput::createByString("24"));
    number_of_frets_input->tooltip("This is the number of frets not counting the zero fret.");
    CHECK2(number_of_frets_input);

    auto perpendicular_fret_index = group->addValueInput(Param::perpendicular_fret_index, "Perpendicular Fret", "", ValueInput::createByString("0"));
    perpendicular_fret_index->tooltip("This is the position of the perpendicular fret for multiscale instruments. Use 100 to have the bridge be perpendicular to the strings (i.e. not slanted)");
    perpendicular_fret_index->tooltipDescription("This number is a floating point representing the position of the fretboard. If you choose, say, 7.5, the perpandicular position will be calculated as if it was right in between the 7th and the 8th fret. You can also use a negative number to completely offset the slanting of the string.");

    auto has_zero_fret = group->addBoolValueInput(Param::has_zero_fret, "Zero fret", true, "", true);
    has_zero_fret->tooltip("Enable or disable the use of a zero-fret.");
    has_zero_fret->tooltipDescription("Seriously, why would you disable the zero fret? Are you nuts?");

    auto nut_to_zero_fret_offset = group->addValueInput(Param::nut_to_zero_fret_offset, "Distance from nut to zero fret", "mm", ValueInput::createByString("3 mm"));
    nut_to_zero_fret_offset->tooltip("This is the distance in between the zero fret's slot and the nut.");
    nut_to_zero_fret_offset->tooltipDescription("Is it ignored if you don't have a zero fret or if you choose not to carve the nut.");

    auto draw_frets = group->addBoolValueInput(Param::draw_frets, "Draw frets", true, "", true);
    draw_frets->tooltip("Enabling this will enable the creation of bodies that represent the frets.");
    draw_frets->tooltipDescription("This is purely for esthetics, if you want to create renders for a client with a fretboard that looks finished.");

    auto carve_fret_slots = group->addBoolValueInput(Param::carve_fret_slots, "Carve fret slots", true, "", true);
    carve_fret_slots->tooltip("Enabling this will enable the carving of fret slots in the fretboard.");

    auto hidden_tang_length = group->addValueInput(Param::hidden_tang_length, "Blind tang length", "mm", ValueInput::createByString("2 mm"));
    hidden_tang_length->tooltip("This is the distance in between the tang of the frets and the border of the fretboard plank");
    hidden_tang_length->tooltipDescription("If you want to have the fret tangs appearing on the border of the fretboard, use 0mm. Any other number will create blind/hidden frets tangs.");

    auto fret_slots_width = group->addValueInput(Param::fret_slots_width, "Fret slots width", "mm", ValueInput::createByString("0.6 mm"));
    fret_slots_width->tooltip("This is the kerf of the tool you will use to cut the fret slots and the thickness of the fret's tang. 0.5 to 0.6mm is generally a good range.");
    fret_slots_width->tooltipDescription("This is also used to model the fret tang bodies if you choose to draw the frets");

    auto fret_slots_height = group->addValueInput(Param::fret_slots_height, "Fret slots height", "mm", ValueInput::createByString("1.5 mm"));
    fret_slots_height->tooltip("This is the depth of the fret slots.");
    fret_slots_height->tooltipDescription("This should be greater of equal than the height of the tang of the frets you are going to use.");

    auto fret_crown_width = group->addValueInput(Param::fret_crown_width, "Fret crown width", "mm", ValueInput::createByString("2.34 mm"));
    fret_crown_width->tooltip("This is use to model the crown of the frets should you choose to draw them.");
    fret_crown_width->tooltipDescription("This is purely esthetic.");

    auto fret_crown_height = group->addValueInput(Param::fret_crown_height, "Fret crown height", "mm", ValueInput::createByString("1.22 mm"));
    fret_crown_height->tooltip("This is use to model the crown of the frets should you choose to draw them.");
    fret_crown_height->tooltipDescription("This is purely esthetic.");

    auto last_fret_cut_offset = group->addValueInput(Param::last_fret_cut_offset, "Last fret cut offset", "mm", ValueInput::createByString("0 mm"));
    last_fret_cut_offset->tooltip("This is an offset that is added after the last fret to choose the length of the fretboard.");
    last_fret_cut_offset->tooltipDescription("Using 0mm here will have your fretboard stopping one half fret after the last fret.");

    group = inputs->addTabCommandInput("nut", "Nut")->children();
    auto space_before_nut = group->addValueInput(Param::space_before_nut, "Space before nut", "mm", ValueInput::createByString("7 mm"));
    space_before_nut->tooltip("This is the amount of fretboard you need before in between it's start and the nut.");
    space_before_nut->tooltipDescription("This is probably 0 if you are using a Gibson type nut. Fender nuts typically need 3 to 5 mm here.");

    auto nut_thickness = group->addValueInput(Param::nut_thickness, "Thickness", "mm", ValueInput::createByString("4 mm"));
    nut_thickness->tooltip("This is the thickness/width of the nut (as seen from above).");

    auto carve_nut_slot = group->addBoolValueInput(Param::carve_nut_slot, "Carve the nut slot", true, "", true);
    carve_nut_slot->tooltip("You can disable carving the nut.");
    carve_nut_slot->tooltipDescription("Some headless instruments use the their mini head pieces as a nut/string guide.");

    auto nut_height_under = group->addValueInput(Param::nut_height_under, "Slot depth", "mm", ValueInput::createByString("3 mm"));
    nut_height_under->tooltip("This is the depth of the nut cavity from the top of the fretboard.");

    auto computed = inputs->addGroupCommandInput("global_values", "Computed values");
    computed->isEnabled(false);
    auto nut_width = computed->children()->addValueInput(Param::nut_width, "Computed nut width", "mm", ValueInput::createByString("45 mm"));
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
static double cfParamVal(const Ptr<CustomFeature>& feature, const char* id) {
    auto params = feature->parameters();
    if (!params) return 0.0;
    auto p = params->itemById(id);
    return p ? p->value() : 0.0;
}

void InstrumentToCustomFeatureInput(const Ptr<CustomFeatureInput>& cfInput,
                                    const Instrument& instrument,
                                    const Ptr<CommandInputs>& inputs) {
    // instrument values are in mm (post scale(10)).
    // Fusion internal unit for length = cm, so divide by 10.
    auto L = [](double mm) { return ValueInput::createByReal(mm / 10.0); };
    auto D = [](double v)  { return ValueInput::createByReal(v); };

    // When dialog inputs are available, use the expression string verbatim so
    // that user-parameter references (e.g. "myScale") are preserved.  Falls back
    // to a literal real value when inputs are not provided (e.g. compute handler).
    auto exprOrL = [&](const char* inputId, double fallbackMm) -> Ptr<ValueInput> {
        if (inputs) {
            auto input = inputs->itemById(inputId)->cast<ValueCommandInput>();
            if (input && input->isValidExpression())
                return ValueInput::createByString(input->expression());
        }
        return L(fallbackMm);
    };

    // Overhangs: map the visible dialog input to the appropriate CF param index.
    auto overhangExpr = [&](int index) -> Ptr<ValueInput> {
        if (inputs) {
            const char* inputId;
            if (instrument.overhang_type == single) {
                inputId = Param::overhangSingle;
            } else if (instrument.overhang_type == nut_and_last_fret) {
                inputId = (index == 0 || index == 2) ? Param::overhangNut : Param::overhangLast;
            } else {
                // "all" mode: use overhang0..overhang3
                switch (index) {
                    case 0: inputId = Param::overhang0; break;
                    case 1: inputId = Param::overhang1; break;
                    case 2: inputId = Param::overhang2; break;
                    default: inputId = Param::overhang3; break;
                }
            }
            auto input = inputs->itemById(inputId)->cast<ValueCommandInput>();
            if (input && input->isValidExpression())
                return ValueInput::createByString(input->expression());
        }
        return L(instrument.overhangs[index]);
    };

    auto addP = [&](const char* id, const std::string& label,
                    const Ptr<ValueInput>& val, const std::string& units) {
        cfInput->addCustomParameter(id, label, val, units);
    };

    addP(Param::scale_length_bass,              "Bass Scale",            exprOrL(Param::scale_length_bass,              instrument.scale_length[0]),              "mm");
    addP(Param::scale_length_treble,            "Treble Scale",          exprOrL(Param::scale_length_treble,            instrument.scale_length[1]),              "mm");
    addP(Param::inter_string_spacing_at_nut,    "String Spacing Nut",    exprOrL(Param::inter_string_spacing_at_nut,    instrument.inter_string_spacing_at_nut),  "mm");
    addP(Param::inter_string_spacing_at_bridge, "String Spacing Bridge", exprOrL(Param::inter_string_spacing_at_bridge, instrument.inter_string_spacing_at_bridge),"mm");
    addP(Param::nut_to_zero_fret_offset,        "Nut-Zero Offset",       exprOrL(Param::nut_to_zero_fret_offset,        instrument.nut_to_zero_fret_offset),      "mm");
    addP(Param::overhangs_0,                    "Overhang Bass Nut",     overhangExpr(0),                                                                         "mm");
    addP(Param::overhangs_1,                    "Overhang Bass Last",    overhangExpr(1),                                                                         "mm");
    addP(Param::overhangs_2,                    "Overhang Treble Nut",   overhangExpr(2),                                                                         "mm");
    addP(Param::overhangs_3,                    "Overhang Treble Last",  overhangExpr(3),                                                                         "mm");
    addP(Param::hidden_tang_length,             "Hidden Tang",           exprOrL(Param::hidden_tang_length,             instrument.hidden_tang_length),           "mm");
    addP(Param::fret_slots_width,               "Slot Width",            exprOrL(Param::fret_slots_width,               instrument.fret_slots_width),             "mm");
    addP(Param::fret_slots_height,              "Slot Height",           exprOrL(Param::fret_slots_height,              instrument.fret_slots_height),            "mm");
    addP(Param::fret_crown_width,               "Crown Width",           exprOrL(Param::fret_crown_width,               instrument.fret_crown_width),             "mm");
    addP(Param::fret_crown_height,              "Crown Height",          exprOrL(Param::fret_crown_height,              instrument.fret_crown_height),            "mm");
    addP(Param::last_fret_cut_offset,           "Last Fret Offset",      exprOrL(Param::last_fret_cut_offset,           instrument.last_fret_cut_offset),         "mm");
    addP(Param::space_before_nut,               "Space Before Nut",      exprOrL(Param::space_before_nut,               instrument.space_before_nut),             "mm");
    addP(Param::nut_thickness,                  "Nut Thickness",         exprOrL(Param::nut_thickness,                  instrument.nut_thickness),                "mm");
    addP(Param::nut_height_under,               "Nut Slot Depth",        exprOrL(Param::nut_height_under,               instrument.nut_height_under),             "mm");
    addP(Param::radius_at_nut,                  "Radius Nut",            exprOrL(Param::radius_at_nut,                  instrument.radius_at_nut),                "mm");
    addP(Param::radius_at_last_fret,            "Radius Last Fret",      exprOrL(Param::radius_at_last_fret,            instrument.radius_at_last_fret),          "mm");
    addP(Param::fretboard_thickness,            "Thickness",             exprOrL(Param::fretboard_thickness,            instrument.fretboard_thickness),          "mm");

    // Perpendicular fret index is dimensionless — no unit conversion.
    auto perpExpr = [&]() -> Ptr<ValueInput> {
        if (inputs) {
            auto input = inputs->itemById(Param::perpendicular_fret_index)->cast<ValueCommandInput>();
            if (input && input->isValidExpression())
                return ValueInput::createByString(input->expression());
        }
        return D(instrument.perpendicular_fret_index);
    };
    addP(Param::perpendicular_fret_index, "Perp Fret",        perpExpr(),                                          "");
    addP(Param::number_of_strings,        "String Count",     D((double)instrument.number_of_strings),             "");

    auto fretCountExpr = [&]() -> Ptr<ValueInput> {
        if (inputs) {
            auto input = inputs->itemById(Param::number_of_frets)->cast<ValueCommandInput>();
            if (input && input->isValidExpression())
                return ValueInput::createByString(input->expression());
        }
        return D((double)instrument.number_of_frets);
    };
    addP(Param::number_of_frets,          "Fret Count",       fretCountExpr(),                                     "");
    addP(Param::overhang_type,            "Overhang Type",    D((double)instrument.overhang_type),                 "");
    addP(Param::right_handed,             "Right Handed",     D(instrument.right_handed     ? 1.0 : 0.0),          "");
    addP(Param::has_zero_fret,            "Has Zero Fret",    D(instrument.has_zero_fret    ? 1.0 : 0.0),          "");
    addP(Param::carve_nut_slot,           "Carve Nut Slot",   D(instrument.carve_nut_slot   ? 1.0 : 0.0),          "");
    addP(Param::draw_strings,             "Draw Strings",     D(instrument.draw_strings     ? 1.0 : 0.0),          "");
    addP(Param::draw_frets,               "Draw Frets",       D(instrument.draw_frets       ? 1.0 : 0.0),          "");
    addP(Param::carve_fret_slots,         "Carve Fret Slots", D(instrument.carve_fret_slots ? 1.0 : 0.0),          "");
}

Instrument InstrumentFromCustomFeature(const Ptr<CustomFeature>& feature) {
    // Length params are stored in cm; multiply by 10 to restore mm.
    auto L = [&](const char* id) { return cfParamVal(feature, id) * 10.0; };
    auto D = [&](const char* id) { return cfParamVal(feature, id); };
    auto B = [&](const char* id) { return (int)cfParamVal(feature, id) != 0; };
    auto I = [&](const char* id) { return (int)cfParamVal(feature, id); };

    Instrument instrument;
    instrument.scale_length[0]              = L(Param::scale_length_bass);
    instrument.scale_length[1]              = L(Param::scale_length_treble);
    instrument.inter_string_spacing_at_nut  = L(Param::inter_string_spacing_at_nut);
    instrument.inter_string_spacing_at_bridge = L(Param::inter_string_spacing_at_bridge);
    instrument.nut_to_zero_fret_offset      = L(Param::nut_to_zero_fret_offset);
    instrument.overhangs[0]                 = L(Param::overhangs_0);
    instrument.overhangs[1]                 = L(Param::overhangs_1);
    instrument.overhangs[2]                 = L(Param::overhangs_2);
    instrument.overhangs[3]                 = L(Param::overhangs_3);
    instrument.hidden_tang_length           = L(Param::hidden_tang_length);
    instrument.fret_slots_width             = L(Param::fret_slots_width);
    instrument.fret_slots_height            = L(Param::fret_slots_height);
    instrument.fret_crown_width             = L(Param::fret_crown_width);
    instrument.fret_crown_height            = L(Param::fret_crown_height);
    instrument.last_fret_cut_offset         = L(Param::last_fret_cut_offset);
    instrument.space_before_nut             = L(Param::space_before_nut);
    instrument.nut_thickness                = L(Param::nut_thickness);
    instrument.nut_height_under             = L(Param::nut_height_under);
    instrument.radius_at_nut                = L(Param::radius_at_nut);
    instrument.radius_at_last_fret          = L(Param::radius_at_last_fret);
    instrument.fretboard_thickness          = L(Param::fretboard_thickness);

    instrument.perpendicular_fret_index     = D(Param::perpendicular_fret_index);
    instrument.number_of_strings            = I(Param::number_of_strings);
    instrument.number_of_frets              = I(Param::number_of_frets);
    instrument.overhang_type                = (OverhangType)I(Param::overhang_type);
    instrument.right_handed                 = B(Param::right_handed);
    instrument.has_zero_fret                = B(Param::has_zero_fret);
    instrument.carve_nut_slot               = B(Param::carve_nut_slot);
    instrument.draw_strings                 = B(Param::draw_strings);
    instrument.draw_frets                   = B(Param::draw_frets);
    instrument.carve_fret_slots             = B(Param::carve_fret_slots);

    instrument.validate();
    // Already in mm — do NOT call scale(10) again.
    return instrument;
}

void InstrumentToCustomFeature(const Ptr<CustomFeature>& feature,
                               const Instrument& instrument) {
    // instrument is in mm; Fusion stores in cm, so divide by 10.
    auto params = feature->parameters();
    if (!params) return;

    auto setL = [&](const char* id, double mm) {
        auto p = params->itemById(id);
        if (p) p->value(mm / 10.0);
    };
    auto setD = [&](const char* id, double v) {
        auto p = params->itemById(id);
        if (p) p->value(v);
    };

    setL(Param::scale_length_bass,              instrument.scale_length[0]);
    setL(Param::scale_length_treble,            instrument.scale_length[1]);
    setL(Param::inter_string_spacing_at_nut,    instrument.inter_string_spacing_at_nut);
    setL(Param::inter_string_spacing_at_bridge, instrument.inter_string_spacing_at_bridge);
    setL(Param::nut_to_zero_fret_offset,        instrument.nut_to_zero_fret_offset);
    setL(Param::overhangs_0,                    instrument.overhangs[0]);
    setL(Param::overhangs_1,                    instrument.overhangs[1]);
    setL(Param::overhangs_2,                    instrument.overhangs[2]);
    setL(Param::overhangs_3,                    instrument.overhangs[3]);
    setL(Param::hidden_tang_length,             instrument.hidden_tang_length);
    setL(Param::fret_slots_width,               instrument.fret_slots_width);
    setL(Param::fret_slots_height,              instrument.fret_slots_height);
    setL(Param::fret_crown_width,               instrument.fret_crown_width);
    setL(Param::fret_crown_height,              instrument.fret_crown_height);
    setL(Param::last_fret_cut_offset,           instrument.last_fret_cut_offset);
    setL(Param::space_before_nut,               instrument.space_before_nut);
    setL(Param::nut_thickness,                  instrument.nut_thickness);
    setL(Param::nut_height_under,               instrument.nut_height_under);
    setL(Param::radius_at_nut,                  instrument.radius_at_nut);
    setL(Param::radius_at_last_fret,            instrument.radius_at_last_fret);
    setL(Param::fretboard_thickness,            instrument.fretboard_thickness);

    setD(Param::perpendicular_fret_index,       instrument.perpendicular_fret_index);
    setD(Param::number_of_strings,              (double)instrument.number_of_strings);
    setD(Param::number_of_frets,                (double)instrument.number_of_frets);
    setD(Param::overhang_type,                  (double)instrument.overhang_type);
    setD(Param::right_handed,                   instrument.right_handed  ? 1.0 : 0.0);
    setD(Param::has_zero_fret,                  instrument.has_zero_fret ? 1.0 : 0.0);
    setD(Param::carve_nut_slot,                 instrument.carve_nut_slot ? 1.0 : 0.0);
    setD(Param::draw_strings,                   instrument.draw_strings  ? 1.0 : 0.0);
    setD(Param::draw_frets,                     instrument.draw_frets    ? 1.0 : 0.0);
    setD(Param::carve_fret_slots,               instrument.carve_fret_slots ? 1.0 : 0.0);
}

// ---------------------------------------------------------------------------
// CfExpressionsToInputs
//
// After calling InstrumentToInputs() to populate boolean/integer controls,
// call this to restore expression strings for all float fields from the CF's
// stored model parameters.  This preserves user-parameter references (e.g.
// "myScale") when reopening the edit dialog.
// ---------------------------------------------------------------------------
void CfExpressionsToInputs(const Ptr<CommandInputs>& inputs,
                           const Ptr<CustomFeature>& cf) {
    auto params = cf->parameters();
    if (!params) return;

    // Copies the expression of a CF parameter into the matching ValueCommandInput.
    auto setExpr = [&](const char* inputId, const char* cfParamId) {
        auto p = params->itemById(cfParamId);
        if (!p) return;
        auto input = inputs->itemById(inputId)->cast<ValueCommandInput>();
        if (!input) return;
        input->expression(p->expression());
    };

    setExpr(Param::scale_length_bass,              Param::scale_length_bass);
    setExpr(Param::scale_length_treble,            Param::scale_length_treble);
    setExpr(Param::inter_string_spacing_at_nut,    Param::inter_string_spacing_at_nut);
    setExpr(Param::inter_string_spacing_at_bridge, Param::inter_string_spacing_at_bridge);
    setExpr(Param::nut_to_zero_fret_offset,        Param::nut_to_zero_fret_offset);
    setExpr(Param::hidden_tang_length,             Param::hidden_tang_length);
    setExpr(Param::fret_slots_width,               Param::fret_slots_width);
    setExpr(Param::fret_slots_height,              Param::fret_slots_height);
    setExpr(Param::fret_crown_width,               Param::fret_crown_width);
    setExpr(Param::fret_crown_height,              Param::fret_crown_height);
    setExpr(Param::last_fret_cut_offset,           Param::last_fret_cut_offset);
    setExpr(Param::space_before_nut,               Param::space_before_nut);
    setExpr(Param::nut_thickness,                  Param::nut_thickness);
    setExpr(Param::nut_height_under,               Param::nut_height_under);
    setExpr(Param::radius_at_nut,                  Param::radius_at_nut);
    setExpr(Param::radius_at_last_fret,            Param::radius_at_last_fret);
    setExpr(Param::fretboard_thickness,            Param::fretboard_thickness);
    setExpr(Param::perpendicular_fret_index,       Param::perpendicular_fret_index);
    setExpr(Param::number_of_frets,                Param::number_of_frets);

    // Overhang fields: pick the visible input for the current overhang mode.
    int overhangType = (int)cfParamVal(cf, Param::overhang_type);
    if (overhangType == (int)single) {
        setExpr(Param::overhangSingle, Param::overhangs_0);
    } else if (overhangType == (int)nut_and_last_fret) {
        setExpr(Param::overhangNut,  Param::overhangs_0);
        setExpr(Param::overhangLast, Param::overhangs_1);
    } else {
        setExpr(Param::overhang0, Param::overhangs_0);
        setExpr(Param::overhang1, Param::overhangs_1);
        setExpr(Param::overhang2, Param::overhangs_2);
        setExpr(Param::overhang3, Param::overhangs_3);
    }
}
