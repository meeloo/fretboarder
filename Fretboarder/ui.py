"""Command dialog: build the inputs, and map them to/from an Instrument.
(Port of the essential parts of Instruments+Inputs.cpp / CommandCreatedEventHandler.cpp.)

The dialog works in centimetres (Fusion's internal length unit), matching the
Instrument fields. A single 'overhang' value is applied to all four sides.
"""
import adsk.core

from luthier import Instrument, presets

# Input ids
PRESET = "preset"
LOAD = "load"
SAVE = "save"
RIGHT_HANDED = "right_handed"
NUMBER_OF_STRINGS = "number_of_strings"
SPACING_AT_NUT = "inter_string_spacing_at_nut"
SPACING_AT_BRIDGE = "inter_string_spacing_at_bridge"
DRAW_STRINGS = "draw_strings"

SCALE_TREBLE = "scale_length_treble"
SCALE_BASS = "scale_length_bass"
PERP_FRET = "perpendicular_fret_index"
HAS_ZERO_FRET = "has_zero_fret"
NUT_TO_ZERO = "nut_to_zero_fret_offset"

NUMBER_OF_FRETS = "number_of_frets"
RADIUS_AT_NUT = "radius_at_nut"
RADIUS_AT_LAST_FRET = "radius_at_last_fret"
FRETBOARD_THICKNESS = "fretboard_thickness"
LAST_FRET_CUT = "last_fret_cut_offset"
OVERHANG = "overhang"
SPACE_BEFORE_NUT = "space_before_nut"

DRAW_FRETS = "draw_frets"
CARVE_FRET_SLOTS = "carve_fret_slots"
FRET_SLOTS_WIDTH = "fret_slots_width"
FRET_SLOTS_HEIGHT = "fret_slots_height"
FRET_CROWN_WIDTH = "fret_crown_width"
FRET_CROWN_HEIGHT = "fret_crown_height"
HIDDEN_TANG = "hidden_tang_length"

CARVE_NUT_SLOT = "carve_nut_slot"
NUT_THICKNESS = "nut_thickness"
NUT_HEIGHT_UNDER = "nut_height_under"


def _tab(inputs, tid, name):
    return inputs.addTabCommandInput(tid, name).children


def _flt(children, cid, name, value, mn=0.0, mx=1000.0, step=0.1, unit="cm"):
    children.addFloatSpinnerCommandInput(cid, name, unit, mn, mx, step, float(value))


def _int(children, cid, name, value, mn, mx):
    children.addIntegerSpinnerCommandInput(cid, name, mn, mx, 1, int(value))


def _bool(children, cid, name, value):
    children.addBoolValueInput(cid, name, True, "", bool(value))


def build_dialog(cmd, instrument):
    inp = cmd.commandInputs
    i = instrument

    strings = _tab(inp, "tab_strings", "Strings")
    dd = strings.addDropDownCommandInput(PRESET, "Preset", adsk.core.DropDownStyles.TextListDropDownStyle)
    dd.listItems.add("Custom", True)
    for name, _ in presets():
        dd.listItems.add(name, False)
    strings.addBoolValueInput(LOAD, "Load .frt", False)
    strings.addBoolValueInput(SAVE, "Save .frt", False)
    _bool(strings, RIGHT_HANDED, "Right handed", i.right_handed)
    _int(strings, NUMBER_OF_STRINGS, "Number of strings", i.number_of_strings, 1, 12)
    _flt(strings, SPACING_AT_NUT, "Inter-string spacing at nut", i.inter_string_spacing_at_nut, 0.1, 5.0, 0.05)
    _flt(strings, SPACING_AT_BRIDGE, "Inter-string spacing at bridge", i.inter_string_spacing_at_bridge, 0.1, 5.0, 0.05)
    _bool(strings, DRAW_STRINGS, "Draw strings", i.draw_strings)

    scale = _tab(inp, "tab_scale", "Scale length")
    _flt(scale, SCALE_TREBLE, "Scale length (treble)", i.scale_length[0], 10.0, 120.0, 0.1)
    _flt(scale, SCALE_BASS, "Scale length (bass)", i.scale_length[1], 10.0, 120.0, 0.1)
    _flt(scale, PERP_FRET, "Perpendicular fret index", i.perpendicular_fret_index, -24.0, 36.0, 1.0, "")
    _bool(scale, HAS_ZERO_FRET, "Has zero fret", i.has_zero_fret)
    _flt(scale, NUT_TO_ZERO, "Nut to zero-fret offset", i.nut_to_zero_fret_offset, 0.0, 5.0, 0.05)

    board = _tab(inp, "tab_board", "Fretboard")
    _int(board, NUMBER_OF_FRETS, "Number of frets", i.number_of_frets, 1, 36)
    _flt(board, RADIUS_AT_NUT, "Radius at nut", i.radius_at_nut, 5.0, 200.0, 0.5)
    _flt(board, RADIUS_AT_LAST_FRET, "Radius at last fret", i.radius_at_last_fret, 5.0, 200.0, 0.5)
    _flt(board, FRETBOARD_THICKNESS, "Fretboard thickness", i.fretboard_thickness, 0.2, 3.0, 0.05)
    _flt(board, OVERHANG, "Overhang", i.overhangs[0], 0.0, 2.0, 0.05)
    _flt(board, SPACE_BEFORE_NUT, "Space before nut", i.space_before_nut, 0.0, 5.0, 0.1)
    _flt(board, LAST_FRET_CUT, "Last fret cut offset", i.last_fret_cut_offset, -5.0, 5.0, 0.1)

    frets = _tab(inp, "tab_frets", "Frets")
    _bool(frets, DRAW_FRETS, "Draw frets", i.draw_frets)
    _bool(frets, CARVE_FRET_SLOTS, "Carve fret slots", i.carve_fret_slots)
    _flt(frets, FRET_SLOTS_WIDTH, "Fret slot width", i.fret_slots_width, 0.01, 0.5, 0.01)
    _flt(frets, FRET_SLOTS_HEIGHT, "Fret slot height", i.fret_slots_height, 0.01, 1.0, 0.01)
    _flt(frets, FRET_CROWN_WIDTH, "Fret crown width", i.fret_crown_width, 0.05, 0.5, 0.01)
    _flt(frets, FRET_CROWN_HEIGHT, "Fret crown height", i.fret_crown_height, 0.02, 0.5, 0.01)
    _flt(frets, HIDDEN_TANG, "Hidden tang length", i.hidden_tang_length, 0.0, 2.0, 0.05)

    nut = _tab(inp, "tab_nut", "Nut")
    _bool(nut, CARVE_NUT_SLOT, "Carve nut slot", i.carve_nut_slot)
    _flt(nut, NUT_THICKNESS, "Nut thickness", i.nut_thickness, 0.1, 2.0, 0.05)
    _flt(nut, NUT_HEIGHT_UNDER, "Nut height under", i.nut_height_under, 0.0, 2.0, 0.05)


