#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include "Fretboard.hpp"
#include <iostream>

using namespace adsk::core;
using namespace adsk::fusion;
using namespace fretboarder;

Ptr<Application> app;

Ptr<Point3D> create_point(Point point) {
    return Point3D::create(0.1 * point.x, 0.1 * point.y, 0);
}

//def string(value):
//    return "{} cm".format(quantize(value))
//

//def create_line(sketch_lines, vector):
//    sketch_lines.addByTwoPoints(
//        create_point(vector.point1), create_point(vector.point2)
//    )

void create_closed_polygon(const Ptr<SketchLines>& sketch_lines, const Quad& shape) {
    for (int index = 1; index < 4;  index++) {
        sketch_lines->addByTwoPoints(create_point(shape.points[index - 1]), create_point(shape.points[index]));
    }
    // close
    sketch_lines->addByTwoPoints(create_point(shape.points[3]), create_point(shape.points[0]));
}

void create_line(const Ptr<SketchLines>& sketch_lines, const Vector& vector) {
    sketch_lines->addByTwoPoints(create_point(vector.point1), create_point(vector.point2));
}

Ptr<Sketch> create_radius_circle(const Ptr<Component>& component, const Ptr<ConstructionPlane>& plane, double radius, double thickness) {
    auto sketch = component->sketches()->add(plane);
    sketch->isComputeDeferred(true);
    auto circles = sketch->sketchCurves()->sketchCircles();
    auto center = Point3D::create(-(thickness - radius) * 0.1, 0.0, 0.0);
    circles->addByCenterRadius(center, radius * 0.1);
    sketch->isComputeDeferred(false);
    return sketch;
}


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

    if (0) {
        std::cout << "Fretboarder online" << std::endl;
        for (int i = 0; i < instrument.number_of_frets; i++) {
            auto fret = fretboard.fret_slots().at(i);
            std::cout << "\tfret " << i << std::endl;
    //        app->userInterface()->messageBox("fret");

            Ptr<SketchLine> line = sketchLines->addByTwoPoints(Point3D::create(fret.point1.x * 0.1, fret.point1.y * 0.1, 0), Point3D::create(fret.point2.x * 0.1, fret.point2.y * 0.1, 0));
        }

        auto strings = fretboard.strings();
        for (int i = 0; i < strings.size(); i++) {
            auto s = strings.at(i);
            sketchLines->addByTwoPoints(Point3D::create(s.x_at_start() * 0.1, s.y_at_start() * 0.1, 0), Point3D::create(s.x_at_bridge() * 0.1, s.y_at_bridge() * 0.1, 0));
        }
        std::cout << "Fretboarder done" << std::endl;
    }
    
    // create Fretboard component
    auto occurrence = design->rootComponent()->occurrences()->addNewComponent(Matrix3D::create());
    auto component = occurrence->component();
    component->name("Fretboard");

    // create strings sketch
    auto strings_sketch = component->sketches()->add(component->xYConstructionPlane());
    strings_sketch->name("Strings");
    strings_sketch->isComputeDeferred(true);
    create_closed_polygon(strings_sketch->sketchCurves()->sketchLines(), fretboard.strings_shape());
    strings_sketch->isComputeDeferred(false);

    // create Contour sketch
    auto contour_sketch = component->sketches()->add(component->xYConstructionPlane());
    contour_sketch->name("Contour");
    contour_sketch->isComputeDeferred(true);
    create_closed_polygon(contour_sketch->sketchCurves()->sketchLines(), fretboard.board_shape());
    contour_sketch->isComputeDeferred(false);

    // create fret slots as lines sketch
    auto planes = component->constructionPlanes();
    auto planeInput = planes->createInput();
    auto offsetValue = ValueInput::createByReal((instrument.fretboard_thickness + 10.0) * 0.1);
    planeInput->setByOffset(component->xYConstructionPlane(), offsetValue);
    auto fret_slots_construction_plane = planes->add(planeInput);
    auto fret_slots_sketch = component->sketches()->add(fret_slots_construction_plane);
    fret_slots_sketch->name("Fret slots as lines");
    fret_slots_sketch->isComputeDeferred(true);
    for (auto &&vector : fretboard.fret_slots()) {
        create_line(fret_slots_sketch->sketchCurves()->sketchLines(), vector);
    }
    fret_slots_sketch->isComputeDeferred(false);
    fret_slots_sketch->isVisible(false);
    
    // create construction plane at nut side
    planes = component->constructionPlanes();
    planeInput = planes->createInput();
    offsetValue = ValueInput::createByReal(fretboard.construction_distance_at_nut_side() * 0.1);
    planeInput->setByOffset(component->yZConstructionPlane(), offsetValue);
    auto construction_plane_at_nut_side = planes->add(planeInput);
    construction_plane_at_nut_side->name("Nut Side");

    // create construction plane at nut
    planes = component->constructionPlanes();
    planeInput = planes->createInput();
    offsetValue = ValueInput::createByReal(fretboard.construction_distance_at_nut() * 0.1);
    planeInput->setByOffset(component->yZConstructionPlane(), offsetValue);
    auto construction_plane_at_nut = planes->add(planeInput);
    construction_plane_at_nut->name("Nut");

    // create construction plane at last fret
    planeInput = planes->createInput();
    offsetValue = ValueInput::createByReal(fretboard.construction_distance_at_last_fret() * 0.1);
    planeInput->setByOffset(component->yZConstructionPlane(), offsetValue);
    auto construction_plane_at_last_fret = planes->add(planeInput);
    construction_plane_at_last_fret->name("Last Fret");

    // create construction plane at heel
    planeInput = planes->createInput();
    offsetValue = ValueInput::createByReal(fretboard.construction_distance_at_heel() * 0.1);
    planeInput->setByOffset(component->yZConstructionPlane(), offsetValue);
    auto construction_plane_at_heel = planes->add(planeInput);
    construction_plane_at_heel->name("Heel Side");

    // create construction plane at 12th fret
    planeInput = planes->createInput();
    offsetValue = ValueInput::createByReal(fretboard.construction_distance_at_12th_fret() * 0.1);
    planeInput->setByOffset(component->yZConstructionPlane(), offsetValue);
    auto construction_plane_at_12th_fret = planes->add(planeInput);
    construction_plane_at_12th_fret->name("12th Fret");

    // draw radius circle at nut
    auto radius_1 = create_radius_circle(component, construction_plane_at_nut_side, instrument.radius_at_nut, instrument.fretboard_thickness);
    auto radius_2 = create_radius_circle(component, construction_plane_at_nut, instrument.radius_at_nut, instrument.fretboard_thickness);
    auto radius_3 = create_radius_circle(component, construction_plane_at_last_fret,  instrument.radius_at_last_fret, instrument.fretboard_thickness);
    auto radius_4 = create_radius_circle(component, construction_plane_at_heel, instrument.radius_at_last_fret, instrument.fretboard_thickness);

    // create loft feature
    auto loft_features = component->features()->loftFeatures();
    auto loft_input = loft_features->createInput(adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
    loft_input->loftSections()->add(radius_1->profiles()->item(0));
    loft_input->loftSections()->add(radius_2->profiles()->item(0));
    loft_input->loftSections()->add(radius_3->profiles()->item(0));
    loft_input->loftSections()->add(radius_4->profiles()->item(0));
    loft_input->isSolid(true);
    auto feature = loft_features->add(loft_input);
    auto main_body = feature->bodies()->item(0);
    main_body->name("Main");
    radius_1->isVisible(false);
    radius_2->isVisible(false);
    radius_3->isVisible(false);
    radius_4->isVisible(false);

    #if 0
    // duplicate main body to create the fret slot cutter
    body1 = component.features.copyPasteBodies.add(main_body)
    body1 = body1.bodies.item(0)
    body1.name = "Fret cutter"

    body2 = component.features.copyPasteBodies.add(main_body)
    body2 = body2.bodies.item(0)
    body2.name = "TMP Fret cutter"

    distance = ValueInput.createByReal(
        double(config.instrument.fretboard.thickness)
    )
    component.features.extrudeFeatures.addSimple(
        contour_sketch.profiles.item(0),
        distance,
        adsk.fusion.FeatureOperations.IntersectFeatureOperation,
    )

    // create nut slot
    nut_sketch = component.sketches.add(component.xYConstructionPlane)
    nut_sketch.name = "Nut"
    nut_sketch.is_compute_deferred = True
    create_closed_polygon(
        nut_sketch.sketchCurves.sketchLines, fretboard.nut_slot_shape
    )
    nut_sketch.is_compute_deferred = False
    distance = ValueInput.createByReal(
        double(config.instrument.fretboard.thickness)
    )
    feature = component.features.extrudeFeatures.addSimple(
        nut_sketch.profiles.item(0),
        distance,
        adsk.fusion.FeatureOperations.NewBodyFeatureOperation,
    )
    nut_body = feature.bodies.item(0)
    transform = Matrix3D.create()
    transform.translation = Vector3D.create(
        0.0, 0.0, double(config.instrument.fretboard.nut.height_under)
    )
    items = ObjectCollection.create()
    items.add(nut_body)
    move_features = component.features.moveFeatures
    move_input = move_features.createInput(items, transform)
    component.features.moveFeatures.add(move_input)
    items = ObjectCollection.create()
    items.add(nut_body)
    combine_input = component.features.combineFeatures.createInput(main_body, items)
    combine_input.operation = adsk.fusion.FeatureOperations.CutFeatureOperation
    component.features.combineFeatures.add(combine_input)

    // duplicate main body to keep an unslotted version
    unslotted = component.features.copyPasteBodies.add(main_body)
    unslotted = unslotted.bodies.item(0)
    unslotted.name = "Unslotted"
    unslotted.isVisible = False

    move_features = component.features.moveFeatures
    items = ObjectCollection.create()
    items.add(body2)
    transform = Matrix3D.create()
    transform.translation = Vector3D.create(
        0.0, 0.0, -double(config.instrument.fretboard.fret_slots.height)
    )
    move_input = move_features.createInput(items, transform)
    component.features.moveFeatures.add(move_input)

    main_body.isVisible = False

    combine_features = component.features.combineFeatures
    tools = ObjectCollection.create()
    tools.add(body2)
    combine_input = combine_features.createInput(body1, tools)
    combine_input.isKeepToolBodies = False
    combine_input.operation = adsk.fusion.FeatureOperations.CutFeatureOperation
    combine_features.add(combine_input)

    // create Fret Slots sketch
    fret_slots_sketch = component.sketches.add(component.xYConstructionPlane)
    fret_slots_sketch.name = "Fret slots"
    fret_slots_sketch.is_compute_deferred = True
    for shape in fretboard.fret_slot_shapes:
        create_closed_polygon(fret_slots_sketch.sketchCurves.sketchLines, shape)
    fret_slots_sketch.is_compute_deferred = False

    distance = ValueInput.createByReal(
        double(2 * config.instrument.fretboard.thickness)
    )
    collection = ObjectCollection.create()
    for index in range(fret_slots_sketch.profiles.count):
        collection.add(fret_slots_sketch.profiles.item(index))
    feature = component.features.extrudeFeatures.addSimple(
        collection,
        distance,
        adsk.fusion.FeatureOperations.IntersectFeatureOperation,
    )

    main_body.isVisible = True
    collection = ObjectCollection.create()
    for index in range(feature.bodies.count):
        collection.add(feature.bodies.item(index))
    cut_input = component.features.combineFeatures.createInput(
        main_body, collection
    )
    cut_input.operation = adsk.fusion.FeatureOperations.CutFeatureOperation
    component.features.combineFeatures.add(cut_input)
#endif
    
#if 0
        def create_point(point):
            return Point3D.create(double(point.x), double(point.y), 0)

        def quantize(value):
            value /= Decimal(10)
            return value.quantize(Decimal("0.0001"))

        def double(value):
            return float(quantize(value))

        def string(value):
            return "{} cm".format(quantize(value))

        def create_line(sketch_lines, vector):
            sketch_lines.addByTwoPoints(
                create_point(vector.point1), create_point(vector.point2)
            )

        def create_closed_polygon(sketch_lines, points):
            index = None
            for index in range(len(points)):
                if not index:
                    continue
                sketch_lines.addByTwoPoints(
                    create_point(points[index - 1]), create_point(points[index])
                )
            # close
            if index:
                sketch_lines.addByTwoPoints(
                    create_point(points[index]), create_point(points[0])
                )

        def create_radius_circle(plane, radius, thickness):
            sketch = component.sketches.add(plane)
            sketch.is_compute_deferred = True
            circles = sketch.sketchCurves.sketchCircles
            center = Point3D.create(-double(thickness - radius), 0.0, 0.0)
            circles.addByCenterRadius(center, double(radius))
            sketch.is_compute_deferred = False
            return sketch

        with open(os.path.join(current_file_path, "guitar.yaml"), "r") as yaml_file:
            config = load_config(yaml.safe_load(yaml_file))

        fretboard = FretBoard(config)

        # create Fretboard component
        design = adsk.fusion.Design.cast(app.activeProduct)
        occurrence = design.rootComponent.occurrences.addNewComponent(
            Matrix3D.create()
        )
        component = occurrence.component
        component.name = "Fretboard"

        # create strings sketch
        strings_sketch = component.sketches.add(component.xYConstructionPlane)
        strings_sketch.name = "Strings"
        strings_sketch.is_compute_deferred = True
        create_closed_polygon(
            strings_sketch.sketchCurves.sketchLines, fretboard.strings_shape
        )
        strings_sketch.is_compute_deferred = False

        # create Contour sketch
        contour_sketch = component.sketches.add(component.xYConstructionPlane)
        contour_sketch.name = "Contour"
        contour_sketch.is_compute_deferred = True
        create_closed_polygon(
            contour_sketch.sketchCurves.sketchLines, fretboard.board_shape
        )
        contour_sketch.is_compute_deferred = False

        # create fret slots as lines sketch
        planes = component.constructionPlanes
        planeInput = planes.createInput()
        offsetValue = ValueInput.createByString(
            string(config.instrument.fretboard.thickness + 10)
        )
        planeInput.setByOffset(component.xYConstructionPlane, offsetValue)
        fret_slots_construction_plane = planes.add(planeInput)
        fret_slots_sketch = component.sketches.add(fret_slots_construction_plane)
        fret_slots_sketch.name = "Fret slots as lines"
        fret_slots_sketch.is_compute_deferred = True
        for vector in fretboard.fret_slots:
            create_line(
                fret_slots_sketch.sketchCurves.sketchLines, vector
            )
        fret_slots_sketch.is_compute_deferred = False
        fret_slots_sketch.isVisible = False

        # create construction plane at nut side
        planes = component.constructionPlanes
        planeInput = planes.createInput()
        offsetValue = ValueInput.createByString(
            string(fretboard.construction_distance_at_nut_side)
        )
        planeInput.setByOffset(component.yZConstructionPlane, offsetValue)
        construction_plane_at_nut_side = planes.add(planeInput)
        construction_plane_at_nut_side.name = "Nut Side"

        planes = component.constructionPlanes

        # create construction plane at nut
        planeInput = planes.createInput()
        offsetValue = ValueInput.createByString(
            string(fretboard.construction_distance_at_nut)
        )
        planeInput.setByOffset(component.yZConstructionPlane, offsetValue)
        construction_plane_at_nut = planes.add(planeInput)
        construction_plane_at_nut.name = "Nut"

        # create construction plane at last fret
        planeInput = planes.createInput()
        offsetValue = ValueInput.createByString(
            string(fretboard.construction_distance_at_last_fret)
        )
        planeInput.setByOffset(component.yZConstructionPlane, offsetValue)
        construction_plane_at_last_fret = planes.add(planeInput)
        construction_plane_at_last_fret.name = "Last Fret"

        # create construction plane at heel
        planeInput = planes.createInput()
        offsetValue = ValueInput.createByString(
            string(fretboard.construction_distance_at_heel)
        )
        planeInput.setByOffset(component.yZConstructionPlane, offsetValue)
        construction_plane_at_heel = planes.add(planeInput)
        construction_plane_at_heel.name = "Heel Side"

        # create construction plane at 12th fret
        planeInput = planes.createInput()
        offsetValue = ValueInput.createByString(
            string(fretboard.construction_distance_at_12th_fret)
        )
        planeInput.setByOffset(component.yZConstructionPlane, offsetValue)
        construction_plane_at_12th_fret = planes.add(planeInput)
        construction_plane_at_12th_fret.name = "12th Fret"

        # draw radius circle at nut
        radius_1 = create_radius_circle(
            construction_plane_at_nut_side,
            config.instrument.fretboard.radius.at_nut,
            config.instrument.fretboard.thickness,
        )
        radius_2 = create_radius_circle(
            construction_plane_at_nut,
            config.instrument.fretboard.radius.at_nut,
            config.instrument.fretboard.thickness,
        )
        radius_3 = create_radius_circle(
            construction_plane_at_last_fret,
            config.instrument.fretboard.radius.at_last_fret,
            config.instrument.fretboard.thickness,
        )
        radius_4 = create_radius_circle(
            construction_plane_at_heel,
            config.instrument.fretboard.radius.at_last_fret,
            config.instrument.fretboard.thickness,
        )

        # create loft feature
        loft_features = component.features.loftFeatures
        loft_input = loft_features.createInput(
            adsk.fusion.FeatureOperations.NewBodyFeatureOperation
        )
        loft_input.loftSections.add(radius_1.profiles.item(0))
        loft_input.loftSections.add(radius_2.profiles.item(0))
        loft_input.loftSections.add(radius_3.profiles.item(0))
        loft_input.loftSections.add(radius_4.profiles.item(0))
        loft_input.isSolid = True
        feature = loft_features.add(loft_input)
        main_body = feature.bodies.item(0)
        main_body.name = "Main"
        radius_1.isVisible = False
        radius_2.isVisible = False
        radius_3.isVisible = False
        radius_4.isVisible = False

        # duplicate main body to create the fret slot cutter
        body1 = component.features.copyPasteBodies.add(main_body)
        body1 = body1.bodies.item(0)
        body1.name = "Fret cutter"

        body2 = component.features.copyPasteBodies.add(main_body)
        body2 = body2.bodies.item(0)
        body2.name = "TMP Fret cutter"

        distance = ValueInput.createByReal(
            double(config.instrument.fretboard.thickness)
        )
        component.features.extrudeFeatures.addSimple(
            contour_sketch.profiles.item(0),
            distance,
            adsk.fusion.FeatureOperations.IntersectFeatureOperation,
        )

        # create nut slot
        nut_sketch = component.sketches.add(component.xYConstructionPlane)
        nut_sketch.name = "Nut"
        nut_sketch.is_compute_deferred = True
        create_closed_polygon(
            nut_sketch.sketchCurves.sketchLines, fretboard.nut_slot_shape
        )
        nut_sketch.is_compute_deferred = False
        distance = ValueInput.createByReal(
            double(config.instrument.fretboard.thickness)
        )
        feature = component.features.extrudeFeatures.addSimple(
            nut_sketch.profiles.item(0),
            distance,
            adsk.fusion.FeatureOperations.NewBodyFeatureOperation,
        )
        nut_body = feature.bodies.item(0)
        transform = Matrix3D.create()
        transform.translation = Vector3D.create(
            0.0, 0.0, double(config.instrument.fretboard.nut.height_under)
        )
        items = ObjectCollection.create()
        items.add(nut_body)
        move_features = component.features.moveFeatures
        move_input = move_features.createInput(items, transform)
        component.features.moveFeatures.add(move_input)
        items = ObjectCollection.create()
        items.add(nut_body)
        combine_input = component.features.combineFeatures.createInput(main_body, items)
        combine_input.operation = adsk.fusion.FeatureOperations.CutFeatureOperation
        component.features.combineFeatures.add(combine_input)

        # duplicate main body to keep an unslotted version
        unslotted = component.features.copyPasteBodies.add(main_body)
        unslotted = unslotted.bodies.item(0)
        unslotted.name = "Unslotted"
        unslotted.isVisible = False

        move_features = component.features.moveFeatures
        items = ObjectCollection.create()
        items.add(body2)
        transform = Matrix3D.create()
        transform.translation = Vector3D.create(
            0.0, 0.0, -double(config.instrument.fretboard.fret_slots.height)
        )
        move_input = move_features.createInput(items, transform)
        component.features.moveFeatures.add(move_input)

        main_body.isVisible = False

        combine_features = component.features.combineFeatures
        tools = ObjectCollection.create()
        tools.add(body2)
        combine_input = combine_features.createInput(body1, tools)
        combine_input.isKeepToolBodies = False
        combine_input.operation = adsk.fusion.FeatureOperations.CutFeatureOperation
        combine_features.add(combine_input)

        # create Fret Slots sketch
        fret_slots_sketch = component.sketches.add(component.xYConstructionPlane)
        fret_slots_sketch.name = "Fret slots"
        fret_slots_sketch.is_compute_deferred = True
        for shape in fretboard.fret_slot_shapes:
            create_closed_polygon(fret_slots_sketch.sketchCurves.sketchLines, shape)
        fret_slots_sketch.is_compute_deferred = False

        distance = ValueInput.createByReal(
            double(2 * config.instrument.fretboard.thickness)
        )
        collection = ObjectCollection.create()
        for index in range(fret_slots_sketch.profiles.count):
            collection.add(fret_slots_sketch.profiles.item(index))
        feature = component.features.extrudeFeatures.addSimple(
            collection,
            distance,
            adsk.fusion.FeatureOperations.IntersectFeatureOperation,
        )

        main_body.isVisible = True
        collection = ObjectCollection.create()
        for index in range(feature.bodies.count):
            collection.add(feature.bodies.item(index))
        cut_input = component.features.combineFeatures.createInput(
            main_body, collection
        )
        cut_input.operation = adsk.fusion.FeatureOperations.CutFeatureOperation
        component.features.combineFeatures.add(cut_input)
    except:
        if ui:
            ui.messageBox("Failed:\n{}".format(traceback.format_exc()))
#endif
            
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
