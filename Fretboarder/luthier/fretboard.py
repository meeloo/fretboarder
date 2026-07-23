"""Instrument parameters, presets, and the Fretboard geometry builder
(port of fretboarderLib/Fretboard.hpp + Fretboard.cpp).

Coordinate system: X runs along the neck (+X toward the body), Y across the
strings (+Y toward the first string). X origin is the perpendicular fret.
All dimensions are centimeters.
"""
import json as _json

from .geometry import Point, Vector
from .strings import String

# OverhangType
SINGLE = 0
NUT_AND_LAST_FRET = 1
ALL = 2


def mm_from_inch(v):
    return v * 25.4


def cm_from_inch(v):
    return mm_from_inch(v) * 0.1


class Quad:
    """Four points, in order (a closed quadrilateral)."""
    __slots__ = ("points",)

    def __init__(self, points):
        self.points = list(points)


class Instrument:
    def __init__(self,
                 right_handed=True,
                 number_of_strings=6,
                 scale_length_bass=64.77,
                 scale_length_treble=63.50,
                 perpendicular_fret_index=0,
                 inter_string_spacing_at_nut=0.75,
                 inter_string_spacing_at_bridge=1.2,
                 has_zero_fret=True,
                 nut_to_zero_fret_offset=0.30,
                 number_of_frets=24,
                 overhang0=0.3, overhang1=0.3, overhang2=0.3, overhang3=0.3,
                 hidden_tang_length=0.2,
                 draw_strings=True,
                 draw_frets=True,
                 fret_slots_width=0.06,
                 fret_slots_height=0.15,
                 fret_crown_width=0.234,
                 fret_crown_height=0.122,
                 last_fret_cut_offset=0.0,
                 carve_nut_slot=True,
                 space_before_nut=1.2,
                 nut_thickness=0.4,
                 nut_height_under=0.3,
                 radius_at_nut=25.40,
                 radius_at_last_fret=50.8,
                 fretboard_thickness=0.7):
        self.right_handed = right_handed
        self.number_of_strings = number_of_strings
        # scale_length[0] = treble, [1] = bass (matches the C++ array + .frt files)
        self.scale_length = [scale_length_treble, scale_length_bass]
        self.perpendicular_fret_index = perpendicular_fret_index
        self.inter_string_spacing_at_nut = inter_string_spacing_at_nut
        self.inter_string_spacing_at_bridge = inter_string_spacing_at_bridge
        self.has_zero_fret = has_zero_fret
        self.nut_to_zero_fret_offset = nut_to_zero_fret_offset
        self.number_of_frets_per_octave = 12
        self.number_of_frets = number_of_frets
        self.overhang_type = SINGLE
        self.overhangs = [overhang0, overhang1, overhang2, overhang3]
        self.hidden_tang_length = hidden_tang_length
        self.draw_strings = draw_strings
        self.draw_frets = draw_frets
        self.fret_slots_width = fret_slots_width
        self.fret_slots_height = fret_slots_height
        self.fret_crown_width = fret_crown_width
        self.fret_crown_height = fret_crown_height
        self.carve_fret_slots = True
        self.last_fret_cut_offset = last_fret_cut_offset
        self.carve_nut_slot = carve_nut_slot
        self.space_before_nut = space_before_nut
        self.nut_thickness = nut_thickness
        self.nut_height_under = nut_height_under
        self.radius_at_nut = radius_at_nut
        self.radius_at_last_fret = radius_at_last_fret
        self.fretboard_thickness = fretboard_thickness

        # Computed by validate()
        self.string_spacing_at_nut = 4.30
        self.string_spacing_at_bridge = 7.00
        self.y_at_start = self.string_spacing_at_nut / 2
        self.y_at_bridge = self.string_spacing_at_bridge / 2
        self.validate()

    def scale(self, k):
        self.scale_length[0] *= k
        self.scale_length[1] *= k
        self.inter_string_spacing_at_nut *= k
        self.inter_string_spacing_at_bridge *= k
        self.nut_to_zero_fret_offset *= k
        self.overhangs = [o * k for o in self.overhangs]
        self.hidden_tang_length *= k
        self.fret_slots_width *= k
        self.fret_slots_height *= k
        self.fret_crown_width *= k
        self.fret_crown_height *= k
        self.last_fret_cut_offset *= k
        self.space_before_nut *= k
        self.nut_thickness *= k
        self.nut_height_under *= k
        self.radius_at_nut *= k
        self.radius_at_last_fret *= k
        self.fretboard_thickness *= k
        self.validate()

    def validate(self):
        n = max(2, self.number_of_strings) - 1
        self.string_spacing_at_nut = self.inter_string_spacing_at_nut * n
        self.string_spacing_at_bridge = self.inter_string_spacing_at_bridge * n
        self.y_at_start = self.string_spacing_at_nut / 2
        self.y_at_bridge = self.string_spacing_at_bridge / 2
        if not self.has_zero_fret:
            self.nut_to_zero_fret_offset = 0

    # --- .frt serialization (matches the C++ nlohmann keys) ---
    def to_dict(self):
        return {
            "number_of_strings": self.number_of_strings,
            "scale_length": list(self.scale_length),
            "perpendicular_fret_index": self.perpendicular_fret_index,
            "inter_string_spacing_at_nut": self.inter_string_spacing_at_nut,
            "inter_string_spacing_at_bridge": self.inter_string_spacing_at_bridge,
            "has_zero_fret": self.has_zero_fret,
            "nut_to_zero_fret_offset": self.nut_to_zero_fret_offset,
            "number_of_frets": self.number_of_frets,
            "overhangs": list(self.overhangs),
            "overhang_type": self.overhang_type,
            "hidden_tang_length": self.hidden_tang_length,
            "draw_strings": self.draw_strings,
            "draw_frets": self.draw_frets,
            "fret_slots_width": self.fret_slots_width,
            "fret_slots_height": self.fret_slots_height,
            "fret_crown_width": self.fret_crown_width,
            "fret_crown_height": self.fret_crown_height,
            "last_fret_cut_offset": self.last_fret_cut_offset,
            "carve_nut_slot": self.carve_nut_slot,
            "space_before_nut": self.space_before_nut,
            "nut_thickness": self.nut_thickness,
            "nut_height_under": self.nut_height_under,
            "radius_at_nut": self.radius_at_nut,
            "radius_at_last_fret": self.radius_at_last_fret,
            "fretboard_thickness": self.fretboard_thickness,
        }

    @classmethod
    def from_dict(cls, j):
        inst = cls()
        inst.number_of_strings = j["number_of_strings"]
        inst.scale_length = list(j["scale_length"])
        inst.perpendicular_fret_index = j["perpendicular_fret_index"]
        inst.inter_string_spacing_at_nut = j["inter_string_spacing_at_nut"]
        inst.inter_string_spacing_at_bridge = j["inter_string_spacing_at_bridge"]
        inst.has_zero_fret = j["has_zero_fret"]
        inst.nut_to_zero_fret_offset = j["nut_to_zero_fret_offset"]
        inst.number_of_frets = j["number_of_frets"]
        if "overhang" in j:  # legacy single-value form
            o = j["overhang"]
            inst.overhangs = [o, o, o, o]
        else:
            inst.overhang_type = j["overhang_type"]
            inst.overhangs = list(j["overhangs"])
        inst.hidden_tang_length = j["hidden_tang_length"]
        inst.draw_strings = j["draw_strings"]
        inst.draw_frets = j["draw_frets"]
        inst.fret_slots_width = j["fret_slots_width"]
        inst.fret_slots_height = j["fret_slots_height"]
        inst.fret_crown_width = j["fret_crown_width"]
        inst.fret_crown_height = j["fret_crown_height"]
        inst.last_fret_cut_offset = j["last_fret_cut_offset"]
        inst.carve_nut_slot = j["carve_nut_slot"]
        inst.space_before_nut = j["space_before_nut"]
        inst.nut_thickness = j["nut_thickness"]
        inst.nut_height_under = j["nut_height_under"]
        inst.radius_at_nut = j["radius_at_nut"]
        inst.radius_at_last_fret = j["radius_at_last_fret"]
        inst.fretboard_thickness = j["fretboard_thickness"]
        inst.validate()
        return inst

    def save(self, filename):
        with open(filename, "w") as f:
            _json.dump(self.to_dict(), f)

    @classmethod
    def load(cls, filename):
        with open(filename) as f:
            return cls.from_dict(_json.load(f))


