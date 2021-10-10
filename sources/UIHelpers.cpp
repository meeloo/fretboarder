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

