#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include "Fretboard.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace adsk::core;
using namespace adsk::fusion;
using namespace fretboarder;

Ptr<Application> app;
Ptr<UserInterface> ui;


Ptr<Point3D> create_point(Point point) {
    return Point3D::create(0.1 * point.x, 0.1 * point.y, 0);
}

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


bool createFretboard(const fretboarder::Instrument& instrument) {
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
//    auto radius_2 = create_radius_circle(component, construction_plane_at_nut, instrument.radius_at_nut, instrument.fretboard_thickness);
//    auto radius_3 = create_radius_circle(component, construction_plane_at_last_fret,  instrument.radius_at_last_fret, instrument.fretboard_thickness);
    auto radius_4 = create_radius_circle(component, construction_plane_at_heel, instrument.radius_at_last_fret, instrument.fretboard_thickness);

    // create loft feature
    auto loft_features = component->features()->loftFeatures();
    auto loft_input = loft_features->createInput(adsk::fusion::FeatureOperations::NewBodyFeatureOperation);
    loft_input->loftSections()->add(radius_1->profiles()->item(0));
//    loft_input->loftSections()->add(radius_2->profiles()->item(0));
//    loft_input->loftSections()->add(radius_3->profiles()->item(0));
    loft_input->loftSections()->add(radius_4->profiles()->item(0));
    loft_input->isSolid(true);
    auto feature = loft_features->add(loft_input);
    auto main_body = feature->bodies()->item(0);
    main_body->name("Main");
    radius_1->isVisible(false);
//    radius_2->isVisible(false);
//    radius_3->isVisible(false);
    radius_4->isVisible(false);

    // duplicate main body to create the fret slot cutter
    auto _body1 = component->features()->copyPasteBodies()->add(main_body);
    auto body1 = _body1->bodies()->item(0);
    body1->name("Fret cutter");

    auto _body2 = component->features()->copyPasteBodies()->add(main_body);
    auto body2 = _body2->bodies()->item(0);
    body2->name("TMP Fret cutter");

    auto distance = ValueInput::createByReal(instrument.fretboard_thickness);
    component->features()->extrudeFeatures()->addSimple(contour_sketch->profiles()->item(0), distance, FeatureOperations::IntersectFeatureOperation);

    // create nut slot
    if (instrument.carve_nut_slot) {
        auto nut_sketch = component->sketches()->add(component->xYConstructionPlane());
        nut_sketch->name("Nut");
        nut_sketch->isComputeDeferred(true);
        create_closed_polygon(nut_sketch->sketchCurves()->sketchLines(), fretboard.nut_slot_shape());
        nut_sketch->isComputeDeferred(false);
        distance = ValueInput::createByReal(instrument.fretboard_thickness * 0.1);
        auto _feature = component->features()->extrudeFeatures()->addSimple(nut_sketch->profiles()->item(0), distance, FeatureOperations::NewBodyFeatureOperation);
        auto nut_body = _feature->bodies()->item(0);
        auto transform = Matrix3D::create();
        transform->translation(Vector3D::create(0.0, 0.0, instrument.nut_height_under * 0.1));
        auto items = ObjectCollection::create();
        items->add(nut_body);
        auto move_features = component->features()->moveFeatures();
        auto move_input = move_features->createInput(items, transform);
        component->features()->moveFeatures()->add(move_input);
        items = ObjectCollection::create();
        items->add(nut_body);
        auto combine_input = component->features()->combineFeatures()->createInput(main_body, items);
        combine_input->operation(FeatureOperations::CutFeatureOperation);
        component->features()->combineFeatures()->add(combine_input);
    }
        
    // duplicate main body to keep an unslotted version
    auto _unslotted = component->features()->copyPasteBodies()->add(main_body);
    auto unslotted = _unslotted->bodies()->item(0);
    unslotted->name("Unslotted");
    unslotted->isVisible(false);

    auto move_features = component->features()->moveFeatures();
    auto items = ObjectCollection::create();
    items->add(body2);
    auto transform = Matrix3D::create();
    transform->translation(Vector3D::create(0.0, 0.0, -instrument.fret_slots_height * 0.1));
    auto move_input = move_features->createInput(items, transform);
    move_features->add(move_input);

    main_body->isVisible(false);

    auto combine_features = component->features()->combineFeatures();
    auto tools = ObjectCollection::create();
    tools->add(body2);
    auto combine_input = combine_features->createInput(body1, tools);
    combine_input->isKeepToolBodies(false);
    combine_input->operation(FeatureOperations::CutFeatureOperation);
    combine_features->add(combine_input);

    // create Fret Slots sketch
    fret_slots_sketch = component->sketches()->add(component->xYConstructionPlane());
    fret_slots_sketch->name("Fret slots");
    fret_slots_sketch->isComputeDeferred(true);
    for (auto&& shape : fretboard.fret_slot_shapes()) {
        create_closed_polygon(fret_slots_sketch->sketchCurves()->sketchLines(), shape);
    }
    fret_slots_sketch->isComputeDeferred(false);

    distance = ValueInput::createByReal(2 * instrument.fretboard_thickness);
    auto collection = ObjectCollection::create();
    for (int index = 0; index < fret_slots_sketch->profiles()->count(); index++) {
        collection->add(fret_slots_sketch->profiles()->item(index));
    }
    auto last_feature = component->features()->extrudeFeatures()->addSimple(collection, distance, FeatureOperations::IntersectFeatureOperation);

    main_body->isVisible(true);
    collection = ObjectCollection::create();
    for (int index = 0; index < last_feature->bodies()->count(); index++) {
        collection->add(last_feature->bodies()->item(index));
    }
    auto cut_input = component->features()->combineFeatures()->createInput(main_body, collection);
    cut_input->operation(FeatureOperations::CutFeatureOperation);
    component->features()->combineFeatures()->add(cut_input);

    return true;
}


