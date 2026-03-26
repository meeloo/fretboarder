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

// Builds all tab/input controls for the fretboard dialog (used by both
// the create and edit commands).
void BuildFretboardDialogInputs(const Ptr<CommandInputs>& inputs);

// Serialize an Instrument (in mm, post-scale) into a CustomFeatureInput's
// parameters so Fusion stores them as model parameters.
void InstrumentToCustomFeatureInput(const Ptr<CustomFeatureInput>& cfInput,
                                    const Instrument& instrument);

// Reconstruct an Instrument (in mm, ready for createFretboard) from an
// existing CustomFeature's stored parameters.
Instrument InstrumentFromCustomFeature(const Ptr<CustomFeature>& feature);

// Update the stored model parameters of an existing CustomFeature from an
// Instrument (in mm, post-scale).  Triggers a recompute.
void InstrumentToCustomFeature(const Ptr<CustomFeature>& feature,
                               const Instrument& instrument);

#endif /* Instruments_Inputs_hpp */
