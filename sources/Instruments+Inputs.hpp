//
//  Instruments+Inputs.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#ifndef Instruments_Inputs_hpp
#define Instruments_Inputs_hpp

Instrument InstrumentFromInputs(const Ptr<CommandInputs>& inputs);
void InstrumentToInputs(const Ptr<CommandInputs>& inputs, const Instrument& i);

#endif /* Instruments_Inputs_hpp */