// Adds a new row to the table.
static void addRowToTable(Ptr<TableCommandInput> tableInput)
{
    if (!tableInput)
        return;

    // Define a unique id for each row.
    static int rowNumber = 0;
    std::stringstream rowId;
    rowId << rowNumber;

    // Get the CommandInputs object associated with the parent command.
    Ptr<CommandInputs> cmdInputs = tableInput->commandInputs();

    // Create three new command inputs.
    Ptr<CommandInput> childTableValueInput = cmdInputs->addValueInput("TableInput_value" + rowId.str(), "Value", "cm", ValueInput::createByReal(rowNumber));
    Ptr<CommandInput> childTableStringInput = cmdInputs->addStringValueInput("TableInput_string" + rowId.str(), "String", rowId.str());
    Ptr<CommandInput> childTableSpinnerInput = cmdInputs->addIntegerSpinnerCommandInput("spinnerInt" + rowId.str(), "Integer Spinner", 0, 100, 2, rowNumber);

    // Add the inputs to the table.
    int row = tableInput->rowCount();
    tableInput->addCommandInput(childTableValueInput, row, 0);
    tableInput->addCommandInput(childTableStringInput, row, 1);
    tableInput->addCommandInput(childTableSpinnerInput, row, 2);

    rowNumber = rowNumber + 1;
}

