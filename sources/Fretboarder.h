//
//  Fretboarder.h
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#ifndef Fretboarder_h
#define Fretboarder_h

#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <CAM/CAM/CAM.h>
#include "Fretboard.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

using namespace adsk::core;
using namespace adsk::fusion;
using namespace adsk::cam;
using namespace fretboarder;

class Fretboarder {
public:
    static Ptr<Application> app;
    static Ptr<UserInterface> ui;
    static Ptr<CustomGraphicsGroups> cgGroups;
};

static void DisplayError(const std::string& file, int line, const std::string& exp)
{
    std::string err = "";
    Fretboarder::app->getLastError(&err);
    std::stringstream str;
    str << "ERROR" << file << ":" << line << "\n" << "expression failed: " << exp << "\nError message: " << err;
    std::string s = str.str();
    Fretboarder::ui->messageBox(s);
}

#define CHECK(X, Y) \
if (!(X)) {\
std::string exp = #X;\
DisplayError(__FILE__, __LINE__, exp);\
return Y;\
}

#define CHECK2(X) \
if (!(X)) {\
std::string exp = #X;\
DisplayError(__FILE__, __LINE__, exp);\
return;\
}


#define SHOWERROR() \
{ std::string err = "";\
Fretboarder::app->getLastError(&err);\
Fretboarder::ui->messageBox(err); }

#include "Instruments+Inputs.hpp"
#include "SketchHelpers.hpp"
#include "UIHelpers.hpp"
#include "OnInputChangedEventHander.hpp"

#endif /* Fretboarder_h */
