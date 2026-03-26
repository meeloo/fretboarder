//
//  CustomFeatureHandler.cpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#include "Fretboarder.h"

// ---------------------------------------------------------------------------
// CustomFeatureComputeEventHandler
//
// Called by Fusion when a Fretboard custom feature needs to be recomputed
// (e.g. the user edited parameters or an upstream dependency changed).
//
// Strategy:
//   1. Read the current instrument parameters from the custom feature.
//   2. Delete the Fretboard Occurrence that was previously created (this
//      cascades to all geometry inside it).
//   3. Recreate the fretboard with the current parameters.
//   4. Re-group the new geometry under the custom feature.
// ---------------------------------------------------------------------------
void CustomFeatureComputeEventHandler::notify(const Ptr<CustomFeatureEventArgs>& eventArgs)
{
    auto cf = eventArgs->customFeature();
    if (!cf)
        return;

    // Roll the timeline to just before this custom feature so that any new
    // geometry is inserted at the correct position.
    auto tlo = cf->timelineObject();
    if (tlo)
        tlo->rollTo(true);

    // Get the component that owns this custom feature.
    auto comp = cf->parentComponent();
    if (!comp)
        return;

    // Reconstruct instrument from the stored parameters (returned in mm).
    auto instrument = InstrumentFromCustomFeature(cf);

    // Recreate the fretboard geometry inside the owning component.
    Ptr<Base> firstFeature, lastFeature;
    if (!createFretboard(instrument, firstFeature, lastFeature, comp))
        return;

    // Re-group the new geometry under this custom feature.
    if (firstFeature && lastFeature)
        cf->setStartAndEndFeatures(firstFeature, lastFeature);
}

// ---------------------------------------------------------------------------
// OnEditCommandCreatedEventHandler
//
// Invoked when the user double-clicks a Fretboard custom feature in the
// timeline.  Builds the dialog pre-populated with the current parameters.
// ---------------------------------------------------------------------------
void OnEditCommandCreatedEventHandler::notify(const Ptr<CommandCreatedEventArgs>& eventArgs)
{
    if (!eventArgs)
        return;

    auto command = eventArgs->command();
    if (!command)
        return;

    command->setDialogMinimumSize(300, 600);

    // Wire up sub-handlers.
    auto onExecute = command->execute();
    if (!onExecute) return;
    onExecute->add(&onEditExecuteHandler);

    auto onDestroy = command->destroy();
    if (!onDestroy) return;
    onDestroy->add(&onEditDestroyHandler);

    auto onExPreview = command->executePreview();
    if (!onExPreview) return;
    onExPreview->add(&onExecutePreviewHandler);

    auto onValidate = command->validateInputs();
    if (!onValidate) return;
    onValidate->add(&onValidateInputsHandler);

    auto onInputChanged = command->inputChanged();
    if (!onInputChanged) return;
    onInputChanged->add(&onInputChangedHandler);

    // Build all the dialog controls.
    auto inputs = command->commandInputs();
    if (!inputs) return;
    BuildFretboardDialogInputs(inputs);

    // Find the rolled-back custom feature and pre-populate the controls.
    auto product = Fretboarder::app->activeProduct();
    if (!product) return;
    auto design = product->cast<Design>();
    if (!design) return;

    auto cfCollection = design->rootComponent()->features()->customFeatures();
    if (!cfCollection) return;

    Ptr<CustomFeature> targetFeature;
    for (size_t i = 0; i < cfCollection->count(); ++i) {
        auto cf = cfCollection->item(i);
        if (!cf) continue;
        auto def = cf->definition();
        if (!def) continue;
        if (def->id() != "Fretboarder.Fretboard") continue;
        auto tlo = cf->timelineObject();
        if (tlo && tlo->isRolledBack()) {
            targetFeature = cf;
            break;
        }
    }

    if (!targetFeature) return;

    // InstrumentFromCustomFeature returns instrument in mm.
    // InstrumentToInputs expects cm (internal Fusion units), so scale by 0.1.
    auto instrument = InstrumentFromCustomFeature(targetFeature);
    instrument.scale(0.1);
    InstrumentToInputs(inputs, instrument);
}

// ---------------------------------------------------------------------------
// OnEditExecuteEventHandler
//
// Called when the user clicks OK in the edit dialog.  Updates the stored
// parameters on the custom feature, which triggers a recompute.
// ---------------------------------------------------------------------------
void OnEditExecuteEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    auto command = eventArgs->command();
    if (!command) return;

    auto inputs = command->commandInputs();
    if (!inputs) return;

    // InstrumentFromInputs calls scale(10) internally → result is in mm.
    auto instrument = InstrumentFromInputs(inputs);

    // Find the rolled-back custom feature.
    auto product = Fretboarder::app->activeProduct();
    if (!product) return;
    auto design = product->cast<Design>();
    if (!design) return;

    auto cfCollection = design->rootComponent()->features()->customFeatures();
    if (!cfCollection) return;

    for (size_t i = 0; i < cfCollection->count(); ++i) {
        auto cf = cfCollection->item(i);
        if (!cf) continue;
        auto def = cf->definition();
        if (!def) continue;
        if (def->id() != "Fretboarder.Fretboard") continue;
        auto tlo = cf->timelineObject();
        if (tlo && tlo->isRolledBack()) {
            // Update parameters — Fusion will fire customFeatureCompute.
            InstrumentToCustomFeature(cf, instrument);
            return;
        }
    }
}

// ---------------------------------------------------------------------------
// OnEditDestroyEventHandler  — no-op; add-in must stay loaded.
// ---------------------------------------------------------------------------
void OnEditDestroyEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    // Intentionally empty — do NOT call adsk::terminate().
}