// InputChange event handler.
class OnInputChangedEventHander : public adsk::core::InputChangedEventHandler
{
public:
    void notify(const Ptr<InputChangedEventArgs>& eventArgs) override
    {
        Ptr<Command> command = eventArgs->firingEvent()->sender();
        if (!command)
            return;
        Ptr<CommandInputs> inputs = command->commandInputs();
        if (!inputs)
            return;

        Ptr<CommandInput> cmdInput = eventArgs->input();
        if (!cmdInput)
            return;

        Ptr<BoolValueCommandInput> right_handed = inputs->itemById("right_handed");
        Ptr<IntegerSliderCommandInput> number_of_strings = inputs->itemById("number_of_strings");
        Ptr<FloatSpinnerCommandInput> scale_length_treble = inputs->itemById("scale_length_treble");
        Ptr<FloatSpinnerCommandInput> scale_length_bass = inputs->itemById("scale_length_bass");
        Ptr<FloatSpinnerCommandInput> perpendicular_fret_index = inputs->itemById("perpendicular_fret_index");
        Ptr<FloatSpinnerCommandInput> inter_string_spacing_at_nut = inputs->itemById("inter_string_spacing_at_nut");
        Ptr<FloatSpinnerCommandInput> inter_string_spacing_at_bridge = inputs->itemById("inter_string_spacing_at_bridge");
        Ptr<BoolValueCommandInput> has_zero_fret = inputs->itemById("has_zero_fret");
        Ptr<FloatSpinnerCommandInput> nut_to_zero_fret_offset = inputs->itemById("nut_to_zero_fret_offset");
        Ptr<FloatSpinnerCommandInput> space_before_nut = inputs->itemById("space_before_nut");
        Ptr<BoolValueCommandInput> carve_nut_slot = inputs->itemById("carve_nut_slot");
        Ptr<FloatSpinnerCommandInput> nut_thickness = inputs->itemById("nut_thickness");
        Ptr<FloatSpinnerCommandInput> nut_height_under = inputs->itemById("nut_height_under");
        Ptr<FloatSpinnerCommandInput> nut_width = inputs->itemById("nut_width");
        Ptr<FloatSpinnerCommandInput> radius_at_nut = inputs->itemById("radius_at_nut");
        Ptr<FloatSpinnerCommandInput> radius_at_last_fret = inputs->itemById("radius_at_last_fret");
        Ptr<FloatSpinnerCommandInput> fretboard_thickness = inputs->itemById("fretboard_thickness");
        Ptr<IntegerSliderCommandInput> number_of_frets = inputs->itemById("number_of_frets");
        Ptr<FloatSpinnerCommandInput> overhang = inputs->itemById("overhang");
        Ptr<FloatSpinnerCommandInput> hidden_tang_length = inputs->itemById("hidden_tang_length");
        Ptr<FloatSpinnerCommandInput> fret_slots_width = inputs->itemById("fret_slots_width");
        Ptr<FloatSpinnerCommandInput> fret_slots_height = inputs->itemById("fret_slots_height");
        Ptr<FloatSpinnerCommandInput> last_fret_cut_offset = inputs->itemById("last_fret_cut_offset");

        if (cmdInput->id() == "presets") {
            // Preset has changed
            Ptr<DropDownCommandInput> presetCombo = cmdInput;
            auto item = presetCombo->selectedItem();
            if (!item) {
                return;
            }
            
            auto index = item->index();
            auto preset = Preset::presets()[index];
            preset.instrument.validate();

            // Apply Preset:
            right_handed->value(preset.instrument.right_handed);
            number_of_strings->valueOne(preset.instrument.number_of_strings);
            scale_length_treble->value(preset.instrument.scale_length[1]);
            scale_length_bass->value(preset.instrument.scale_length[0]);
            perpendicular_fret_index->value(preset.instrument.perpendicular_fret_index);
            inter_string_spacing_at_nut->value(preset.instrument.inter_string_spacing_at_nut);
            inter_string_spacing_at_bridge->value(preset.instrument.inter_string_spacing_at_bridge);
            has_zero_fret->value(preset.instrument.has_zero_fret);
            nut_to_zero_fret_offset->value(preset.instrument.nut_to_zero_fret_offset);
            space_before_nut->value(preset.instrument.space_before_nut);
            carve_nut_slot->value(preset.instrument.carve_nut_slot);
            nut_thickness->value(preset.instrument.nut_thickness);
            nut_height_under->value(preset.instrument.nut_height_under);
            radius_at_nut->value(preset.instrument.radius_at_nut);
            radius_at_last_fret->value(preset.instrument.radius_at_last_fret);
            fretboard_thickness->value(preset.instrument.fretboard_thickness);
            number_of_frets->valueOne(preset.instrument.number_of_frets);
            overhang->value(preset.instrument.overhang);
            hidden_tang_length->value(preset.instrument.hidden_tang_length);
            fret_slots_width->value(preset.instrument.fret_slots_width);
            fret_slots_height->value(preset.instrument.fret_slots_height);
            last_fret_cut_offset->value(preset.instrument.last_fret_cut_offset);

        }
        nut_width->value(std::max(number_of_strings->valueOne() - 1, 1) * inter_string_spacing_at_nut->value() + overhang->value() * 2);
    }
};