def _preset(right_handed, number_of_strings, scale_length_bass, scale_length_treble,
            perpendicular_fret_index, inter_string_spacing_at_nut, inter_string_spacing_at_bridge,
            has_zero_fret, nut_to_zero_fret_offset, number_of_frets, overhang, hidden_tang_length,
            draw_strings, draw_frets, fret_slots_width, fret_slots_height, fret_crown_width,
            fret_crown_height, last_fret_cut_offset, carve_nut_slot, space_before_nut, nut_thickness,
            nut_height_under, radius_at_nut, radius_at_last_fret, fretboard_thickness):
    """Build a preset Instrument from the intended 26-argument constructor
    (single overhang applied to all four sides). Mirrors the C++ preset call
    sites but with the correct argument arity — see the shift-bug note in the
    port write-up."""
    return Instrument(
        right_handed=right_handed, number_of_strings=number_of_strings,
        scale_length_bass=scale_length_bass, scale_length_treble=scale_length_treble,
        perpendicular_fret_index=perpendicular_fret_index,
        inter_string_spacing_at_nut=inter_string_spacing_at_nut,
        inter_string_spacing_at_bridge=inter_string_spacing_at_bridge,
        has_zero_fret=has_zero_fret, nut_to_zero_fret_offset=nut_to_zero_fret_offset,
        number_of_frets=number_of_frets,
        overhang0=overhang, overhang1=overhang, overhang2=overhang, overhang3=overhang,
        hidden_tang_length=hidden_tang_length, draw_strings=draw_strings, draw_frets=draw_frets,
        fret_slots_width=fret_slots_width, fret_slots_height=fret_slots_height,
        fret_crown_width=fret_crown_width, fret_crown_height=fret_crown_height,
        last_fret_cut_offset=last_fret_cut_offset, carve_nut_slot=carve_nut_slot,
        space_before_nut=space_before_nut, nut_thickness=nut_thickness,
        nut_height_under=nut_height_under, radius_at_nut=radius_at_nut,
        radius_at_last_fret=radius_at_last_fret, fretboard_thickness=fretboard_thickness)


