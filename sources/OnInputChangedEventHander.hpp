//
//  OnInputChangedEventHander.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#ifndef OnInputChangedEventHander_hpp
#define OnInputChangedEventHander_hpp

// InputChange event handler.
class OnInputChangedEventHander : public adsk::core::InputChangedEventHandler
{
public:
    void notify(const Ptr<InputChangedEventArgs>& eventArgs) override;
    void UpdateNutWidget(const Ptr<CommandInputs>& inputs);
};


#endif /* OnInputChangedEventHander_hpp */
