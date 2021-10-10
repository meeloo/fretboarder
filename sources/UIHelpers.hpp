//
//  UIHelpers.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#ifndef UIHelpers_hpp
#define UIHelpers_hpp

void progress(const Ptr<ProgressDialog>& dialog, const std::string& message);

class OnValidateInputsEventHander: public adsk::core::ValidateInputsEventHandler {
public:
    virtual void notify(const Ptr<ValidateInputsEventArgs>& eventArgs);
};

#endif /* UIHelpers_hpp */
