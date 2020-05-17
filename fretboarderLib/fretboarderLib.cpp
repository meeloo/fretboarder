//
//  fretboarderLib.cpp
//  fretboarderLib
//
//  Created by Sebastien Metrot on 17/05/2020.
//  Copyright © 2020 Autodesk. All rights reserved.
//

#include <iostream>
#include "fretboarderLib.hpp"
#include "fretboarderLibPriv.hpp"

void fretboarderLib::HelloWorld(const char * s)
{
    fretboarderLibPriv *theObj = new fretboarderLibPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void fretboarderLibPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