def presets():
    """Named preset instruments (same set and values as the C++ Preset::presets())."""
    rh = True
    bass = 64.77
    treble = 64.77
    sn = 0.72          # spacing_at_nut
    sb = 1.1           # spacing_at_bridge
    bsn = 1.2          # bass_spacing_at_nut
    bsb = 1.8          # bass_spacing_at_bridge
    ntz = 0.30
    frets = 24
    oh = 0.3
    ds = True
    df = True
    ht = 0.2
    sw = 0.06
    sh = 0.15
    cw = 0.3
    ch = 0.3
    lfo = 0.0
    ns = True
    sbn = 1.2
    nt = 0.4
    nh = 0.3
    th = 0.7
    return [
        ("Telecaster", _preset(rh, 6, bass, treble, 0, sn, sb, False, ntz, 22, oh, ht, ds, df, sw, sh, cw, ch, lfo, ns, sbn, nt, nh, cm_from_inch(9.5), cm_from_inch(9.5), th)),
        ("Stratocaster", _preset(rh, 6, bass, treble, 0, sn, sb, False, ntz, 22, oh, ht, ds, df, sw, sh, cw, ch, lfo, ns, sbn, nt, nh, cm_from_inch(10), cm_from_inch(10), th)),
        ("Les paul", _preset(rh, 6, cm_from_inch(24.7), cm_from_inch(24.7), 0, sn, sb, False, ntz, 22, oh, ht, ds, df, sw, sh, cw, ch, lfo, ns, 0, nt, th, cm_from_inch(12), cm_from_inch(12), th)),
        ("Jazz bass", _preset(rh, 4, cm_from_inch(34), cm_from_inch(34), 0, bsn, bsb, False, ntz, frets, oh, ht, ds, df, sw, sh, cw, ch, lfo, ns, sbn, nt, nh, cm_from_inch(12), cm_from_inch(12), th)),
        ("Precision bass", _preset(rh, 4, cm_from_inch(34), cm_from_inch(34), 0, bsn, bsb, False, ntz, frets, oh, ht, ds, df, sw, sh, cw, ch, lfo, ns, sbn, nt, nh, cm_from_inch(12), cm_from_inch(12), th)),
        ("Boden 6", _preset(rh, 6, cm_from_inch(25.5), cm_from_inch(25.0), 0, sn, sb, True, ntz, frets, oh, ht, ds, df, sw, sh, cw, ch, lfo, ns, sbn, nt, nh, cm_from_inch(12), cm_from_inch(20), th)),
        ("Boden 7", _preset(rh, 7, cm_from_inch(25.5), cm_from_inch(25.0), 0, sn, sb, True, ntz, frets, oh, ht, ds, df, sw, sh, cw, ch, lfo, ns, sbn, nt, nh, cm_from_inch(12), cm_from_inch(20), th)),
        ("Boden bass", _preset(rh, 4, cm_from_inch(34), cm_from_inch(32), 7, bsn, bsb, True, ntz, frets, oh, ht, ds, df, sw, sh, cw, ch, lfo, ns, sbn, nt, nh, cm_from_inch(16), cm_from_inch(20), th)),
        ("Boden bass 5 strings", _preset(rh, 5, cm_from_inch(34), cm_from_inch(32), 7, bsn, bsb, True, ntz, frets, oh, ht, ds, df, sw, sh, cw, ch, lfo, ns, sbn, nt, nh, cm_from_inch(16), cm_from_inch(20), th)),
    ]


