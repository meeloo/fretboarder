//
//  CustomFeatureHandler.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#ifndef CustomFeatureHandler_hpp
#define CustomFeatureHandler_hpp

// Set to true by the execute handler before calling cfFeatures->add() so the
// compute handler knows geometry was already created and should not be recreated.
extern bool gSkipNextCompute;

// Handles recomputation of a Fretboard custom feature (delete old geometry + recreate)
class CustomFeatureComputeEventHandler : public adsk::fusion::CustomFeatureEventHandler
{
public:
    void notify(const Ptr<CustomFeatureEventArgs>& eventArgs) override;
};

// Execute handler for the edit command
class OnEditExecuteEventHandler : public adsk::core::CommandEventHandler
{
public:
    void notify(const Ptr<CommandEventArgs>& eventArgs) override;
};

// Destroy handler for the edit command (no-op, add-in stays loaded)
class OnEditDestroyEventHandler : public adsk::core::CommandEventHandler
{
public:
    void notify(const Ptr<CommandEventArgs>& eventArgs) override;
};

// CommandCreated handler for the edit command (double-click in timeline)
class OnEditCommandCreatedEventHandler : public adsk::core::CommandCreatedEventHandler
{
public:
    void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override;

private:
    OnEditExecuteEventHandler    onEditExecuteHandler;
    OnEditDestroyEventHandler    onEditDestroyHandler;
    OnInputChangedEventHander    onInputChangedHandler;
    OnValidateInputsEventHander  onValidateInputsHandler;
    OnExecutePreviewEventHandler onExecutePreviewHandler;
};

#endif /* CustomFeatureHandler_hpp */
