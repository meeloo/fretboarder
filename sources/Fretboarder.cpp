#include "Fretboarder.h"

Ptr<Application> Fretboarder::app;
Ptr<UserInterface> Fretboarder::ui;
Ptr<CustomGraphicsGroups> Fretboarder::cgGroups;
OnCommandCreatedEventHandler _cmdCreatedHandler;

Ptr<BRepFace> getFretboardTopSurface(const Ptr<Component>& component) {
    CHECK(component, nullptr);
    auto entities = component->findBRepUsingRay(Point3D::create(1, 0, 1), Vector3D::create(0, 0, -1), BRepFaceEntityType);
    CHECK(entities, nullptr);
    return entities->count() > 0 ? entities->item(0) : nullptr;
}

Ptr<SweepFeature> create_fret_element(const Ptr<Sketch>& profile, const Ptr<Path>& path, const Ptr<Component>& component, const std::string& name) {
    CHECK(component, nullptr);
    CHECK(profile, nullptr);
    auto p = profile->profiles()->item(0);
    CHECK(p, nullptr);
    auto input = component->features()->sweepFeatures()->createInput(p, path, NewBodyFeatureOperation);
    CHECK(input, nullptr);
    input->orientation(SweepOrientationTypes::PerpendicularOrientationType);
    CHECK(input, nullptr);
    auto fret = component->features()->sweepFeatures()->add(input);
    CHECK(fret, nullptr);

    auto bodies = fret->bodies();
    CHECK(bodies, nullptr);
    
    CHECK(bodies->count() >= 1, nullptr);
    for (int j = 0; j < bodies->count(); j++) {
        auto body = bodies->item(j);
        CHECK(body, nullptr);
        body->name(name);
    }

    return fret;
}

