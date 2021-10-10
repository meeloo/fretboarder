//
//  CommandCreatedEventHandler.cpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#include "Fretboarder.h"

void OnExecuteEventHander::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    auto command = eventArgs->command();
    Ptr<CommandInputs> inputs = command->commandInputs();
    if (!inputs)
        return;

    auto instrument = InstrumentFromInputs(inputs);
    createFretboard(instrument);
}

// CommandDestroyed event handler
void OnDestroyEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    adsk::terminate();
}

// CommandCreated event handler.
void OnCommandCreatedEventHandler::notify(const Ptr<CommandCreatedEventArgs>& eventArgs)
{
    if (eventArgs)
    {
        // Get the command that was created.
        Ptr<Command> command = eventArgs->command();
        if (command)
        {
            command->setDialogMinimumSize(300, 600);

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

            Ptr<CommandEvent> onExecutePtrview = command->executePreview();
            if (!onExecutePtrview)
                return;
            isOk = onExecutePtrview->add(&onExecutePreviewHandler);
            if (!isOk)
                return;

            Ptr<ValidateInputsEvent> onValidateInputs = command->validateInputs();
            if (!onValidateInputs)
                return;
            isOk = onValidateInputs->add(&onValidateInputsHandler);
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

            auto group = inputs->addTabCommandInput("strings", "Strings")->children();
            CHECK2(group);

            auto presetCombo = group->addDropDownCommandInput("presets", "Presets", TextListDropDownStyle);
            CHECK2(presetCombo);
            auto presets = presetCombo->listItems();
            CHECK2(presets);
            auto allPresets = Preset::presets();
            for (int i = 0; i < allPresets.size(); i++) {
                auto p = allPresets[i];
                presets->add(p.name, false);
            }

            auto loadButton = group->addBoolValueInput("Load", "Load preset", false);
            auto saveButton = group->addBoolValueInput("Save", "Save preset", false);

            auto rightHanded = group->addBoolValueInput("right_handed", "Right handed", true, "", true);
            rightHanded->tooltip("Switch in between right and left handed string layout.");
            rightHanded->tooltipDescription("The left handed version just mirrors the layout of the strings.");

            auto number_of_strings_slider = group->addIntegerSliderCommandInput("number_of_strings", "Count", 1, 15);
            number_of_strings_slider->tooltip("Select the number of strings for your instrument.");
            number_of_strings_slider->tooltipDescription("Minimum is 1 string, maximum 15.");
            CHECK2(number_of_strings_slider);
            number_of_strings_slider->valueOne(6);

            auto inter_string_spacing_at_nut = group->addFloatSpinnerCommandInput("inter_string_spacing_at_nut", "Spacing at nut", "mm", 0.1, 40, 0.1, 7.5);
            inter_string_spacing_at_nut->tooltip("This is the distance in between each string on the nut (or the zero string if you have one)");

            auto inter_string_spacing_at_bridge = group->addFloatSpinnerCommandInput("inter_string_spacing_at_bridge", "Spacing at bridge", "mm", 0.1, 40, 0.1, 12.0);
            inter_string_spacing_at_bridge->tooltip("This is the distance in between each string on the bridge.");
            inter_string_spacing_at_bridge->tooltipDescription("This may be dictated by the design of your bridge.");


            auto overhang_type = group->addDropDownCommandInput("overhang_type", "Overhang type", TextListDropDownStyle);
            auto overhang_items = overhang_type->listItems();
            overhang_items->add("single", true);
            overhang_items->add("nut and last fret", false);
            overhang_items->add("all", false);

            auto overhangSingle = group->addFloatSpinnerCommandInput("overhangSingle", "Fret overhang", "mm", 0, 50, 0.1, 3.0);
            overhangSingle->tooltip("This is the distance in between the outer strings and the border of the fretboard");
            overhangSingle->tooltipDescription("");

            auto overhangNut = group->addFloatSpinnerCommandInput("overhangNut", "Fret overhang at the nut", "mm", 0, 50, 0.1, 3.0);
            overhangNut->tooltip("This is the distance in between the outer string and the border of the fretboard at the nut");
            overhangNut->tooltipDescription("");
            overhangNut->isVisible(false);

            auto overhangLast = group->addFloatSpinnerCommandInput("overhangLast", "Fret overhang at last fret", "mm", 0, 50, 0.1, 3.0);
            overhangLast->tooltip("This is the distance in between the outer strings and the border of the fretboard at the last fret");
            overhangLast->tooltipDescription("");
            overhangLast->isVisible(false);


            auto overhang0 = group->addFloatSpinnerCommandInput("overhang0", "Fret overhang at nut (bass)", "mm", 0, 50, 0.1, 3.0);
            overhang0->tooltip("This is the distance in between the first string and the border of the fretboard at the nut");
            overhang0->tooltipDescription("");
            overhang0->isVisible(false);

            auto overhang1 = group->addFloatSpinnerCommandInput("overhang1", "Fret overhang at last fret (bass)", "mm", 0, 50, 0.1, 3.0);
            overhang1->tooltip("This is the distance in between the first string and the border of the fretboard at the last fret");
            overhang1->tooltipDescription("");
            overhang1->isVisible(false);

            auto overhang2 = group->addFloatSpinnerCommandInput("overhang2", "Fret overhang at nut (treble)", "mm", 0, 50, 0.1, 3.0);
            overhang2->tooltip("This is the distance in between the last string and the border of the fretboard at the nut");
            overhang2->tooltipDescription("");
            overhang2->isVisible(false);

            auto overhang3 = group->addFloatSpinnerCommandInput("overhang3", "Fret overhang at last fret (treble)", "mm", 0, 50, 0.1, 3.0);
            overhang3->tooltip("This is the distance in between the last string and the border of the fretboard at last fret");
            overhang3->tooltipDescription("");
            overhang3->isVisible(false);


            auto draw_strings = group->addBoolValueInput("draw_strings", "Draw strings", true, "", true);
            draw_strings->tooltip("Enabling this will create a sketch for the position of the strings following the ideal radius of the fretboard and the bridge.");
            draw_strings->tooltipDescription("");

            group = inputs->addTabCommandInput("scale_length", "Scale length")->children();
            auto scale_length_bass = group->addFloatSpinnerCommandInput("scale_length_bass", "Bass side", "in", 1, 10000, 0.1, 25.5);
            scale_length_bass->tooltip("Enter the scale length of the bass string");
            scale_length_bass->tooltipDescription("This is independent of the handedness of the instrument you are building. Use the 'right handed' switch above to control that instead.");

            auto scale_length_treble = group->addFloatSpinnerCommandInput("scale_length_treble", "Treble side", "in", 1, 10000, 0.1, 25.0);
            scale_length_treble->tooltip("Enter the scale length of the treble string");
            scale_length_treble->tooltipDescription("This is independent of the handedness of the instrument you are building. Use the 'right handed' switch above to control that instead.");

            group = inputs->addTabCommandInput("fretboard_radius", "Fretboard")->children();
            auto radius_at_nut = group->addFloatSpinnerCommandInput("radius_at_nut", "Radius at nut", "in", 0, 10000, 0.1, 9.5);
            radius_at_nut->tooltip("Enter the desired fretboard radius at the nut.");
            radius_at_nut->tooltipDescription("You can choose a different radius at the nut and at the last fret to create a compound radius.");

            auto radius_at_last_fret = group->addFloatSpinnerCommandInput("radius_at_last_fret", "Radius at last fret", "in", 0, 10000, 0.1, 20.0);
            radius_at_last_fret->tooltip("Enter the desired fretboard radius at the last fret.");
            radius_at_last_fret->tooltipDescription("You can choose a different radius at the nut and at the last fret to create a compound radius.");

            auto fretboard_thickness = group->addFloatSpinnerCommandInput("fretboard_thickness", "Thickness", "mm", 0, 100, 0.1, 7.0);
            fretboard_thickness->tooltip("This is the thickness of your fretboard.");
            fretboard_thickness->tooltipDescription("This has to be less than thickness of the stock plank you will use.");

            group = inputs->addTabCommandInput("frets", "Frets")->children();
            auto number_of_frets_slider = group->addIntegerSliderCommandInput("number_of_frets", "Number of frets", 0, 60);
            number_of_frets_slider->tooltip("This is the number of frets not counting the zero fret.");
            number_of_frets_slider->tooltipDescription("");
            CHECK2(number_of_frets_slider);
            number_of_frets_slider->valueOne(24);

            auto perpendicular_fret_index = group->addFloatSpinnerCommandInput("perpendicular_fret_index", "Perpendicular Fret", "", -36, 100, 0.1, 0.0);
            perpendicular_fret_index->tooltip("This is the position of the perpendicular fret for multiscale instruments. Use 100 to have the bridge be perpendicular to the strings (i.e. not slanted)");
            perpendicular_fret_index->tooltipDescription("This number is a floating point representing the position of the fretboard. If you choose, say, 7.5, the perpandicular position will be calculated as if it was right in between the 7th and the 8th fret. You can also use a negative number to completely offset the slanting of the string.");

            auto has_zero_fret = group->addBoolValueInput("has_zero_fret", "Zero fret", true, "", true);
            has_zero_fret->tooltip("Enable or disable the use of a zero-fret.");
            has_zero_fret->tooltipDescription("Seriously, why would you disable the zero fret? Are you nuts?");

            auto nut_to_zero_fret_offset = group->addFloatSpinnerCommandInput("nut_to_zero_fret_offset", "Distance from nut to zero fret", "mm", 0, 200, 0.1, 3.0);
            nut_to_zero_fret_offset->tooltip("This is the distance in between the zero fret's slot and the nut.");
            nut_to_zero_fret_offset->tooltipDescription("Is it ignored if you don't have a zero fret or if you choose not to carve the nut.");

            auto draw_frets = group->addBoolValueInput("draw_frets", "Draw frets", true, "", true);
            draw_frets->tooltip("Enabling this will enable the creation of bodies that represent the frets.");
            draw_frets->tooltipDescription("This is purely for esthetics, if you want to create renders for a client with a fretboard that looks finished.");

            auto carve_fret_slots = group->addBoolValueInput("carve_fret_slots", "Carve fret slots", true, "", true);
            carve_fret_slots->tooltip("Enabling this will enable the carving of fret slots in the fretboard.");

            auto hidden_tang_length = group->addFloatSpinnerCommandInput("hidden_tang_length", "Blind tang length", "mm", 0, 50, 0.1, 2.0);
            hidden_tang_length->tooltip("This is the distance in between the tang of the frets and the border of the fretboard plank");
            hidden_tang_length->tooltipDescription("If you want to have the fret tangs appearing on the border of the fretboard, use 0mm. Any other number will create blind/hidden frets tangs.");

            auto fret_slots_width = group->addFloatSpinnerCommandInput("fret_slots_width", "Fret slots width", "mm", 0, 2, 0.1, 0.6);
            fret_slots_width->tooltip("This is the kerf of the tool you will use to cut the fret slots and the thickness of the fret's tang. 0.5 to 0.6mm is generally a good range.");
            fret_slots_width->tooltipDescription("This is also used to model the fret tang bodies if you choose to draw the frets");

            auto fret_slots_height = group->addFloatSpinnerCommandInput("fret_slots_height", "Fret slots height", "mm", 0, 10, 0.1, 1.5);
            fret_slots_height->tooltip("This is the depth of the fret slots.");
            fret_slots_height->tooltipDescription("This should be greater of equal than the height of the tang of the frets you are going to use.");

            auto fret_crown_width = group->addFloatSpinnerCommandInput("fret_crown_width", "Fret crown width", "mm", 0, 10, 0.1, 2.34);
            fret_crown_width->tooltip("This is use to model the crown of the frets should you choose to draw them.");
            fret_crown_width->tooltipDescription("This is purely esthetic.");

            auto fret_crown_height = group->addFloatSpinnerCommandInput("fret_crown_height", "Fret crown height", "mm", 0, 10, 0.1, 1.22);
            fret_crown_height->tooltip("This is use to model the crown of the frets should you choose to draw them.");
            fret_crown_height->tooltipDescription("This is purely esthetic.");

            auto last_fret_cut_offset = group->addFloatSpinnerCommandInput("last_fret_cut_offset", "Last fret cut offset", "mm", 0, 10, 0.1, 0);
            last_fret_cut_offset->tooltip("This is an offset that is added after the last fret to choose the length of the fretboard.");
            last_fret_cut_offset->tooltipDescription("Using 0mm here will have your fretboard stopping one half fret after the last fret.");

            group = inputs->addTabCommandInput("nut", "Nut")->children();
            auto space_before_nut = group->addFloatSpinnerCommandInput("space_before_nut", "Space before nut", "mm", 0, 100, 0.1, 7.0);
            space_before_nut->tooltip("This is the amount of fretboard you need before in between it's start and the nut.");
            space_before_nut->tooltipDescription("This is probably 0 if you are using a Gibson type nut. Fender nuts typically need 3 to 5 mm here.");

            auto nut_thickness = group->addFloatSpinnerCommandInput("nut_thickness", "Thickness", "mm", 0, 100, 0.1, 4.0);
            nut_thickness->tooltip("This is the thickness/width of the nut (as seen from above).");
            nut_thickness->tooltipDescription("");

            auto carve_nut_slot = group->addBoolValueInput("carve_nut_slot", "Carve the nut slot", true, "", true);
            carve_nut_slot->tooltip("You can disable carving the nut.");
            carve_nut_slot->tooltipDescription("Some headless instruments use the their mini head pieces as a nut/string guide.");

            auto nut_height_under = group->addFloatSpinnerCommandInput("nut_height_under", "Slot depth", "mm", 0, 100, 0.1, 3.0);
            nut_height_under->tooltip("This is the depth of the nut cavity from the top of the fretboard.");
            nut_height_under->tooltipDescription("");

            auto computed = inputs->addGroupCommandInput("global_values", "Computed values");
            computed->isEnabled(false);
            auto nut_width = computed->children()->addFloatSpinnerCommandInput("nut_width", "Computed nut width", "mm", 0, 1000, 0.1, 45);
            nut_width->tooltip("This is calculated for you.");
            nut_width->tooltipDescription("It indicates the actual width of the nut/fretboard depending on the values you have used for the other parameters");
            CHECK2(nut_width);
            nut_width->isEnabled(false);
        }
    }
}