class OnExecuteEventHander : public adsk::core::CommandEventHandler
{
public:
    void notify(const Ptr<CommandEventArgs>& eventArgs) override
    {
        auto command = eventArgs->command();
        Ptr<CommandInputs> inputs = command->commandInputs();
        if (!inputs)
            return;

        Ptr<BoolValueCommandInput> right_handed = inputs->itemById("right_handed");
        Ptr<IntegerSliderCommandInput> number_of_strings = inputs->itemById("number_of_strings");
        Ptr<FloatSpinnerCommandInput> scale_length_treble = inputs->itemById("scale_length_treble");
        Ptr<FloatSpinnerCommandInput> scale_length_bass = inputs->itemById("scale_length_bass");
        Ptr<FloatSpinnerCommandInput> perpendicular_fret_index = inputs->itemById("perpendicular_fret_index");
        Ptr<FloatSpinnerCommandInput> inter_string_spacing_at_nut = inputs->itemById("inter_string_spacing_at_nut");
        Ptr<FloatSpinnerCommandInput> inter_string_spacing_at_bridge = inputs->itemById("inter_string_spacing_at_bridge");
        Ptr<BoolValueCommandInput> has_zero_fret = inputs->itemById("has_zero_fret");
        Ptr<FloatSpinnerCommandInput> nut_to_zero_fret_offset = inputs->itemById("nut_to_zero_fret_offset");
        Ptr<FloatSpinnerCommandInput> space_before_nut = inputs->itemById("space_before_nut");
        Ptr<BoolValueCommandInput> carve_nut_slot = inputs->itemById("carve_nut_slot");
        Ptr<FloatSpinnerCommandInput> nut_thickness = inputs->itemById("nut_thickness");
        Ptr<FloatSpinnerCommandInput> nut_height_under = inputs->itemById("nut_height_under");
        Ptr<FloatSpinnerCommandInput> nut_width = inputs->itemById("nut_width");

        Ptr<FloatSpinnerCommandInput> radius_at_nut = inputs->itemById("radius_at_nut");
        Ptr<FloatSpinnerCommandInput> radius_at_last_fret = inputs->itemById("radius_at_last_fret");
        Ptr<FloatSpinnerCommandInput> fretboard_thickness = inputs->itemById("fretboard_thickness");
        Ptr<IntegerSliderCommandInput> number_of_frets = inputs->itemById("number_of_frets");
        Ptr<FloatSpinnerCommandInput> overhang = inputs->itemById("overhang");
        Ptr<FloatSpinnerCommandInput> hidden_tang_length = inputs->itemById("hidden_tang_length");
        Ptr<FloatSpinnerCommandInput> fret_slots_width = inputs->itemById("fret_slots_width");
        Ptr<FloatSpinnerCommandInput> fret_slots_height = inputs->itemById("fret_slots_height");
        Ptr<FloatSpinnerCommandInput> last_fret_cut_offset = inputs->itemById("last_fret_cut_offset");

        fretboarder::Instrument instrument;
        instrument.right_handed = right_handed->value();
        instrument.number_of_strings = number_of_strings->valueOne();
        instrument.scale_length[0] = scale_length_bass->value();
        instrument.scale_length[1] = scale_length_treble->value();
        instrument.perpendicular_fret_index = perpendicular_fret_index->value();
        instrument.inter_string_spacing_at_nut = inter_string_spacing_at_nut->value();
        instrument.inter_string_spacing_at_bridge = inter_string_spacing_at_bridge->value();
        instrument.has_zero_fret = has_zero_fret->value();
        instrument.nut_to_zero_fret_offset = nut_to_zero_fret_offset->value();
        instrument.number_of_frets = number_of_frets->valueOne();
        instrument.overhang = overhang->value();
        instrument.hidden_tang_length = hidden_tang_length->value();
        instrument.fret_slots_width = fret_slots_width->value();
        instrument.fret_slots_height = fret_slots_height->value();
        instrument.last_fret_cut_offset = last_fret_cut_offset->value();
        instrument.space_before_nut = space_before_nut->value();
        instrument.carve_nut_slot = carve_nut_slot->value();
        instrument.nut_thickness = nut_thickness->value();
        instrument.nut_height_under = nut_height_under->value();
        instrument.radius_at_nut = radius_at_nut->value();
        instrument.radius_at_last_fret = radius_at_last_fret->value();
        instrument.fretboard_thickness = fretboard_thickness->value();
        instrument.validate();
        instrument.scale(10); // cm to mm
        createFretboard(instrument);
    }
};