bool createFretboard(const fretboarder::Instrument& instrument) {
    Ptr<Document> doc = Fretboarder::app->activeDocument();
    CHECK(doc, false);

    Ptr<Product> product = Fretboarder::app->activeProduct();
    CHECK(product, false);

    Ptr<Design> design = product;
    CHECK(design, false);

    Ptr<Timeline> timeline = design->timeline();
    CHECK(timeline, false);

    Ptr<TimelineGroups> timelineGroups = timeline->timelineGroups();
    CHECK(timelineGroups, false);

    // Get the root component of the active design
    Ptr<Component> rootComp = design->rootComponent();
    CHECK(rootComp, false);

    auto fretboardCreationStartIndex = timeline->markerPosition();

    fretboarder::Fretboard fretboard(instrument);

    auto progressDialog = Fretboarder::ui->createProgressDialog();
    progressDialog->isCancelButtonShown(false);
    //progressDialog->isBackgroundTranslucent(true);
    progressDialog->show("creating fretboard", "", 0, 3 + instrument.number_of_strings + instrument.number_of_frets);

    progress(progressDialog, "create fretboard plank");
    // create Fretboard component
    auto occurrence = rootComp->occurrences()->addNewComponent(Matrix3D::create());
    CHECK(occurrence, false);
    auto component = occurrence->component();
    CHECK(component, false);
    component->name("Fretboard");
    occurrence->activate();
//    design->designType(DirectDesignType);
    design->designType(ParametricDesignType);

    // create strings sketch
    auto strings_area_sketch = component->sketches()->add(component->xYConstructionPlane());
    CHECK(strings_area_sketch, false);
    strings_area_sketch->name("Strings area");
    strings_area_sketch->isComputeDeferred(true);
    create_closed_polygon(strings_area_sketch->sketchCurves()->sketchLines(), fretboard.strings_shape());
    strings_area_sketch->isComputeDeferred(false);
    strings_area_sketch->isVisible(false);
    
    
    if (instrument.draw_strings) {
        auto strings_sketch = component->sketches()->add(component->xYConstructionPlane());
        CHECK(strings_sketch, false);
        strings_sketch->name("Strings");
        strings_sketch->isComputeDeferred(true);
        {
            auto radius1 = instrument.radius_at_nut ;
            //  Find radius at bridge
            // TODO: not perfect, should be per string and with the correct ratio for the actual last fret
            double b = instrument.radius_at_last_fret / 1.5; // Scale radius at last fret to find radius at 12th fret
            auto radius2 = instrument.radius_at_nut + (b - instrument.radius_at_nut) * 2; // scale to get radius at bridge
            auto center = Point(0.0, 0.0, 0.0);
            
            for (size_t s = 0; s < fretboard.strings().size(); s++) {
                std::stringstream str;
                str << "create string " << (int)s;
                progress(progressDialog, str.str());
                const auto& string = fretboard.strings()[s];
                auto nutSide = string.point_at_nut();
                auto bridgeSide = string.point_at_bridge();
                
                auto offset1 = YOnCircleGivenX(nutSide.y, center, radius1);
                auto offset2 = YOnCircleGivenX(bridgeSide.y, center, radius2);
                
                nutSide.z += (offset1 + radius1 + instrument.fretboard_thickness) + instrument.fret_crown_height;
                bridgeSide.z += (offset2 + radius2 + instrument.fretboard_thickness) + instrument.fret_crown_height + 4;
                auto v = Vector(nutSide, bridgeSide);
                create_line(strings_sketch->sketchCurves()->sketchLines(), v);
            }
        }
        //create_closed_polygon(strings_sketch->sketchCurves()->sketchLines(), fretboard.strings_shape());
        strings_sketch->isComputeDeferred(false);
    }
    
    // create Contour sketch
    auto contour_sketch = component->sketches()->add(component->xYConstructionPlane());
    CHECK(contour_sketch, false);
    contour_sketch->name("Contour");
    contour_sketch->isComputeDeferred(true);
    create_closed_polygon(contour_sketch->sketchCurves()->sketchLines(), fretboard.board_shape());
    contour_sketch->isComputeDeferred(false);
    
    // create fret slots as lines sketch
    auto planes = component->constructionPlanes();
    CHECK(planes, false);
    auto planeInput = planes->createInput();
    CHECK(planeInput, false);
    auto offsetValue = ValueInput::createByReal((instrument.fretboard_thickness + 10.0) * 0.1);
    planeInput->setByOffset(component->xYConstructionPlane(), offsetValue);
    auto fret_slots_construction_plane = planes->add(planeInput);
    CHECK(fret_slots_construction_plane, false);
    auto fret_slots_sketch = component->sketches()->add(fret_slots_construction_plane);
    CHECK(fret_slots_sketch, false);
    fret_slots_sketch->name("Fret slots as lines");
    fret_slots_sketch->isComputeDeferred(true);
    for (auto &&vector : fretboard.fret_slots()) {
        create_line(fret_slots_sketch->sketchCurves()->sketchLines(), vector);
    }
    fret_slots_sketch->isComputeDeferred(false);
    fret_slots_sketch->isVisible(false);

    auto fret_lines_sketch = component->sketches()->add(fret_slots_construction_plane);
    CHECK(fret_lines_sketch, false);
    fret_lines_sketch->name("Fret lines as lines");
    fret_lines_sketch->isComputeDeferred(true);
    for (auto &&vector : fretboard.fret_lines()) {
        create_line(fret_lines_sketch->sketchCurves()->sketchLines(), vector);
    }
    fret_lines_sketch->isComputeDeferred(false);
    fret_lines_sketch->isVisible(false);

    // create construction plane at nut side
    planeInput = planes->createInput();
    CHECK(planeInput, false);
    offsetValue = ValueInput::createByReal(fretboard.construction_distance_at_nut_side() * 0.1);
    CHECK(offsetValue, false);
    planeInput->setByOffset(component->yZConstructionPlane(), offsetValue);
    auto construction_plane_at_nut_side = planes->add(planeInput);
    CHECK(construction_plane_at_nut_side, false);
    construction_plane_at_nut_side->name("Nut Side");
    
    // create construction plane at nut
    planeInput = planes->createInput();
    CHECK(planeInput, false);
    offsetValue = ValueInput::createByReal(fretboard.construction_distance_at_nut() * 0.1);
    CHECK(offsetValue, false);
    planeInput->setByOffset(component->yZConstructionPlane(), offsetValue);
    auto construction_plane_at_nut = planes->add(planeInput);
    CHECK(construction_plane_at_nut, false);
    construction_plane_at_nut->name("Nut");
    
    // create construction plane at last fret
    planeInput = planes->createInput();
    CHECK(planeInput, false);
    offsetValue = ValueInput::createByReal(fretboard.construction_distance_at_last_fret() * 0.1);
    CHECK(offsetValue, false);
    planeInput->setByOffset(component->yZConstructionPlane(), offsetValue);
    auto construction_plane_at_last_fret = planes->add(planeInput);
    CHECK(construction_plane_at_last_fret, false);
    construction_plane_at_last_fret->name("Last Fret");
    
    // create construction plane at heel
    planeInput = planes->createInput();
    CHECK(planeInput, false);
    offsetValue = ValueInput::createByReal(fretboard.construction_distance_at_heel() * 0.1);
    CHECK(offsetValue, false);
    planeInput->setByOffset(component->yZConstructionPlane(), offsetValue);
    CHECK(planeInput, false);
    auto construction_plane_at_heel = planes->add(planeInput);
    CHECK(construction_plane_at_heel, false);
    construction_plane_at_heel->name("Heel Side");
    
    // create construction plane at 12th fret
    planeInput = planes->createInput();
    CHECK(planeInput, false);
    offsetValue = ValueInput::createByReal(fretboard.construction_distance_at_12th_fret() * 0.1);
    CHECK(offsetValue, false);
    planeInput->setByOffset(component->yZConstructionPlane(), offsetValue);
    auto construction_plane_at_12th_fret = planes->add(planeInput);
    CHECK(construction_plane_at_12th_fret, false);
    construction_plane_at_12th_fret->name("12th Fret");
    
    // draw radius circle at nut
    auto radius_1 = create_radius_circle(component, construction_plane_at_nut_side, instrument.radius_at_nut, instrument.fretboard_thickness);
    CHECK(radius_1, false);
    //    auto radius_2 = create_radius_circle(component, construction_plane_at_nut, instrument.radius_at_nut, instrument.fretboard_thickness);
    //    auto radius_3 = create_radius_circle(component, construction_plane_at_last_fret,  instrument.radius_at_last_fret, instrument.fretboard_thickness);
    auto radius_4 = create_radius_circle(component, construction_plane_at_heel, instrument.radius_at_last_fret, instrument.fretboard_thickness);
    CHECK(radius_4, false);

    auto fretboardCreationMiddleIndex = timeline->markerPosition();
    timelineGroups->add(fretboardCreationStartIndex + 1, fretboardCreationMiddleIndex - 1);
    progress(progressDialog, "create fretboard radius");

    // create loft feature
    auto loft_features = component->features()->loftFeatures();
    CHECK(loft_features, false);
    auto loft_input = loft_features->createInput(adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
    CHECK(loft_input, false);
    loft_input->loftSections()->add(radius_1->profiles()->item(0));
    //    loft_input->loftSections()->add(radius_2->profiles()->item(0));
    //    loft_input->loftSections()->add(radius_3->profiles()->item(0));
    loft_input->loftSections()->add(radius_4->profiles()->item(0));
    loft_input->isSolid(true);
    auto feature = loft_features->add(loft_input);
    CHECK(feature, false);
    auto main_body = feature->bodies()->item(0);
    CHECK(main_body, false);
    main_body->name("Main body");
//    auto lib = Fretboarder::app->materialLibraries()->itemByName("Fusion 360 Material Library");
    auto lib = Fretboarder::app->materialLibraries()->itemById("C1EEA57C-3F56-45FC-B8CB-A9EC46A9994C"); ////("Fusion 360 Material Library");
    CHECK(lib, false);
    auto mat = lib->materials()->itemById("PrismMaterial-271"); //("Walnut");
    CHECK(mat, false);
//    std::stringstream str;
//    str << "Material library id:" << lib->id() << " material id: " << mat->id();
//    Fretboarder::ui->messageBox(str.str());
    main_body->material(mat);

    
    radius_1->isVisible(false);
    radius_4->isVisible(false);

    auto fretboardCreationEndIndex = timeline->markerPosition();
    timelineGroups->add(fretboardCreationMiddleIndex + 1, fretboardCreationEndIndex - 1);

    auto distance = ValueInput::createByReal(instrument.fretboard_thickness);
    CHECK(distance, false);
    component->features()->extrudeFeatures()->addSimple(contour_sketch->profiles()->item(0), distance, FeatureOperations::IntersectFeatureOperation);

    // create nut slot
    progress(progressDialog, "create nut");
    if (instrument.carve_nut_slot) {
        auto nut_sketch = component->sketches()->add(component->xYConstructionPlane());
        CHECK(nut_sketch, false);
        nut_sketch->name("Nut");
        nut_sketch->isComputeDeferred(true);
        create_closed_polygon(nut_sketch->sketchCurves()->sketchLines(), fretboard.nut_slot_shape());
        nut_sketch->isComputeDeferred(false);
        distance = ValueInput::createByReal(instrument.fretboard_thickness * 0.1);
        CHECK(distance, false);
        auto _feature = component->features()->extrudeFeatures()->addSimple(nut_sketch->profiles()->item(0), distance, FeatureOperations::NewBodyFeatureOperation);
        CHECK(_feature, false);
        auto nut_body = _feature->bodies()->item(0);
        CHECK(nut_body, false);
        auto transform = Matrix3D::create();
        CHECK(transform, false);
        transform->translation(Vector3D::create(0.0, 0.0, instrument.nut_height_under * 0.1));
        auto items = ObjectCollection::create();
        CHECK(items, false);
        items->add(nut_body);
        auto move_features = component->features()->moveFeatures();
        CHECK(move_features, false);
        auto move_input = move_features->createInput(items, transform);
        CHECK(move_input, false);
        component->features()->moveFeatures()->add(move_input);
        items = ObjectCollection::create();
        items->add(nut_body);
        auto combine_input = component->features()->combineFeatures()->createInput(main_body, items);
        CHECK(combine_input, false);
        combine_input->operation(FeatureOperations::CutFeatureOperation);
        component->features()->combineFeatures()->add(combine_input);
    }

    auto nutslotCreationEndIndex = timeline->markerPosition();
    timelineGroups->add(fretboardCreationEndIndex + 1, nutslotCreationEndIndex - 1);

    // duplicate main body to keep an unslotted version
    auto unslotted = main_body->copyToComponent(occurrence);
    CHECK(unslotted, false);
    unslotted->name("Unslotted");
    unslotted->isVisible(false);
    
    main_body->isVisible(false);
    
    main_body->isVisible(true);
    
    auto top = getFretboardTopSurface(component);
    CHECK(top, false);
    if (top) {
        auto fretsOccurrence = component->occurrences()->addNewComponent(Matrix3D::create());
        CHECK(fretsOccurrence, false);
        auto fretsComponent = fretsOccurrence->component();
        CHECK(fretsComponent, false);
        fretsComponent->name("Frets");
        //occurrence->activate();

        // Prepare material for frets
        //auto lib = Fretboarder::app->materialLibraries()->itemByName("Fusion 360 Material Library");
//        auto mat = lib->materials()->itemByName("Steel, Chrome Plated");
        auto mat = lib->materials()->itemById("PrismMaterial-069"); //("Steel, Chrome Plated");
//        std::stringstream str;
//        str << "Material library id:" << lib->id() << " material id: " << mat->id();
//        Fretboarder::ui->messageBox(str.str());
        CHECK(mat, false);

        std::vector<Ptr<BRepFace>> faces;
        faces.push_back(top);
        auto C = fret_slots_sketch->sketchCurves();
        CHECK(C, false);
        auto L = fret_lines_sketch->sketchCurves();
        CHECK(L, false);
        for (int i = 0; i < C->count(); i++) {
            auto fretCreationStartIndex = timeline->markerPosition();

            {
                std::stringstream str;
                str << "fret " << i;
                progress(progressDialog, str.str());
            }
            
            std::vector<Ptr<Base>> curvesS;
            std::vector<Ptr<Base>> curvesL;
            auto c = C->item(i);
            CHECK(c, false);
            auto l = L->item(i);
            CHECK(l, false);
            curvesS.push_back(c);
            curvesL.push_back(l);

            std::stringstream str;
            str << "Fret Profile " << i;
            auto projected_fret_profile = fretsComponent->sketches()->add(fretsComponent->yZConstructionPlane());
            CHECK(projected_fret_profile, false);
            projected_fret_profile->name(str.str());
            
            auto profilesS = projected_fret_profile->projectToSurface(faces, curvesS, AlongVectorSurfaceProjectType, fretsComponent->zConstructionAxis());
            std::vector<Ptr<Sketch>> profiles;
            CHECK(profilesS.size() == 1, false);

            auto profileS = profilesS[0];
            CHECK(profileS, false);

            Ptr<Path> pathS;
            pathS = fill_path_from_profile(fretsComponent, pathS, profileS, i, profiles);
            CHECK(pathS, false);

            auto profilesL = projected_fret_profile->projectToSurface(faces, curvesL, AlongVectorSurfaceProjectType, fretsComponent->zConstructionAxis());
            Ptr<Path> pathL;
            auto profileL = profilesL[0];
            CHECK(profileL, false);
            pathL = fill_path_from_profile(fretsComponent, pathL, profileL, i, profiles);
            CHECK(pathL, false);
            Ptr<Point3D> startS, endS;

            pathS->item(0)->curve()->evaluator()->getEndPoints(startS, endS);
            Ptr<Point3D> startL, endL;

            pathL->item(0)->curve()->evaluator()->getEndPoints(startL, endL);

            if (!instrument.carve_fret_slots && !instrument.draw_frets) {
                continue;
            }
            // create fret wire profile
            auto p1 = fretboard.fret_slots()[i].point1;
            auto p2 = fretboard.fret_slots()[i].point2;
            Point pp((p1.x + p2.x)/2, (p1.y + p2.y)/2, (p1.z + p2.z)/2);
            pp.x *= 10;
            //pp.y = ;
            pp.y -= (instrument.fretboard_thickness - instrument.fret_slots_height) * 10 ;
            
            auto fret_tang_profile = create_frettang_profile(instrument, fretboard, i, fretsComponent, pathS);
            CHECK(fret_tang_profile, false);
            
            std::stringstream strFret;
            strFret << "fret tang " << i;
            auto fretTang = create_fret_element(fret_tang_profile, pathS, fretsComponent, strFret.str());
            CHECK(fretTang, false);

            auto items = ObjectCollection::create();
            CHECK(items, false);
            for (int j = 0; j < fretTang->bodies()->count(); j++) {
                auto body = fretTang->bodies()->item(j);
                CHECK(body, false);
                items->add(body);
            }
            auto combine_input = fretsComponent->features()->combineFeatures()->createInput(main_body, items);
            CHECK(combine_input, false);
            combine_input->isKeepToolBodies(true);
            combine_input->operation(FeatureOperations::CutFeatureOperation);
            fretsComponent->features()->combineFeatures()->add(combine_input);

            // remote temp objects
            fret_tang_profile->isVisible(false);

            auto fretsCreationStartIndex = timeline->markerPosition();

            if (instrument.draw_frets) {
                auto fret_wire_profile = create_fretwire_profile(instrument, fretboard, i, fretsComponent, pathL);
                CHECK(fret_wire_profile, false);

                std::stringstream str;
                str << "fret " << i;

                auto fret = create_fret_element(fret_wire_profile, pathL, fretsComponent, str.str());
                CHECK(fret, false);

                
                // Try to fillet the start and end faces' top edges
                if (1) {
                    auto bodies = fret->bodies();
                    CHECK(bodies, false);
                    auto body = bodies->item(0);
                    CHECK(body, false);
                    auto edges = body->edges();
                    CHECK(edges, false);

                    auto filetInput = fretsComponent->features()->filletFeatures()->createInput();
                    SHOWERROR();
                    CHECK(filetInput, false);
                    auto edgesColl = ObjectCollection::create();
                    CHECK(edgesColl, false);

                    double maxRadius = instrument.fret_crown_height * 1.1;
                    
                    for (int e = 0; e < edges->count(); e++) {
                        auto edge = edges->item(e);
                        CHECK(edge, false);
                        auto ellipticalArc3D = edge->geometry()->cast<EllipticalArc3D>();
                        if (ellipticalArc3D && ellipticalArc3D->majorRadius() < maxRadius) {
                            edgesColl->add(edge);
                            continue;
                        }

                        auto arc3D = edge->geometry()->cast<Arc3D>();
                        if (arc3D && arc3D->radius() < maxRadius) {
                            edgesColl->add(edge);
                            continue;
                        }
                    }

                    auto radius = ValueInput::createByReal(.1);
                    CHECK(radius, false);
                    bool res = filetInput->addConstantRadiusEdgeSet(edgesColl, radius, false);
                    filetInput->isG2(false);
                    filetInput->isTangentChain(false);
                    filetInput->isRollingBallCorner(true);
                    CHECK(res, false);
                    SHOWERROR();

                    auto filet = fretsComponent->features()->filletFeatures()->add(filetInput);
                    CHECK(filet, false);
                }
                
                
                auto items = ObjectCollection::create();
                CHECK(items, false);
                for (int j = 0; j < fretTang->bodies()->count(); j++) {
                    auto body = fretTang->bodies()->item(j);
                    CHECK(body, false);
                    items->add(body);
                }

                auto bodies = fretsComponent->bRepBodies();
                CHECK(bodies, false);
                auto combine_input = fretsComponent->features()->combineFeatures()->createInput(bodies->item(bodies->count()-1), items);
                CHECK(combine_input, false);
                combine_input->isKeepToolBodies(false);
                combine_input->operation(FeatureOperations::NewBodyFeatureOperation);
                fretsComponent->features()->combineFeatures()->add(combine_input);

                for (int b = 0; b < fretsComponent->bRepBodies()->count(); b++)
                {
                    auto body = fretsComponent->bRepBodies()->item(b);
                    CHECK(body, false);
                    body->material(mat);
                }
                
                fret_wire_profile->isVisible(false);

                auto fretCreationEndIndex = timeline->markerPosition();
                auto group = timelineGroups->add(fretCreationStartIndex + 1, fretCreationEndIndex - 1);
                group->isCollapsed(true);

            }
            projected_fret_profile->isVisible(false);

            for (auto p : profiles) {
                p->isVisible(false);
            }

            auto fretsCreationEndIndex = timeline->markerPosition();
            timelineGroups->add(fretsCreationStartIndex + 1, fretsCreationEndIndex - 1);

        }
        
//        main_body->isVisible(false);
    }
    else {
        Fretboarder::ui->messageBox("Top not found");
    }
    
    progressDialog->hide();

    return true;
}

extern "C" XI_EXPORT bool run(const char* context)
{
    Fretboarder::app = Application::get();
    if (!Fretboarder::app)
        return false;
    
    Fretboarder::ui = Fretboarder::app->userInterface();
    if (!Fretboarder::ui)
        return false;
    
    // get the entry for custom graphics
    Ptr<Product> activeProd = Fretboarder::app->activeProduct();
    if (!activeProd)
        return false;

    Ptr<CAM> cam = activeProd->cast<CAM>();
    if (cam) {
        Fretboarder::cgGroups = cam->customGraphicsGroups();
    }
    else {
        auto design = activeProd->cast<Design>();
        if (!design)
            return false;

        Ptr<Component> rootComp = design->rootComponent();
        if (!rootComp)
            return false;
        Fretboarder::cgGroups = rootComp->customGraphicsGroups();
    }
    if (!Fretboarder::cgGroups)
        return false;

    auto customFeatureDefinition = CustomFeatureDefinition::create("Fretboarder.Fretboard", "Fretboard", "");
    customFeatureDefinition->editCommandId("editFretboard");
    auto customFratureComputeEvent = customFeatureDefinition->customFeatureCompute();
//    customFratureComputeEvent->add(customFratureComputeEventHandler);

    // Create the command definition.
    Ptr<CommandDefinitions> commandDefinitions = Fretboarder::ui->commandDefinitions();
    if (!commandDefinitions)
        return nullptr;
    
    // Get the existing command definition or create it if it doesn't already exist.
    Ptr<CommandDefinition> cmdDef = commandDefinitions->itemById("cmdFretboarder");
    if (!cmdDef)
    {
        cmdDef = commandDefinitions->addButtonDefinition("cmdFretboarder",
                                                         "Fretboarder",
                                                         "Create fretboards for stringed instruments.");
    }
    
    // Connect to the command created event.
    Ptr<CommandCreatedEvent> commandCreatedEvent = cmdDef->commandCreated();
    if (!commandCreatedEvent)
        return false;
    commandCreatedEvent->add(&_cmdCreatedHandler);
    
    // Execute the command definition.
    cmdDef->execute();
    
    // Prevent this module from being terminated when the script returns, because we are waiting for event handlers to fire.
    adsk::autoTerminate(false);
    
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
