//
//  OnExecutePreviewEventHandler.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#ifndef OnExecutePreviewEventHandler_hpp
#define OnExecutePreviewEventHandler_hpp

class OnExecutePreviewEventHandler : public adsk::core::CommandEventHandler
{
public:
    void notify(const Ptr<CommandEventArgs>& eventArgs);
    void applyLinesProperties(Ptr<CustomGraphicsLines> cgLines);
};


#endif /* OnExecutePreviewEventHandler_hpp */
