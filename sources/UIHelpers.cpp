//
//  UIHelpers.cpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#include "Fretboarder.h"
#include "UIHelpers.hpp"

void progress(const Ptr<ProgressDialog>& dialog, const std::string& message) {
    dialog->message(message);
    dialog->progressValue(dialog->progressValue() + 1);
}

void OnValidateInputsEventHander::notify(const Ptr<ValidateInputsEventArgs>& eventArgs) {
    bool valid = true;
    auto instrument = InstrumentFromInputs(eventArgs->inputs());
    auto fretboard = Fretboard(instrument);
    double widthAtNut = abs(fretboard.board_shape().points[3].y - fretboard.board_shape().points[0].y);
    double widthAtLastFret = abs(fretboard.board_shape().points[2].y - fretboard.board_shape().points[1].y);
    double thickness = instrument.fretboard_thickness;
    double radius1 = instrument.radius_at_nut;
    double radius2 = instrument.radius_at_last_fret;
    double sin1 = widthAtNut / (2 * radius1);
    double angle1 = asin(sin1);
    double minThickness1 = radius1 - (cos(angle1) * radius1);

    double sin2 = widthAtLastFret / (2 * radius2);
    double angle2 = asin(sin2);
    double minThickness2 = radius2 - (cos(angle2) * radius2);

    valid = valid && (minThickness1 < thickness - instrument.fret_slots_height - 0.1);
    valid = valid && (minThickness2 < thickness - instrument.fret_slots_height - 0.11);

    eventArgs->areInputsValid(valid);
}

