//
//  Instruments+Inputs.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#ifndef Instruments_Inputs_hpp
#define Instruments_Inputs_hpp

// ---------------------------------------------------------------------------
// Param — canonical IDs for every command input and CustomFeature parameter.
// Use these everywhere instead of raw string literals so a typo is a
// compile-time error rather than a silent runtime failure.
// ---------------------------------------------------------------------------
namespace Param {
    // Length parameters (stored in CF and used as dialog input IDs)
    constexpr const char* scale_length_bass              = "scale_length_bass";
    constexpr const char* scale_length_treble            = "scale_length_treble";
    constexpr const char* inter_string_spacing_at_nut    = "inter_string_spacing_at_nut";
    constexpr const char* inter_string_spacing_at_bridge = "inter_string_spacing_at_bridge";
    constexpr const char* nut_to_zero_fret_offset        = "nut_to_zero_fret_offset";
    constexpr const char* hidden_tang_length             = "hidden_tang_length";
    constexpr const char* fret_slots_width               = "fret_slots_width";
    constexpr const char* fret_slots_height              = "fret_slots_height";
    constexpr const char* fret_crown_width               = "fret_crown_width";
    constexpr const char* fret_crown_height              = "fret_crown_height";
    constexpr const char* last_fret_cut_offset           = "last_fret_cut_offset";
    constexpr const char* space_before_nut               = "space_before_nut";
    constexpr const char* nut_thickness                  = "nut_thickness";
    constexpr const char* nut_height_under               = "nut_height_under";
    constexpr const char* radius_at_nut                  = "radius_at_nut";
    constexpr const char* radius_at_last_fret            = "radius_at_last_fret";
    constexpr const char* fretboard_thickness            = "fretboard_thickness";

    // Dimensionless parameters (stored in CF and used as dialog input IDs)
    constexpr const char* perpendicular_fret_index       = "perpendicular_fret_index";
    constexpr const char* number_of_strings              = "number_of_strings";
    constexpr const char* number_of_frets                = "number_of_frets";
    constexpr const char* overhang_type                  = "overhang_type";

    // Boolean parameters (stored in CF and used as dialog input IDs)
    constexpr const char* right_handed                   = "right_handed";
    constexpr const char* has_zero_fret                  = "has_zero_fret";
    constexpr const char* carve_nut_slot                 = "carve_nut_slot";
    constexpr const char* draw_strings                   = "draw_strings";
    constexpr const char* draw_frets                     = "draw_frets";
    constexpr const char* carve_fret_slots               = "carve_fret_slots";

    // CF-only overhang parameter IDs (stored in CF; different from dialog input names)
    constexpr const char* overhangs_0                    = "overhangs_0";
    constexpr const char* overhangs_1                    = "overhangs_1";
    constexpr const char* overhangs_2                    = "overhangs_2";
    constexpr const char* overhangs_3                    = "overhangs_3";

    // Dialog-only input IDs (not stored in CF)
    constexpr const char* overhangSingle                 = "overhangSingle";
    constexpr const char* overhangNut                    = "overhangNut";
    constexpr const char* overhangLast                   = "overhangLast";
    constexpr const char* overhang0                      = "overhang0";
    constexpr const char* overhang1                      = "overhang1";
    constexpr const char* overhang2                      = "overhang2";
    constexpr const char* overhang3                      = "overhang3";
    constexpr const char* nut_width                      = "nut_width";
} // namespace Param

Instrument InstrumentFromInputs(const Ptr<CommandInputs>& inputs);
void InstrumentToInputs(const Ptr<CommandInputs>& inputs, const Instrument& i);

// Builds all tab/input controls for the fretboard dialog (used by both
// the create and edit commands).
void BuildFretboardDialogInputs(const Ptr<CommandInputs>& inputs);

// Serialize an Instrument (in mm, post-scale) into a CustomFeatureInput's
// parameters.  When `inputs` is provided the dialog's expression strings are
// stored verbatim so that user-parameter references (e.g. "myScale") are
// preserved in the model parameter table and drive auto-recompute.
void InstrumentToCustomFeatureInput(const Ptr<CustomFeatureInput>& cfInput,
                                    const Instrument& instrument,
                                    const Ptr<CommandInputs>& inputs = nullptr);

// Reconstruct an Instrument (in mm, ready for createFretboard) from an
// existing CustomFeature's stored parameters.
Instrument InstrumentFromCustomFeature(const Ptr<CustomFeature>& feature);

// Update the stored model parameters of an existing CustomFeature from an
// Instrument (in mm, post-scale).  Triggers a recompute.
void InstrumentToCustomFeature(const Ptr<CustomFeature>& feature,
                               const Instrument& instrument);

// After calling InstrumentToInputs() to populate boolean/integer fields, call
// this to restore the expression strings for all float fields from the CF's
// stored model parameters.  This preserves user-parameter references (e.g.
// "myScale") in the edit dialog.
void CfExpressionsToInputs(const Ptr<CommandInputs>& inputs,
                           const Ptr<CustomFeature>& cf);

#endif /* Instruments_Inputs_hpp */
