//
//  CustomFeatureHandler.cpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#include "Fretboarder.h"

// The custom feature currently being edited.  Set in commandCreated, used in
// execute, cleared when done.
static Ptr<CustomFeature>   gEditedCF;

// ---------------------------------------------------------------------------
// CustomFeatureComputeEventHandler
//
// Geometry is managed by the execute/edit handlers (create and edit flows).
// This handler is intentionally a no-op — it exists only so the add-in stays
// resident and can service any Fusion-internal recompute requests.
// ---------------------------------------------------------------------------
void CustomFeatureComputeEventHandler::notify(const Ptr<CustomFeatureEventArgs>& eventArgs)
{
    (void)eventArgs;
}

// ---------------------------------------------------------------------------
// OnEditCommandCreatedEventHandler
//
// Invoked when the user double-clicks a Fretboard custom feature in the
// timeline.  Gets the CF being edited from the active selection, builds the
// dialog, and pre-populates the controls from the stored parameters.
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
    auto onActivate = command->activate();
    if (!onActivate) return;
    onActivate->add(&onEditActivateHandler);

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

    // Build dialog controls.
    auto inputs = command->commandInputs();
    if (!inputs) return;
    BuildFretboardDialogInputs(inputs);

    // Fusion selects the CF node before firing the edit command.
    // Get it from the active selections rather than searching for isRolledBack().
    gEditedCF = nullptr;
    auto selections = Fretboarder::ui->activeSelections();
    if (selections && selections->count() > 0) {
        auto sel = selections->item(0);
        if (sel) {
            auto cf = sel->entity()->cast<CustomFeature>();
            if (cf && cf->definition() && cf->definition()->id() == "Fretboarder.Fretboard")
                gEditedCF = cf;
        }
    }

    if (!gEditedCF)
        return;

    // Pre-populate the dialog.  InstrumentFromCustomFeature returns mm;
    // InstrumentToInputs expects cm (Fusion internal units).
    auto instrument = InstrumentFromCustomFeature(gEditedCF);
    instrument.scale(0.1);
    InstrumentToInputs(inputs, instrument);
    CfExpressionsToInputs(inputs, gEditedCF);
}

// ---------------------------------------------------------------------------
// OnEditActivateEventHandler
//
// The delete-and-recreate edit strategy doesn't need timeline rollback.
// The timeline must stay at the end so the CF is active when execute fires.
// ---------------------------------------------------------------------------
void OnEditActivateEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    (void)eventArgs;
}

// ---------------------------------------------------------------------------
// OnEditExecuteEventHandler
//
// Called when the user clicks OK.  Detaches inner features from the CF,
// deletes them, recreates geometry with the new parameters, re-groups the
// new features under the CF, and restores the timeline marker.
// ---------------------------------------------------------------------------
void OnEditExecuteEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    if (!gEditedCF) return;

    auto command = eventArgs->command();
    if (!command) return;

    auto inputs = command->commandInputs();
    if (!inputs) return;

    auto instrument = InstrumentFromInputs(inputs);

    auto product = Fretboarder::app->activeProduct();
    if (!product) return;
    auto design = product->cast<Design>();
    if (!design) return;

    // Snapshot inner features before deleting the CF.
    // (deleteMe may or may not cascade-delete inner features.)
    auto oldInnerFeatures = gEditedCF->features();

    // Delete the existing CF.  This is the only reliable path since
    // setStartAndEndFeatures on an existing CF requires Fusion's internal
    // "Edit Feature" mode which we can't trigger (editCommandId setter fails).
    gEditedCF->deleteMe();
    gEditedCF = nullptr;

    // Delete any inner features that weren't cascade-deleted with the CF.
    for (int i = (int)oldInnerFeatures.size() - 1; i >= 0; --i) {
        auto entity = oldInnerFeatures[i];
        if (!entity) continue;
        if (auto feat  = entity->cast<Feature>())                feat->deleteMe();
        else if (auto sk    = entity->cast<Sketch>())            sk->deleteMe();
        else if (auto plane = entity->cast<ConstructionPlane>()) plane->deleteMe();
    }

    // Recreate using the same geometry-first approach as the create flow.
    auto cfFeatures = design->rootComponent()->features()->customFeatures();
    if (!cfFeatures) return;

    auto cfInput = cfFeatures->createInput(Fretboarder::customFeatureDef);
    if (!cfInput) return;

    InstrumentToCustomFeatureInput(cfInput, instrument, inputs);

    Ptr<Base> firstFeature, lastFeature;
    if (!createFretboard(instrument, firstFeature, lastFeature)) return;

    if (firstFeature && lastFeature)
        cfInput->setStartAndEndFeatures(firstFeature, lastFeature);

    cfFeatures->add(cfInput);
}

// ---------------------------------------------------------------------------
// OnEditDestroyEventHandler  — no-op; add-in must stay loaded.
// ---------------------------------------------------------------------------
void OnEditDestroyEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    (void)eventArgs;
    gEditedCF = nullptr;
}

