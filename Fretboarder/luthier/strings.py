"""One instrument string: computes fret positions along it (port of fretboarderLib/String).

Fret spacing is equal temperament: distance_from_bridge = scale / 2^(fret/frets_per_octave).
Negative perpendicular-fret indices support fanned/multiscale boards.
"""
import math

from .geometry import Point, Vector


class String:
    def __init__(self, index, scale_length, perpendicular_fret_index,
                 y_at_start, y_at_bridge, has_zero_fret,
                 nut_to_zero_fret_offset, number_of_frets_per_octave=12):
        self._index = index
        self._scale_length = scale_length
        self._perpendicular_fret_index = perpendicular_fret_index
        self._x_at_start = 0.0
        self._y_at_start = y_at_start
        self._x_at_bridge = 0.0
        self._y_at_bridge = y_at_bridge
        self._number_of_frets_per_octave = number_of_frets_per_octave
        self._x_at_nut = 0.0
        self._x_offset = 0.0

        self._nut_to_zero_fret_offset = nut_to_zero_fret_offset if has_zero_fret else 0.0

        if perpendicular_fret_index < 0:
            self._perpendicular_fret_from_start = self.distance_from_start(perpendicular_fret_index)
            virtual_length = self.distance_from_bridge(perpendicular_fret_index)

            y_intersect = (y_at_start
                           + self._perpendicular_fret_from_start
                           * (y_at_bridge - y_at_start) / scale_length)

            virtual_string = String(index, virtual_length, 0, y_intersect, y_at_bridge,
                                    has_zero_fret, nut_to_zero_fret_offset,
                                    number_of_frets_per_octave)

            self._x_at_start = virtual_string.distance_from_start(-perpendicular_fret_index)
            self._x_at_nut = self._x_at_start - self._nut_to_zero_fret_offset
            self._x_at_bridge = virtual_string.x_at_bridge()
        else:
            self._perpendicular_fret_from_start = self.distance_from_start(perpendicular_fret_index)

            y_intersect = (y_at_start
                           + self._perpendicular_fret_from_start
                           * (y_at_bridge - y_at_start) / scale_length)

            # C++ has `sign = _perpendicular_fret_from_start < 0 ? 1 : 1;` (always 1).
            self._x_at_start = -math.sqrt(self._perpendicular_fret_from_start ** 2
                                          - (y_intersect - y_at_start) ** 2)
            self._x_at_nut = self._x_at_start - self._nut_to_zero_fret_offset

            xab = math.sqrt(scale_length ** 2 - (y_at_bridge - y_at_start) ** 2)
            xab -= abs(self._x_at_start)
            self._x_at_bridge = xab

    def distance_from_start(self, fret_index):
        return self._scale_length - self.distance_from_bridge(fret_index)

    def distance_from_bridge(self, fret_index):
        if fret_index == 100:
            return 0.0
        l = self._scale_length
        i = fret_index
        if i < 0:
            # Rescale onto a longer virtual string so a negative index is valid.
            s = 1 + int(-i / 12)
            l = l * (1 << s)
            i = 12 * s + i
        return l / (2.0 ** (i / self._number_of_frets_per_octave))

    def point_at_fret(self, fret_index):
        d = self.distance_from_start(fret_index)
        t = d / self._scale_length
        x = t * (self.x_at_bridge() - self.x_at_start())
        y = t * (self.y_at_bridge() - self.y_at_start())
        return Point(self.x_at_start() + x, self.y_at_start() + y)

    def point_at_nut(self):
        return Point(self.x_at_start(), self.y_at_start())

    def point_at_bridge(self):
        return Point(self.x_at_bridge(), self.y_at_bridge())

    def line(self):
        return Vector(self.point_at_bridge(), self.point_at_nut())

    # Accessors
    def index(self):
        return self._index

    def scale_length(self):
        return self._scale_length

    def perpendicular_fret_index(self):
        return self._perpendicular_fret_index

    def x_at_start(self):
        return self._x_offset + self._x_at_start

    def x_at_nut(self):
        return self._x_offset + self._x_at_nut

    def y_at_start(self):
        return self._y_at_start

    def x_at_bridge(self):
        return self._x_offset + self._x_at_bridge

    def y_at_bridge(self):
        return self._y_at_bridge

    def number_of_frets_per_octave(self):
        return self._number_of_frets_per_octave

    def nut_to_zero_fret_offset(self):
        return self._nut_to_zero_fret_offset

    def x_offset(self):
        return self._x_offset

    def set_x_offset(self, v):
        self._x_offset = v
