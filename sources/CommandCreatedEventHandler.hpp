//
//  CommandCreatedEventHandler.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#ifndef CommandCreatedEventHandler_hpp
#define CommandCreatedEventHandler_hpp

class OnExecuteEventHander : public adsk::core::CommandEventHandler
{
public:
    void notify(const Ptr<CommandEventArgs>& eventArgs) override;
};

// CommandDestroyed event handler
class OnDestroyEventHandler : public adsk::core::CommandEventHandler
{
public:
    void notify(const Ptr<CommandEventArgs>& eventArgs) override;
};

// CommandCreated event handler.
class OnCommandCreatedEventHandler : public adsk::core::CommandCreatedEventHandler
{
public:
    void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override;

private:
    OnExecuteEventHander onExecuteHandler;
    OnDestroyEventHandler onDestroyHandler;
    OnInputChangedEventHander onInputChangedHandler;
    OnValidateInputsEventHander onValidateInputsHandler;
    OnExecutePreviewEventHandler onExecutePreviewHandler;
};


#endif /* CommandCreatedEventHandler_hpp */