class Fretboard:
    def __init__(self, instrument):
        inst = instrument
        first = inst.scale_length[1 if inst.right_handed else 0]
        last = inst.scale_length[0 if inst.right_handed else 1]

        max_i = inst.number_of_strings - 1
        diff = last - first
        ydiff_start = -inst.y_at_start * 2
        ydiff_bridge = -inst.y_at_bridge * 2

        self._strings = []
        for i in range(inst.number_of_strings):
            ratio = 0.5 if max_i == 0 else i / max_i
            length = first + ratio * diff
            ystart = inst.y_at_start + ratio * ydiff_start
            ybridge = inst.y_at_bridge + ratio * ydiff_bridge
            self._strings.append(String(i, length, inst.perpendicular_fret_index,
                                        ystart, ybridge, inst.has_zero_fret,
                                        inst.nut_to_zero_fret_offset,
                                        inst.number_of_frets_per_octave))

        # Global X offset = average of the per-string x_at_start, negated.
        offset = sum(s.x_at_start() for s in self._strings) / inst.number_of_strings
        for s in self._strings:
            s.set_x_offset(-offset)

        self.number_of_frets = inst.number_of_frets
        self.has_zero_fret = inst.has_zero_fret
        self.nut_to_zero_fret_offset = inst.nut_to_zero_fret_offset

        first_string = self._strings[0]
        last_string = self._strings[-1]

        if inst.number_of_strings == 1:
            first_string = String(0, self._strings[0].scale_length(),
                                  inst.perpendicular_fret_index, inst.overhangs[0], inst.overhangs[1],
                                  inst.has_zero_fret, inst.nut_to_zero_fret_offset,
                                  inst.number_of_frets_per_octave)
            last_string = String(0, self._strings[0].scale_length(),
                                 inst.perpendicular_fret_index, -inst.overhangs[2], -inst.overhangs[3],
                                 inst.has_zero_fret, inst.nut_to_zero_fret_offset,
                                 inst.number_of_frets_per_octave)

        # Fretboard area = strings extended by the overhangs at first and last fret.
        first_fret_v = Vector(first_string.point_at_fret(0), last_string.point_at_fret(0))
        last_fret_v = Vector(first_string.point_at_fret(inst.number_of_frets),
                             last_string.point_at_fret(inst.number_of_frets))
        self.first_border = first_string.line().offset2D_between(
            inst.overhangs[0], first_fret_v, inst.overhangs[1], last_fret_v)
        self.last_border = last_string.line().offset2D_between(
            -inst.overhangs[2], first_fret_v, -inst.overhangs[3], last_fret_v)

        self.first_tang_border = self.first_border.offset2D(inst.hidden_tang_length)
        self.last_tang_border = self.last_border.offset2D(-inst.hidden_tang_length)

        first_fret = 0 if self.has_zero_fret else 1
        last_fret = inst.number_of_frets + 1

        self._fret_slots = []
        self._fret_lines = []
        for fret_index in range(first_fret, last_fret):
            fret_line = Vector(first_string.point_at_fret(fret_index),
                               last_string.point_at_fret(fret_index))
            self._fret_slots.append(Vector(fret_line.intersection(self.first_tang_border),
                                           fret_line.intersection(self.last_tang_border)))
            self._fret_lines.append(Vector(fret_line.intersection(self.first_border),
                                           fret_line.intersection(self.last_border)))

        self._fret_slot_shapes = []
        for fret_index in range(first_fret, last_fret):
            point1 = first_string.point_at_fret(fret_index)
            point2 = last_string.point_at_fret(fret_index)
            fret_line = Vector(point1, point2)
            sign = 1 if point1.x <= point2.x else -1
            offset_line1 = fret_line.offset2D(sign * -inst.fret_slots_width / 2)
            offset_line2 = fret_line.offset2D(sign * inst.fret_slots_width / 2)
            self._fret_slot_shapes.append(Quad([
                offset_line1.intersection(self.first_tang_border),
                offset_line2.intersection(self.first_tang_border),
                offset_line2.intersection(self.last_tang_border),
                offset_line1.intersection(self.last_tang_border),
            ]))

        # Last fret cut = a virtual (last+1)th fret, optionally X-offset.
        p0 = first_string.point_at_fret(last_fret)
        p1 = last_string.point_at_fret(last_fret)
        last_fret_line = Vector(p0, p1)
        sign = 1 if first_string.x_at_bridge() <= last_string.x_at_bridge() else -1
        last_fret_cut = last_fret_line.offset2D(sign * inst.last_fret_cut_offset)

        nut_line = Vector(Point(first_string.x_at_nut(), first_string.y_at_start()),
                          Point(last_string.x_at_nut(), last_string.y_at_start()))

        board_cut_at_nut = nut_line.offset2D(inst.space_before_nut)
        self._board_shape = Quad([
            board_cut_at_nut.intersection(self.first_border),
            last_fret_cut.intersection(self.first_border),
            last_fret_cut.intersection(self.last_border),
            board_cut_at_nut.intersection(self.last_border),
        ])

        self._construction_distance_at_nut_side = min(self._board_shape.points[0].x,
                                                      self._board_shape.points[3].x)
        self._construction_distance_at_heel = max(self._board_shape.points[1].x,
                                                   self._board_shape.points[2].x)
        self._construction_distance_at_nut = (first_string.x_at_nut() + last_string.x_at_nut()) / 2
        self._construction_distance_at_last_fret = (
            first_string.point_at_fret(self.number_of_frets).x
            + last_string.point_at_fret(self.number_of_frets).x) / 2
        self._construction_distance_at_12th_fret = 0.0
        if self.number_of_frets > 12:
            self._construction_distance_at_12th_fret = (
                first_string.point_at_fret(12).x + last_string.point_at_fret(12).x) / 2

        nut_line_2 = nut_line.offset2D(inst.nut_thickness)
        self._nut_shape = Quad([
            nut_line_2.intersection(self.first_border),
            nut_line.intersection(self.first_border),
            nut_line.intersection(self.last_border),
            nut_line_2.intersection(self.last_border),
        ])

        external_line_1 = self.first_border.offset2D(-5)
        external_line_2 = self.last_border.offset2D(5)
        self._nut_slot_shape = Quad([
            nut_line_2.intersection(external_line_1),
            nut_line.intersection(external_line_1),
            nut_line.intersection(external_line_2),
            nut_line_2.intersection(external_line_2),
        ])

        self._strings_shape = Quad([
            first_string.point_at_nut(),
            first_string.point_at_bridge(),
            last_string.point_at_bridge(),
            last_string.point_at_nut(),
        ])

    # Accessors
    def fret_slots(self):
        return self._fret_slots

    def fret_lines(self):
        return self._fret_lines

    def fret_slot_shapes(self):
        return self._fret_slot_shapes

    def strings(self):
        return self._strings

    def construction_distance_at_nut_side(self):
        return self._construction_distance_at_nut_side

    def construction_distance_at_heel(self):
        return self._construction_distance_at_heel

    def construction_distance_at_nut(self):
        return self._construction_distance_at_nut

    def construction_distance_at_last_fret(self):
        return self._construction_distance_at_last_fret

    def construction_distance_at_12th_fret(self):
        return self._construction_distance_at_12th_fret

    def board_shape(self):
        return self._board_shape

    def nut_shape(self):
        return self._nut_shape

    def nut_slot_shape(self):
        return self._nut_slot_shape

    def strings_shape(self):
        return self._strings_shape
