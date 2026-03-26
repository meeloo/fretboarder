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

    // If Custom Features API is available: create geometry first, then create
    // the CF node that wraps it using cfInput->setStartAndEndFeatures before add().
    if (Fretboarder::customFeatureDef) {
        auto cfFeatures = design->rootComponent()->features()->customFeatures();
        if (cfFeatures) {
            auto cfInput = cfFeatures->createInput(Fretboarder::customFeatureDef);
            if (cfInput) {
                InstrumentToCustomFeatureInput(cfInput, instrument);

                auto timeline    = design->timeline();
                int  beforeCount = timeline ? (int)timeline->count() : 0;

                // Create geometry first — features land in the timeline before the CF node.
                Ptr<Base> firstFeature, lastFeature;
                if (createFretboard(instrument, firstFeature, lastFeature)) {
                    int innerCount = (int)timeline->count() - beforeCount;

                    // Tell the CF input which features it owns BEFORE calling add().
                    if (firstFeature && lastFeature)
                        cfInput->setStartAndEndFeatures(firstFeature, lastFeature);

                    // Now create the CF node — it wraps the already-existing features.
                    auto cf = cfFeatures->add(cfInput);
                    if (cf) {
                        // Store innerCount for the edit handler.
                        auto attrs = cf->attributes();
                        if (attrs)
                            attrs->add("Fretboarder", "innerCount", std::to_string(innerCount));

                        // Save parameters to the CF for future edits.
                        InstrumentToCustomFeature(cf, instrument);
                    }
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
    // Do not terminate the add-in; it must stay loaded to handle customFeatureCompute events.
}

// CommandCreated event handler.
void OnCommandCreatedEventHandler::notify(const Ptr<CommandCreatedEventArgs>& eventArgs)
{
    if (eventArgs)
    {
        // Get the command that was created.
        Ptr<Command> command = eventArgs->command();
        if (command)
        {
            command->setDialogMinimumSize(300, 600);

            // Connect to the command destroyed event.
            Ptr<CommandEvent> onDestroy = command->destroy();
            if (!onDestroy)
                return;
            bool isOk = onDestroy->add(&onDestroyHandler);
            if (!isOk)
                return;

            // Connect to the command execute event.
            Ptr<CommandEvent> onExecute = command->execute();
            if (!onExecute)
                return;
            isOk = onExecute->add(&onExecuteHandler);
            if (!isOk)
                return;

            Ptr<CommandEvent> onExecutePtrview = command->executePreview();
            if (!onExecutePtrview)
                return;
            isOk = onExecutePtrview->add(&onExecutePreviewHandler);
            if (!isOk)
                return;

            Ptr<ValidateInputsEvent> onValidateInputs = command->validateInputs();
            if (!onValidateInputs)
                return;
            isOk = onValidateInputs->add(&onValidateInputsHandler);
            if (!isOk)
                return;


            // Connect to the input changed event.
            Ptr<InputChangedEvent> onInputChanged = command->inputChanged();
            if (!onInputChanged)
                return;
            isOk = onInputChanged->add(&onInputChangedHandler);
            if (!isOk)
                return;

            // Get the CommandInputs collection associated with the command.
            Ptr<CommandInputs> inputs = command->commandInputs();
            if (!inputs)
                return;

            BuildFretboardDialogInputs(inputs);
        }
    }
}