// CommandDestroyed event handler
class OnDestroyEventHandler : public adsk::core::CommandEventHandler
{
public:
    void notify(const Ptr<CommandEventArgs>& eventArgs) override
    {
        adsk::terminate();
    }
};

// CommandCreated event handler.
class CommandCreatedEventHandler : public adsk::core::CommandCreatedEventHandler
{
public:
    void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override
    {
        if (eventArgs)
        {
            // Get the command that was created.
            Ptr<Command> command = eventArgs->command();
            if (command)
            {
                // Connect to the command destroyed event.
                Ptr<CommandEvent> onDestroy = command->destroy();
                if (!onDestroy)
                    return;
                bool isOk = onDestroy->add(&onDestroyHandler);
                if (!isOk)
                    return;

                // Connect to the command execute event.
                Ptr<CommandEvent> onExecute = command->execute();
                if (!onExecute)
                    return;
                isOk = onExecute->add(&onExecuteHandler);
                if (!isOk)
                    return;


                // Connect to the input changed event.
                Ptr<InputChangedEvent> onInputChanged = command->inputChanged();
                if (!onInputChanged)
                    return;
                isOk = onInputChanged->add(&onInputChangedHandler);
                if (!isOk)
                    return;

                // Get the CommandInputs collection associated with the command.
                Ptr<CommandInputs> inputs = command->commandInputs();
                if (!inputs)
                    return;

                auto presetCombo = inputs->addDropDownCommandInput("presets", "Presets", TextListDropDownStyle);
                auto presets = presetCombo->listItems();
                auto allPresets = Preset::presets();
                for (int i = 0; i < allPresets.size(); i++) {
                    auto p = allPresets[i];
                    presets->add(p.name, false);
                }
                auto group = inputs->addGroupCommandInput("strings", "Strings")->children();
                group->addBoolValueInput("right_handed", "Right handed", true, "", true);
                auto number_of_strings_slider = group->addIntegerSliderCommandInput("number_of_strings", "Count", 1, 20);
                number_of_strings_slider->valueOne(6);
                group->addFloatSpinnerCommandInput("inter_string_spacing_at_nut", "Spacing at nut", "mm", 0.1, 20, 0.1, 7.5);
                group->addFloatSpinnerCommandInput("inter_string_spacing_at_bridge", "Spacing at bridge", "mm", 0.1, 20, 0.1, 12.0);
                group->addFloatSpinnerCommandInput("overhang", "Fret overhang", "mm", 0, 50, 0.1, 3.0);

                group = inputs->addGroupCommandInput("scale_length", "Scale length")->children();
                group->addFloatSpinnerCommandInput("scale_length_bass", "Bass side", "in", 1, 10000, 0.1, 25.5);
                group->addFloatSpinnerCommandInput("scale_length_treble", "Treble side", "in", 1, 10000, 0.1, 25.0);

                group = inputs->addGroupCommandInput("fretboard_radius", "Fretboard geometry")->children();
                group->addFloatSpinnerCommandInput("radius_at_nut", "Radius at nut", "in", 0, 10000, 0.1, 9.5);
                group->addFloatSpinnerCommandInput("radius_at_last_fret", "Radius at last fret", "in", 0, 10000, 0.1, 20.0);
                group->addFloatSpinnerCommandInput("fretboard_thickness", "Thickness", "mm", 0, 100, 0.1, 7.0);

                group = inputs->addGroupCommandInput("frets", "Frets")->children();
                auto number_of_frets_slider = group->addIntegerSliderCommandInput("number_of_frets", "Number of frets", 0, 36);
                number_of_frets_slider->valueOne(24);
                group->addFloatSpinnerCommandInput("perpendicular_fret_index", "Perpendicular Fret", "", 0, 36, 0.1, 0.0);
                group->addBoolValueInput("has_zero_fret", "Zero fret", true, "", true);
                group->addFloatSpinnerCommandInput("nut_to_zero_fret_offset", "Distance from nut to zero fret", "mm", 0, 200, 0.1, 3.0);

                group->addFloatSpinnerCommandInput("hidden_tang_length", "Blind tang length", "mm", 0, 50, 0.1, 2.0);
                group->addFloatSpinnerCommandInput("fret_slots_width", "Fret slots width", "mm", 0, 2, 0.1, 0.6);
                group->addFloatSpinnerCommandInput("fret_slots_height", "Fret slots height", "mm", 0, 10, 0.1, 1.5);
                group->addFloatSpinnerCommandInput("last_fret_cut_offset", "Last fret cut offset", "mm", 0, 10, 0.1, 0);

                group = inputs->addGroupCommandInput("nut", "Nut")->children();
                group->addFloatSpinnerCommandInput("space_before_nut", "Space before nut", "mm", 0, 100, 0.1, 7.0);
                group->addFloatSpinnerCommandInput("nut_thickness", "Thickness", "mm", 0, 100, 0.1, 4.0);
                group->addBoolValueInput("carve_nut_slot", "Carve the nut slot", true, "", true);
                group->addFloatSpinnerCommandInput("nut_height_under", "Slot depth", "mm", 0, 100, 0.1, 3.0);

                auto nut_width = group->addFloatSpinnerCommandInput("nut_width", "Width", "mm", 0, 1000, 0.1, 45);
                nut_width->isEnabled(false);

                
#if 0
                // Create a read only textbox input.
//                inputs->addTextBoxCommandInput("readonly_textBox", "Text Box 1", "This is an example of a read-only text box.", 2, true);

                // Create a message that spans the entire width of the dialog by leaving out the "name" argument.
//                std::string message = "<div align=\"center\">A \"full width\" message using <a href=\"http:fusion360.autodesk.com\">html.</a></div>";
//                inputs->addTextBoxCommandInput("fullWidth_textBox", "", message, 1, true);

                // Create value input.
                inputs->addValueInput("value", "Value", "cm", ValueInput::createByReal(0.0));

                // Create bool value input with checkbox style.
                inputs->addBoolValueInput("checkbox", "Checkbox", true, "", false);

                // Create bool value input with button style that can be clicked.
                //inputs->addBoolValueInput("buttonClick", "Click Button", false, "resources/button", true);

                // Create float slider input with two sliders and visible texts
                Ptr<FloatSliderCommandInput> floatSlider3 = inputs->addFloatSliderCommandInput("floatSlider3", "Float Slider 3", "", 0, 50.0, false);
                if (!floatSlider3)
                    return;
                floatSlider3->setText("Min", "Max");

                // Create integer slider input with one slider
                inputs->addIntegerSliderCommandInput("intSlider", "Integer Slider", 0, 10);

                // Create float spinner input.
                inputs->addFloatSpinnerCommandInput("spinnerFloat", "Float Spinner", "cm", 0.2, 9.0, 2.2, 1);

                // Create integer spinner input.
                inputs->addIntegerSpinnerCommandInput("spinnerInt", "Integer Spinner", 2, 9, 2, 3);
#endif
            }
        }
    }
private:
    OnExecuteEventHander onExecuteHandler;
    OnDestroyEventHandler onDestroyHandler;
    OnInputChangedEventHander onInputChangedHandler;
} _cmdCreatedHandler;



extern "C" XI_EXPORT bool run(const char* context)
{
    app = Application::get();
    if (!app)
        return false;
    
    ui = app->userInterface();
    if (!ui)
        return false;

    // Create the command definition.
    Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
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
