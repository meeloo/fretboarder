#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include "Fretboard.hpp"

using namespace adsk::core;
using namespace adsk::fusion;

Ptr<Application> app;

extern "C" XI_EXPORT bool run(const char* context)
{
    app = Application::get();
    if (!app)
        return false;

    Ptr<Document> doc = app->activeDocument();
    if (!doc)
        return false;

    Ptr<Product> product = app->activeProduct();
    if (!product)
        return false;

    Ptr<Design> design = product;
    if (!design)
        return false;

    // Get the root component of the active design
    Ptr<Component> rootComp = design->rootComponent();
    if(!rootComp)
        return false;

    fretboarder::Instrument instrument;
    fretboarder::Fretboard fretboard(instrument);
    
    // Create a new sketch on the xy plane.
    Ptr<Sketches> sketches = rootComp->sketches();
    if(!sketches)
        return false;
    Ptr<ConstructionPlane> xyPlane = rootComp->xYConstructionPlane();
    if(!xyPlane)
        return false;
    Ptr<Sketch> sketch = sketches->add(xyPlane);
    if(!sketch)
        return false;

    
    Ptr<SketchCurves> sketchCurves = sketch->sketchCurves();
    if(!sketchCurves)
        return false;
    Ptr<SketchLines> sketchLines = sketchCurves->sketchLines();
    if(!sketchLines)
        return false;
    
    printf("Fretboarder online\n");
    for (int i = 0; i < instrument.number_of_frets; i++) {
        auto fret = fretboard.fret_slots().at(i);
        printf("\tfret %d\n", i);
//        app->userInterface()->messageBox("fret");

        Ptr<SketchLine> line = sketchLines->addByTwoPoints(Point3D::create(fret.point1.x * 0.1, fret.point1.y * 0.1, 0), Point3D::create(fret.point2.x * 0.1, fret.point2.y * 0.1, 0));
    }

    auto strings = fretboard.strings();
    for (int i = 0; i < strings.size(); i++) {
        auto s = strings.at(i);
        sketchLines->addByTwoPoints(Point3D::create(s.x_at_start() * 0.1, s.y_at_start() * 0.1, 0), Point3D::create(s.x_at_bridge() * 0.1, s.y_at_bridge() * 0.1, 0));
    }
    printf("Fretboarder done\n");

    if (0)
    {
        // Draw two connected lines.
        Ptr<SketchLine> line1 = sketchLines->addByTwoPoints(Point3D::create(0, 0, 0), Point3D::create(3, 1, 0));
        if(!line1)
            return false;
        Ptr<SketchLine> line2 = sketchLines->addByTwoPoints(line1->endSketchPoint(), Point3D::create(1, 4, 0));
        if(!line2)
            return false;

        // Draw a rectangle by two points.
        Ptr<SketchLineList> recLines = sketchLines->addTwoPointRectangle(Point3D::create(4, 0, 0), Point3D::create(7, 2, 0));
        if(!recLines)
            return false;

        // Use the returned lines to add some constraints.
        Ptr<GeometricConstraints> constraints = sketch->geometricConstraints();
        if(!constraints)
            return false;

        Ptr<HorizontalConstraint> HConstraint = constraints->addHorizontal(recLines->item(0));
        if(!HConstraint)
            return false;
        HConstraint = constraints->addHorizontal(recLines->item(2));
        if(!HConstraint)
            return false;

        Ptr<VerticalConstraint> VConstraint = constraints->addVertical(recLines->item(1));
        if(!VConstraint)
            return false;
        VConstraint = constraints->addVertical(recLines->item(3));
        if(!VConstraint)
            return false;

        Ptr<SketchDimensions> sketchDimensions = sketch->sketchDimensions();
        if(!sketchDimensions)
            return false;
        Ptr<SketchDimension> sketchDimension = sketchDimensions->addDistanceDimension(recLines->item(0)->startSketchPoint(), recLines->item(0)->endSketchPoint(), HorizontalDimensionOrientation, Point3D::create(5.5, -1, 0));
        if(!sketchDimension)
            return false;

        // Draw a rectangle by three points.
        recLines = sketchLines->addThreePointRectangle(Point3D::create(8, 0, 0), Point3D::create(11, 1, 0), Point3D::create(9, 3, 0));
        if(!recLines)
            return false;

        // Draw a rectangle by a center point.
        recLines = sketchLines->addCenterPointRectangle(Point3D::create(14, 3, 0), Point3D::create(16, 4, 0));
        if(!recLines)
            return false;
    }

    return true;
}

#ifdef XI_WIN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#endif // XI_WIN
