//
//  fretboard.cpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 07/04/2020.
//  Copyright © 2020 Autodesk. All rights reserved.
//

#include "fretboard.hpp"

//# X origin is where we have the first perpendicular fret of the fretboard
//# on a standard guitar it's the nut, because all frets are perpendicular to the center line of the fretboard.
//# on a fanned fret guitar, it could be 0, but it could be 5, 7, 12 or anything else.
//# The neck goes along the X axis, +X points toward the body of the instrument.
//# -X points toward the headstock.
//# +Y points to the first string of the instrument (for a right handed guitar it would be high E)
//# -Y points to the last string of the instrument (for a right handed guitar it would be low E)


class Fret {
}

class FretBoard {
#if 0
    def __init__(self, config):
        config = config.instrument
        self.first_string = String(
            index=0,
            scale_length=config.scale_length[0],
            perpendicular_fret_index=config.fretboard.perpendicular_fret_index,
            y_at_start=config.fretboard.string_spacing.at_nut / 2,
            y_at_bridge=config.fretboard.string_spacing.at_bridge / 2,
            has_zero_fret=config.fretboard.has_zero_fret,
            nut_to_zero_fret_offset=config.fretboard.nut_to_zero_fret_offset,
            number_of_frets_per_octave=config.fretboard.number_of_frets_per_octave,
        )
        self.last_string = String(
            index=config.number_of_strings - 1,
            scale_length=config.scale_length[-1],
            perpendicular_fret_index=config.fretboard.perpendicular_fret_index,
            y_at_start=-config.fretboard.string_spacing.at_nut / 2,
            y_at_bridge=-config.fretboard.string_spacing.at_bridge / 2,
            has_zero_fret=config.fretboard.has_zero_fret,
            nut_to_zero_fret_offset=config.fretboard.nut_to_zero_fret_offset,
            number_of_frets_per_octave=config.fretboard.number_of_frets_per_octave,
        )

        self.number_of_frets = int(config.fretboard.number_of_frets)

        self.has_zero_fret = int(config.fretboard.has_zero_fret)
        self.nut_to_zero_fret_offset = int(config.fretboard.nut_to_zero_fret_offset)

        self.first_border = self.first_string.line.offset(config.fretboard.overhang)
        self.last_border = self.last_string.line.offset(-config.fretboard.overhang)

        self.first_tang_border = self.first_border.offset(
            -config.fretboard.fret_slots.hidden_tang_length
        )
        self.last_tang_border = self.last_border.offset(
            config.fretboard.fret_slots.hidden_tang_length
        )

        self.fret_slots = []
        first_fret = 1
        if self.has_zero_fret:
            first_fret = 0
        last_fret = int(config.fretboard.number_of_frets) + 1
        for fret_index in range(first_fret, last_fret):
            fret_line = StraightLine.from_points(
                self.first_string.point_at_fret(fret_index),
                self.last_string.point_at_fret(fret_index),
            )
            self.fret_slots.append(
                Vector(
                    fret_line.intersection(self.first_tang_border),
                    fret_line.intersection(self.last_tang_border),
                )
            )

        self.fret_slot_shapes = []
        for fret_index in range(first_fret, last_fret):
            point1 = self.first_string.point_at_fret(fret_index)
            point2 = self.last_string.point_at_fret(fret_index)
            fret_line = StraightLine.from_points(point1, point2)
            sign = 1 if point1.x <= point2.x else -1
            offset_line1 = fret_line.offset(
                sign * -config.fretboard.fret_slots.width / 2
            )
            offset_line2 = fret_line.offset(
                sign * config.fretboard.fret_slots.width / 2
            )
            self.fret_slot_shapes.append(
                [
                    offset_line1.intersection(self.first_tang_border),
                    offset_line2.intersection(self.first_tang_border),
                    offset_line2.intersection(self.last_tang_border),
                    offset_line1.intersection(self.last_tang_border),
                ]
            )

        # last fret cut is like a last+1th fret. If you have 22 frets, the cut is at a virtual 23th fret.
        # you can use last_fret_cut_offset to add an X offset to the cut.
        last_fret_line = StraightLine.from_points(
            self.first_string.point_at_fret(last_fret),
            self.last_string.point_at_fret(last_fret),
        )
        sign = (
            1 if self.first_string.x_at_bridge <= self.last_string.x_at_bridge else -1
        )
        self.last_fret_cut = last_fret_line.offset(
            sign * config.fretboard.last_fret_cut_offset
        )

        nut_sign = 1 if self.first_string.x_at_nut > self.last_string.x_at_nut else -1
        nut_line = StraightLine.from_points(
            Point(self.first_string.x_at_nut, self.first_string.y_at_start),
            Point(self.last_string.x_at_nut, self.last_string.y_at_start),
        )

        # board shape
        board_cut_at_nut = nut_line.offset(nut_sign * config.fretboard.space_before_nut)
        self.board_shape = [
            board_cut_at_nut.intersection(self.first_border),
            self.last_fret_cut.intersection(self.first_border),
            self.last_fret_cut.intersection(self.last_border),
            board_cut_at_nut.intersection(self.last_border),
        ]

        self.construction_distance_at_nut_side = min(
            self.board_shape[0].x, self.board_shape[3].x
        )
        self.construction_distance_at_heel = max(
            self.board_shape[1].x, self.board_shape[2].x
        )
        self.construction_distance_at_nut = mean([
            self.first_string.x_at_nut, self.last_string.x_at_nut
        ])
        self.construction_distance_at_last_fret = mean([
            self.first_string.point_at_fret(self.number_of_frets).x,
            self.last_string.point_at_fret(self.number_of_frets).x,
        ])

        if self.number_of_frets > 12:
            self.construction_distance_at_12th_fret = mean([
                self.first_string.point_at_fret(12).x,
                self.last_string.point_at_fret(12).x,
            ])

        # nut shape
        nut_line_2 = nut_line.offset(nut_sign * config.fretboard.nut.thickness)
        self.nut_shape = [
            nut_line_2.intersection(self.first_border),
            nut_line.intersection(self.first_border),
            nut_line.intersection(self.last_border),
            nut_line_2.intersection(self.last_border),
        ]

        external_line_1 = self.first_border.offset(Decimal("5"))
        external_line_2 = self.last_border.offset(Decimal("-5"))
        self.nut_slot_shape = [
            nut_line_2.intersection(external_line_1),
            nut_line.intersection(external_line_1),
            nut_line.intersection(external_line_2),
            nut_line_2.intersection(external_line_2),
        ]

        self.strings_shape = [
            self.first_string.point_at_nut(),
            self.first_string.point_at_bridge(),
            self.last_string.point_at_bridge(),
            self.last_string.point_at_nut(),
        ]

#endif
}
