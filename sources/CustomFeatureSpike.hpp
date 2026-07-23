//
//  CustomFeatureSpike.hpp
//  Fretboarder
//
//  Throwaway spike: the smallest possible *editable* Fusion Custom Feature (a
//  parametric box) used to prove the create -> timeline node -> double-click
//  edit -> recompute lifecycle works BEFORE porting the real fretboard geometry
//  onto it. The whole point is to find out whether editCommandId actually binds
//  double-click to our edit command in this Fusion build. Delete once proven.
//

#ifndef CustomFeatureSpike_hpp
#define CustomFeatureSpike_hpp

// Registers the box custom-feature definition + create/edit commands and wires
// the compute handler. Call from run(). Returns false (after showing an error)
// if any step fails; the editCommandId result is reported to the user.
bool register_custom_feature_spike();

// Tears down the commands/controls created by register_custom_feature_spike().
// Call from stop().
void unregister_custom_feature_spike();

#endif /* CustomFeatureSpike_hpp */
