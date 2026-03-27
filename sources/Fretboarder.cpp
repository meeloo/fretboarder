#include "Fretboarder.h"

Ptr<Application> Fretboarder::app;
Ptr<UserInterface> Fretboarder::ui;
Ptr<CustomGraphicsGroups> Fretboarder::cgGroups;
Ptr<CustomFeatureDefinition> Fretboarder::customFeatureDef;

OnCommandCreatedEventHandler _cmdCreatedHandler;
CustomFeatureComputeEventHandler _customFeatureComputeHandler;
OnEditCommandCreatedEventHandler _editCmdCreatedHandler;

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

bool createFretboard(const fretboarder::Instrument& instrument,
                     Ptr<Base>& outFirstFeature,
                     Ptr<Base>& outLastFeature,
                     Ptr<Component> inComponent) {

    // Use the caller-supplied component if provided, otherwise look up the
    // root component from the active design.
    Ptr<Component> component = inComponent;
    if (!component) {
        Ptr<Product> product = Fretboarder::app->activeProduct();
        CHECK(product, false);
        Ptr<Design> design = product;
        CHECK(design, false);
        design->designType(ParametricDesignType);
        component = design->rootComponent();
        CHECK(component, false);
    }

    fretboarder::Fretboard fretboard(instrument);

    Ptr<ProgressDialog> progressDialog;
    if (!inComponent) {
        progressDialog = Fretboarder::ui->createProgressDialog();
        progressDialog->isCancelButtonShown(false);
        progressDialog->show("creating fretboard", "", 0, 3 + instrument.number_of_strings + instrument.number_of_frets);
    }

    progress(progressDialog, "create fretboard plank");

    // create strings sketch
    auto strings_area_sketch = component->sketches()->add(component->xYConstructionPlane());
    CHECK(strings_area_sketch, false);
    // First timeline item — anchors the CustomFeature group start.
    // cfInput->setStartAndEndFeatures() (used before cfFeatures->add) accepts
    // Sketch types; cf->setStartAndEndFeatures() (post-add) does not.
    outFirstFeature = strings_area_sketch;
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
    
    // create construction plane at 12th fret (only when there are enough frets)
    Ptr<ConstructionPlane> construction_plane_at_12th_fret;
    if (instrument.number_of_frets > 12) {
        planeInput = planes->createInput();
        CHECK(planeInput, false);
        offsetValue = ValueInput::createByReal(fretboard.construction_distance_at_12th_fret() * 0.1);
        CHECK(offsetValue, false);
        planeInput->setByOffset(component->yZConstructionPlane(), offsetValue);
        construction_plane_at_12th_fret = planes->add(planeInput);
        CHECK(construction_plane_at_12th_fret, false);
        construction_plane_at_12th_fret->name("12th Fret");
    }
    
    // draw radius circle at nut
    auto radius_1 = create_radius_circle(component, construction_plane_at_nut_side, instrument.radius_at_nut, instrument.fretboard_thickness);
    CHECK(radius_1, false);
    //    auto radius_2 = create_radius_circle(component, construction_plane_at_nut, instrument.radius_at_nut, instrument.fretboard_thickness);
    //    auto radius_3 = create_radius_circle(component, construction_plane_at_last_fret,  instrument.radius_at_last_fret, instrument.fretboard_thickness);
    auto radius_4 = create_radius_circle(component, construction_plane_at_heel, instrument.radius_at_last_fret, instrument.fretboard_thickness);
    CHECK(radius_4, false);

    progress(progressDialog, "create fretboard radius");

    // create loft feature
    auto loft_features = component->features()->loftFeatures();
    CHECK(loft_features, false);
    auto loft_input = loft_features->createInput(adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
    CHECK(loft_input, false);
    auto profile_1 = radius_1->profiles()->item(0);
    CHECK(profile_1, false);
    auto profile_4 = radius_4->profiles()->item(0);
    CHECK(profile_4, false);
    CHECK(loft_input->loftSections()->add(profile_1), false);
    CHECK(loft_input->loftSections()->add(profile_4), false);
    loft_input->isSolid(true);
    auto feature = loft_features->add(loft_input);
    CHECK(feature, false);
    // In Fusion's parametric timeline, features added during command execution are
    // queued but not computed until the timeline is evaluated after the handler returns.
    // RolledBackFeatureHealthState (4) means "not yet computed" — not a failure.
    // Only fail fast if the feature is in an actual error state.
    if (feature->healthState() == ErrorFeatureHealthState) {
        std::string fusionErr;
        Fretboarder::app->getLastError(&fusionErr);
        std::string featureErr = feature->errorOrWarningMessage();
        std::ostringstream msg;
        msg << "The fretboard loft failed.\n\n"
            << "Computed values (all in mm):\n"
            << "  Bass scale:    " << instrument.scale_length[0] << " mm\n"
            << "  Treble scale:  " << instrument.scale_length[1] << " mm\n"
            << "  Frets:         " << instrument.number_of_frets << "\n"
            << "  Perp fret:     " << instrument.perpendicular_fret_index << "\n"
            << "  Radius at nut: " << instrument.radius_at_nut << " mm\n"
            << "  Radius at 12:  " << instrument.radius_at_last_fret << " mm\n"
            << "  Thickness:     " << instrument.fretboard_thickness << " mm\n"
            << "  Loft plane 1 (nut side):  X = " << fretboard.construction_distance_at_nut_side() << " mm\n"
            << "  Loft plane 2 (heel side): X = " << fretboard.construction_distance_at_heel() << " mm\n"
            << "  Loft error message: " << featureErr << "\n"
            << "  Fusion error: " << fusionErr;
        Fretboarder::ui->messageBox(msg.str());
        return false;
    }
    // Body name and material are cosmetic — only set them if the body is already
    // available (it won't be during deferred evaluation; Fusion sets it up later).
    Ptr<BRepBody> main_body;
    auto lib = Fretboarder::app->materialLibraries()->itemById("C1EEA57C-3F56-45FC-B8CB-A9EC46A9994C");
    if (feature->bodies() && feature->bodies()->count() > 0) {
        main_body = feature->bodies()->item(0);
    }

    radius_1->isVisible(false);
    radius_4->isVisible(false);

    auto distance = ValueInput::createByReal(instrument.fretboard_thickness * 0.1);
    CHECK(distance, false);
    auto intersectExtrude = component->features()->extrudeFeatures()->addSimple(contour_sketch->profiles()->item(0), distance, FeatureOperations::IntersectFeatureOperation);

    // Use the loft feature as the guaranteed-non-null fallback for outLastFeature.
    // setStartAndEndFeatures MUST be called for the CF to evaluate its inner features
    // and produce geometry. If it is not called the CF scope is empty and all solid
    // feature recipes are rolled back, leaving only sketches.
    outLastFeature = feature;

    // The intersect extrude trims the arc cylinder to the board shape.
    // In deferred context, addSimple with IntersectFeatureOperation may return null
    // if Fusion can't resolve a body to intersect at recipe-creation time — that's
    // handled gracefully: the loft remains as outLastFeature.
    if (intersectExtrude)
        outLastFeature = intersectExtrude;

    // create nut slot
    progress(progressDialog, "create nut");
    if (instrument.carve_nut_slot) {
        // Use CutFeatureOperation directly — no body reference needed.
        // Fusion implicitly targets the fretboard body at evaluation time.
        auto nutSlotPlaneInput = planes->createInput();
        CHECK(nutSlotPlaneInput, false);
        auto nutSlotOffset = ValueInput::createByReal(instrument.nut_height_under * 0.1);
        CHECK(nutSlotOffset, false);
        nutSlotPlaneInput->setByOffset(component->xYConstructionPlane(), nutSlotOffset);
        auto nut_slot_plane = planes->add(nutSlotPlaneInput);
        CHECK(nut_slot_plane, false);
        nut_slot_plane->name("Nut Slot");

        auto nut_sketch = component->sketches()->add(nut_slot_plane);
        CHECK(nut_sketch, false);
        nut_sketch->name("Nut");
        nut_sketch->isComputeDeferred(true);
        create_closed_polygon(nut_sketch->sketchCurves()->sketchLines(), fretboard.nut_slot_shape());
        nut_sketch->isComputeDeferred(false);

        // Cut upward from nut_height_under through the remaining fretboard thickness.
        // Add 0.5 cm clearance to ensure the cut goes all the way through.
        auto nutCutDist = ValueInput::createByReal(
            (instrument.fretboard_thickness - instrument.nut_height_under) * 0.1 + 0.5);
        CHECK(nutCutDist, false);
        auto nutCutFeature = component->features()->extrudeFeatures()->addSimple(
            nut_sketch->profiles()->item(0), nutCutDist, FeatureOperations::CutFeatureOperation);
        if (nutCutFeature)
            outLastFeature = nutCutFeature;
    }

    if (main_body) main_body->isVisible(true);

    auto top = getFretboardTopSurface(component);
    if (top) {
        // Use the same component for frets.
        auto fretsComponent = component;

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
            auto tangCombine = fretsComponent->features()->combineFeatures()->add(combine_input);
            outLastFeature = tangCombine;

            // remote temp objects
            fret_tang_profile->isVisible(false);

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
                auto wireCombine = fretsComponent->features()->combineFeatures()->add(combine_input);
                outLastFeature = wireCombine;

                for (int b = 0; b < fretsComponent->bRepBodies()->count(); b++)
                {
                    auto body = fretsComponent->bRepBodies()->item(b);
                    CHECK(body, false);
                    body->material(mat);
                }
                
                fret_wire_profile->isVisible(false);

            }
            projected_fret_profile->isVisible(false);

            for (auto p : profiles) {
                p->isVisible(false);
            }

        }
        
//        main_body->isVisible(false);
    }
    // else: top surface not yet available (deferred evaluation) — frets skipped.
    
    if (progressDialog)
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

    // Create the command definition.
    Ptr<CommandDefinitions> commandDefinitions = Fretboarder::ui->commandDefinitions();
    if (!commandDefinitions)
        return false;

    // Create (or reuse) the main Fretboarder command definition.
    Ptr<CommandDefinition> cmdDef = commandDefinitions->itemById("cmdFretboarder");
    if (!cmdDef)
    {
        cmdDef = commandDefinitions->addButtonDefinition("cmdFretboarder",
                                                         "Fretboarder",
                                                         "Create fretboards for stringed instruments.");
    }
    Ptr<CommandCreatedEvent> commandCreatedEvent = cmdDef->commandCreated();
    if (!commandCreatedEvent)
        return false;
    commandCreatedEvent->add(&_cmdCreatedHandler);

    // Create (or reuse) the edit command definition BEFORE defining the custom
    // feature so that editCommandId can be validated when we set it below.
    Ptr<CommandDefinition> editCmdDef = commandDefinitions->itemById("editFretboard");
    if (!editCmdDef)
    {
        editCmdDef = commandDefinitions->addButtonDefinition("editFretboard",
                                                             "Edit Fretboard",
                                                             "Edit an existing fretboard.");
    }
    Ptr<CommandCreatedEvent> editCommandCreatedEvent = editCmdDef->commandCreated();
    if (!editCommandCreatedEvent)
        return false;
    editCommandCreatedEvent->add(&_editCmdCreatedHandler);

    // Add a button to the Solid > Create panel so the user can invoke the command.
    auto createPanel = Fretboarder::ui->allToolbarPanels()->itemById("SolidCreatePanel");
    if (createPanel) {
        createPanel->controls()->addCommand(cmdDef, "", false);
        // Also add the edit command to the same panel so Fusion considers it
        // a "live" command that can be used as a custom feature editCommandId.
        createPanel->controls()->addCommand(editCmdDef, "", false);
    }

    // Register the custom feature definition AFTER both commands are in the panel.
    Fretboarder::customFeatureDef = CustomFeatureDefinition::create("Fretboarder.Fretboard", "Fretboard", "sources/icons");
    if (Fretboarder::customFeatureDef) {
        Fretboarder::customFeatureDef->editCommandId("editFretboard");
        auto computeEvent = Fretboarder::customFeatureDef->customFeatureCompute();
        if (computeEvent)
            computeEvent->add(&_customFeatureComputeHandler);
    }

    // Prevent this module from being terminated when the script returns,
    // because we are waiting for event handlers to fire.
    adsk::autoTerminate(false);

    return true;
}

extern "C" XI_EXPORT void stop(const char* context)
{
    if (!Fretboarder::ui)
        return;

    // Remove the toolbar buttons.
    auto createPanel = Fretboarder::ui->allToolbarPanels()->itemById("SolidCreatePanel");
    if (createPanel) {
        auto ctrl = createPanel->controls()->itemById("cmdFretboarder");
        if (ctrl)
            ctrl->deleteMe();
        auto editCtrl = createPanel->controls()->itemById("editFretboard");
        if (editCtrl)
            editCtrl->deleteMe();
    }

    // Remove the command definitions.
    auto commandDefinitions = Fretboarder::ui->commandDefinitions();
    if (commandDefinitions) {
        auto cmdDef = commandDefinitions->itemById("cmdFretboarder");
        if (cmdDef) cmdDef->deleteMe();

        auto editCmdDef = commandDefinitions->itemById("editFretboard");
        if (editCmdDef) editCmdDef->deleteMe();
    }
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
