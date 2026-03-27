//
//  CommandCreatedEventHandler.cpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#include "Fretboarder.h"

void OnExecuteEventHander::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    auto command = eventArgs->command();
    Ptr<CommandInputs> inputs = command->commandInputs();
    if (!inputs)
        return;

    // InstrumentFromInputs returns mm (calls scale(10) internally).
    auto instrument = InstrumentFromInputs(inputs);

    auto product = Fretboarder::app->activeProduct();
    auto design = product->cast<Design>();
    if (!design)
        return;

    // Custom features require a parametric (timeline) document.
    design->designType(ParametricDesignType);

    // If Custom Features API is available: create geometry first, then wrap it
    // in a CF node.  Using cfInput->setStartAndEndFeatures() before add() avoids
    // the slow timeline-reorganization that happens when grouping after the fact.
    if (Fretboarder::customFeatureDef) {
        auto cfFeatures = design->rootComponent()->features()->customFeatures();
        if (cfFeatures) {
            auto cfInput = cfFeatures->createInput(Fretboarder::customFeatureDef);
            if (cfInput) {
                InstrumentToCustomFeatureInput(cfInput, instrument, inputs);

                // Create geometry first — features appear at the end of the timeline.
                Ptr<Base> firstFeature, lastFeature;
                if (createFretboard(instrument, firstFeature, lastFeature)) {
                    if (firstFeature && lastFeature)
                        cfInput->setStartAndEndFeatures(firstFeature, lastFeature);

                    cfFeatures->add(cfInput);
                }
                return;
            }
        }
    }

    // Fallback: Custom Features API not available.
    Ptr<Base> firstFeature, lastFeature;
    createFretboard(instrument, firstFeature, lastFeature);
}

// CommandDestroyed event handler
void OnDestroyEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    // Do not terminate the add-in; it must stay loaded to handle events.
    (void)eventArgs;
}

// CommandCreated event handler.
void OnCommandCreatedEventHandler::notify(const Ptr<CommandCreatedEventArgs>& eventArgs)
{
    if (!eventArgs) return;

    Ptr<Command> command = eventArgs->command();
    if (!command) return;

    command->setDialogMinimumSize(300, 600);

    Ptr<CommandEvent> onDestroy = command->destroy();
    if (!onDestroy) return;
    if (!onDestroy->add(&onDestroyHandler)) return;

    Ptr<CommandEvent> onExecute = command->execute();
    if (!onExecute) return;
    if (!onExecute->add(&onExecuteHandler)) return;

    Ptr<CommandEvent> onExecutePreview = command->executePreview();
    if (!onExecutePreview) return;
    if (!onExecutePreview->add(&onExecutePreviewHandler)) return;

    Ptr<ValidateInputsEvent> onValidateInputs = command->validateInputs();
    if (!onValidateInputs) return;
    if (!onValidateInputs->add(&onValidateInputsHandler)) return;

    Ptr<InputChangedEvent> onInputChanged = command->inputChanged();
    if (!onInputChanged) return;
    if (!onInputChanged->add(&onInputChangedHandler)) return;

    Ptr<CommandInputs> inputs = command->commandInputs();
    if (!inputs) return;

    BuildFretboardDialogInputs(inputs);
}
