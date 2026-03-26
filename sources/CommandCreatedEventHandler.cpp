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

    auto instrument = InstrumentFromInputs(inputs);

    Ptr<Base> firstFeature, lastFeature;
    if (!createFretboard(instrument, firstFeature, lastFeature))
        return;

    if (!Fretboarder::customFeatureDef || !firstFeature || !lastFeature)
        return;

    auto product = Fretboarder::app->activeProduct();
    auto design = product->cast<Design>();
    if (!design)
        return;

    auto cfFeatures = design->rootComponent()->features()->customFeatures();
    if (!cfFeatures)
        return;

    auto cfInput = cfFeatures->createInput(Fretboarder::customFeatureDef);
    if (!cfInput)
        return;

    InstrumentToCustomFeatureInput(cfInput, instrument);
    cfInput->setStartAndEndFeatures(firstFeature, lastFeature);

    design->rootComponent()->features()->customFeatures()->add(cfInput);
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

