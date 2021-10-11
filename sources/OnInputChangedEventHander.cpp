//
//  OnInputChangedEventHander.cpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#include "Fretboarder.h"
#include "OnInputChangedEventHander.hpp"

// InputChange event handler.
void OnInputChangedEventHander::notify(const Ptr<InputChangedEventArgs>& eventArgs)
{
    Ptr<Command> command = eventArgs->firingEvent()->sender();
    if (!command)
        return;
    Ptr<CommandInputs> inputs = command->commandInputs();
    if (!inputs)
        return;

    Ptr<CommandInput> cmdInput = eventArgs->input();
    if (!cmdInput)
        return;

    if (cmdInput->id() == "presets") {
        // Preset has changed
        Ptr<DropDownCommandInput> presetCombo = cmdInput;
        CHECK2(presetCombo)
        auto item = presetCombo->selectedItem();
        if (!item) {
            return;
        }

        auto index = item->index();
        auto preset = Preset::presets()[index];
        preset.instrument.validate();

        // Apply Preset:
        InstrumentToInputs(inputs, preset.instrument);

    } else if (cmdInput->id() == "Load") {
        auto fileDialog = Fretboarder::ui->createFileDialog();
        fileDialog->isMultiSelectEnabled(false);
        fileDialog->title("Choose a file to load the fretboard preset from");
        fileDialog->filter("Fretboards (*.frt)");
        if (DialogOK == fileDialog->showOpen()) {
            std::string filename = fileDialog->filename();
            Instrument instrument;
            if (!instrument.load(filename)) {
                std::stringstream str;
                str << "Unable to load fretboard file \"" << filename << "\"";
                Fretboarder::ui->messageBox(str.str());
                return;
            }
            instrument.scale(0.1); // mm to cm
            InstrumentToInputs(inputs, instrument);
        }

    } else if (cmdInput->id() == "Save") {
        auto fileDialog = Fretboarder::ui->createFileDialog();
        fileDialog->isMultiSelectEnabled(false);
        fileDialog->title("Choose a file to save the fretboard preset to");
        fileDialog->filter("Fretboards (*.frt)");
        if (DialogOK == fileDialog->showSave()) {
            std::string filename = fileDialog->filename();
            Instrument instrument = InstrumentFromInputs(inputs);
            if (!instrument.save(filename)) {
                std::stringstream str;
                str << "Unable to save fretboard file \"" << filename << "\"";
            }
        }

    } else if (cmdInput->id() == "overhang_type") {
        Instrument instrument = InstrumentFromInputs(inputs);
        instrument.scale(0.1); // mm to cm
        InstrumentToInputs(inputs, instrument);
    }

    UpdateNutWidget(inputs);

}

void OnInputChangedEventHander::UpdateNutWidget(const Ptr<CommandInputs>& inputs) {
    // update nut width
    Ptr<IntegerSliderCommandInput> number_of_strings = inputs->itemById("number_of_strings");
    Ptr<FloatSpinnerCommandInput> inter_string_spacing_at_nut = inputs->itemById("inter_string_spacing_at_nut");
    Ptr<DropDownCommandInput> overhang_type = inputs->itemById("overhang_type");
    Ptr<FloatSpinnerCommandInput> overhangSingle = inputs->itemById("overhangSingle");
    Ptr<FloatSpinnerCommandInput> overhangNut = inputs->itemById("overhangNut");
    Ptr<FloatSpinnerCommandInput> overhangLast = inputs->itemById("overhangLast");
    Ptr<FloatSpinnerCommandInput> overhang0 = inputs->itemById("overhang0");
    Ptr<FloatSpinnerCommandInput> overhang1 = inputs->itemById("overhang1");
    Ptr<FloatSpinnerCommandInput> overhang2 = inputs->itemById("overhang2");
    Ptr<FloatSpinnerCommandInput> overhang3 = inputs->itemById("overhang3");
    Ptr<FloatSpinnerCommandInput> nut_width = inputs->itemById("nut_width");
    CHECK2(number_of_strings);
    CHECK2(inter_string_spacing_at_nut);
    CHECK2(overhang_type);
    CHECK2(overhangSingle);
    CHECK2(overhangNut);
    CHECK2(overhangLast);
    CHECK2(overhang0);
    CHECK2(overhang1);
    CHECK2(overhang2);
    CHECK2(overhang3);
    CHECK2(nut_width);
    auto selected_item = overhang_type->selectedItem();
    double left = 0;
    double right = 0;
    if (selected_item != nullptr) {
        if (selected_item->name() == "single") {
            left = right = overhangSingle->value();
        } else if (selected_item->name() == "nut and last fret") {
            left = right = overhangNut->value();
        } else if (selected_item->name() == "all") {
            left = overhang0->value();
            right = overhang2->value();
        }
    }
    nut_width->value(std::max(number_of_strings->valueOne() - 1, 1) * inter_string_spacing_at_nut->value() + left + right);
}
