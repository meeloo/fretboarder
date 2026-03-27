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
                InstrumentToCustomFeatureInput(cfInput, instrument);

                // Create geometry first — features appear at the end of the timeline.
                Ptr<Base> firstFeature, lastFeature;
                if (createFretboard(instrument, firstFeature, lastFeature)) {
                    // Group the features into the CF input before calling add().
                    bool setOk = false;
                    if (firstFeature && lastFeature)
                        setOk = cfInput->setStartAndEndFeatures(firstFeature, lastFeature);

                    // Create the CF node — it absorbs the grouped features.
                    auto cf = cfFeatures->add(cfInput);

                    std::string cfErr;
                    Fretboarder::app->getLastError(&cfErr);
                    int innerCount = cf ? (int)cf->features().size() : -1;

                    // Check editCommandId from the CF's own definition (may differ from customFeatureDef)
                    std::string cfDefEid = "NO CF";
                    bool editCmdExists = false;
                    if (cf) {
                        auto def = cf->definition();
                        cfDefEid = def ? def->editCommandId() : "NO DEF";
                        auto editCmd = Fretboarder::ui->commandDefinitions()->itemById("editFretboard");
                        editCmdExists = (editCmd != nullptr);
                    }

                    std::stringstream dbg;
                    dbg << "setStartAndEndFeatures(cfInput): " << (setOk ? "OK" : "FAILED") << "\n"
                        << "cfFeatures->add: " << (cf ? "OK" : "FAILED") << "\n"
                        << "inner feature count: " << innerCount << "\n"
                        << "cf->definition()->editCommandId(): " << cfDefEid << "\n"
                        << "editFretboard cmd in commandDefs: " << (editCmdExists ? "YES" : "NO") << "\n"
                        << "lastError: " << cfErr;
                    Fretboarder::ui->messageBox(dbg.str());

                    if (cf) {
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
#define FAIL(msg) { Fretboarder::ui->messageBox("CommandCreated FAILED: " msg); return; }
    if (!eventArgs) FAIL("no eventArgs");

    Ptr<Command> command = eventArgs->command();
    if (!command) FAIL("no command");

    command->setDialogMinimumSize(300, 600);

    Ptr<CommandEvent> onDestroy = command->destroy();
    if (!onDestroy) FAIL("destroy event null");
    if (!onDestroy->add(&onDestroyHandler)) FAIL("add onDestroyHandler");

    Ptr<CommandEvent> onExecute = command->execute();
    if (!onExecute) FAIL("execute event null");
    if (!onExecute->add(&onExecuteHandler)) FAIL("add onExecuteHandler");

    Ptr<CommandEvent> onExecutePreview = command->executePreview();
    if (!onExecutePreview) FAIL("executePreview event null");
    if (!onExecutePreview->add(&onExecutePreviewHandler)) FAIL("add onExecutePreviewHandler");

    Ptr<ValidateInputsEvent> onValidateInputs = command->validateInputs();
    if (!onValidateInputs) FAIL("validateInputs event null");
    if (!onValidateInputs->add(&onValidateInputsHandler)) FAIL("add onValidateInputsHandler");

    Ptr<InputChangedEvent> onInputChanged = command->inputChanged();
    if (!onInputChanged) FAIL("inputChanged event null");
    if (!onInputChanged->add(&onInputChangedHandler)) FAIL("add onInputChangedHandler");

    Ptr<CommandInputs> inputs = command->commandInputs();
    if (!inputs) FAIL("commandInputs null");

    BuildFretboardDialogInputs(inputs);
#undef FAIL
}

