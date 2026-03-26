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
// Geometry is created in the execute/edit handlers; this handler stays
// resident so the add-in can service compute requests after reload.
// ---------------------------------------------------------------------------
void CustomFeatureComputeEventHandler::notify(const Ptr<CustomFeatureEventArgs>& eventArgs)
{
    // Geometry is managed by execute handlers — nothing to do here.
    (void)eventArgs;
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
// Called when the user clicks OK in the edit dialog.  Removes the existing
// timeline group, deletes old inner features, recreates geometry, then wraps
// everything in a new timeline group.
// ---------------------------------------------------------------------------
void OnEditExecuteEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    auto command = eventArgs->command();
    if (!command) return;

    auto inputs = command->commandInputs();
    if (!inputs) return;

    auto instrument = InstrumentFromInputs(inputs);

    auto product = Fretboarder::app->activeProduct();
    if (!product) return;
    auto design = product->cast<Design>();
    if (!design) return;

    auto cfCollection = design->rootComponent()->features()->customFeatures();
    if (!cfCollection) return;

    Ptr<CustomFeature> targetCF;
    for (size_t i = 0; i < cfCollection->count(); ++i) {
        auto cf = cfCollection->item(i);
        if (!cf) continue;
        auto def = cf->definition();
        if (!def || def->id() != "Fretboarder.Fretboard") continue;
        auto tlo = cf->timelineObject();
        if (tlo && tlo->isRolledBack()) { targetCF = cf; break; }
    }
    if (!targetCF) return;

    auto cfTlo    = targetCF->timelineObject();
    auto timeline = design->timeline();
    if (!timeline || !cfTlo) return;

    // Read the stored inner feature count.
    int innerCount = 0;
    Ptr<Attribute> countAttr;
    auto attrs = targetCF->attributes();
    if (attrs) {
        countAttr = attrs->itemByName("Fretboarder", "innerCount");
        if (countAttr) innerCount = std::stoi(countAttr->value());
    }

    // Roll TO the CF node so new features land after it.
    cfTlo->rollTo(false);
    int cfIndex = (int)cfTlo->index();

    // Delete old inner features high-to-low.
    for (int i = cfIndex + innerCount; i > cfIndex; i--) {
        auto innerTlo = timeline->item(i);
        if (!innerTlo || !innerTlo->isValid()) continue;
        auto entity = innerTlo->entity();
        if (!entity) continue;
        if (auto feat  = entity->cast<Feature>())                feat->deleteMe();
        else if (auto sk    = entity->cast<Sketch>())            sk->deleteMe();
        else if (auto plane = entity->cast<ConstructionPlane>()) plane->deleteMe();
    }

    // Recreate geometry after the CF node.
    int beforeCount = (int)timeline->count();
    Ptr<Base> firstFeature, lastFeature;
    if (!createFretboard(instrument, firstFeature, lastFeature))
        return;

    int newInnerCount = (int)timeline->count() - beforeCount;

    // Update the stored inner count.
    if (countAttr)
        countAttr->value(std::to_string(newInnerCount));
    else if (attrs)
        attrs->add("Fretboarder", "innerCount", std::to_string(newInnerCount));

    // Re-group inner features under the CF node.
    if (firstFeature && lastFeature)
        targetCF->setStartAndEndFeatures(firstFeature, lastFeature);

    // Persist updated parameters.
    InstrumentToCustomFeature(targetCF, instrument);
}

// ---------------------------------------------------------------------------
// OnEditDestroyEventHandler  — no-op; add-in must stay loaded.
// ---------------------------------------------------------------------------
void OnEditDestroyEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    // Intentionally empty — do NOT call adsk::terminate().
}