def instrument_from_inputs(inputs):
    """Read an Instrument (in cm) from the dialog inputs."""

    def v(cid):
        return inputs.itemById(cid).value

    inst = Instrument()
    inst.right_handed = v(RIGHT_HANDED)
    inst.number_of_strings = int(v(NUMBER_OF_STRINGS))
    inst.inter_string_spacing_at_nut = v(SPACING_AT_NUT)
    inst.inter_string_spacing_at_bridge = v(SPACING_AT_BRIDGE)
    inst.draw_strings = v(DRAW_STRINGS)
    inst.scale_length = [v(SCALE_TREBLE), v(SCALE_BASS)]
    inst.perpendicular_fret_index = v(PERP_FRET)
    inst.has_zero_fret = v(HAS_ZERO_FRET)
    inst.nut_to_zero_fret_offset = v(NUT_TO_ZERO)
    inst.number_of_frets = int(v(NUMBER_OF_FRETS))
    inst.radius_at_nut = v(RADIUS_AT_NUT)
    inst.radius_at_last_fret = v(RADIUS_AT_LAST_FRET)
    inst.fretboard_thickness = v(FRETBOARD_THICKNESS)
    oh = v(OVERHANG)
    inst.overhangs = [oh, oh, oh, oh]
    inst.space_before_nut = v(SPACE_BEFORE_NUT)
    inst.last_fret_cut_offset = v(LAST_FRET_CUT)
    inst.draw_frets = v(DRAW_FRETS)
    inst.carve_fret_slots = v(CARVE_FRET_SLOTS)
    inst.fret_slots_width = v(FRET_SLOTS_WIDTH)
    inst.fret_slots_height = v(FRET_SLOTS_HEIGHT)
    inst.fret_crown_width = v(FRET_CROWN_WIDTH)
    inst.fret_crown_height = v(FRET_CROWN_HEIGHT)
    inst.hidden_tang_length = v(HIDDEN_TANG)
    inst.carve_nut_slot = v(CARVE_NUT_SLOT)
    inst.nut_thickness = v(NUT_THICKNESS)
    inst.nut_height_under = v(NUT_HEIGHT_UNDER)
    inst.validate()
    return inst


def instrument_to_inputs(inputs, inst):
    """Write an Instrument's values back into the dialog (for presets / load)."""

    def s(cid, val):
        inputs.itemById(cid).value = val

    s(RIGHT_HANDED, inst.right_handed)
    s(NUMBER_OF_STRINGS, int(inst.number_of_strings))
    s(SPACING_AT_NUT, inst.inter_string_spacing_at_nut)
    s(SPACING_AT_BRIDGE, inst.inter_string_spacing_at_bridge)
    s(DRAW_STRINGS, inst.draw_strings)
    s(SCALE_TREBLE, inst.scale_length[0])
    s(SCALE_BASS, inst.scale_length[1])
    s(PERP_FRET, inst.perpendicular_fret_index)
    s(HAS_ZERO_FRET, inst.has_zero_fret)
    s(NUT_TO_ZERO, inst.nut_to_zero_fret_offset)
    s(NUMBER_OF_FRETS, int(inst.number_of_frets))
    s(RADIUS_AT_NUT, inst.radius_at_nut)
    s(RADIUS_AT_LAST_FRET, inst.radius_at_last_fret)
    s(FRETBOARD_THICKNESS, inst.fretboard_thickness)
    s(OVERHANG, inst.overhangs[0])
    s(SPACE_BEFORE_NUT, inst.space_before_nut)
    s(LAST_FRET_CUT, inst.last_fret_cut_offset)
    s(DRAW_FRETS, inst.draw_frets)
    s(CARVE_FRET_SLOTS, inst.carve_fret_slots)
    s(FRET_SLOTS_WIDTH, inst.fret_slots_width)
    s(FRET_SLOTS_HEIGHT, inst.fret_slots_height)
    s(FRET_CROWN_WIDTH, inst.fret_crown_width)
    s(FRET_CROWN_HEIGHT, inst.fret_crown_height)
    s(HIDDEN_TANG, inst.hidden_tang_length)
    s(CARVE_NUT_SLOT, inst.carve_nut_slot)
    s(NUT_THICKNESS, inst.nut_thickness)
    s(NUT_HEIGHT_UNDER, inst.nut_height_under)
