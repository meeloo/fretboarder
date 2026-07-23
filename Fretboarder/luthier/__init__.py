"""Platform-independent fretboard math (Python port of fretboarderLib).

No Fusion dependencies — importable and testable with plain CPython.
"""
from .geometry import Point, Vector
from .strings import String
from .fretboard import (
    Instrument, Fretboard, Quad, presets,
    mm_from_inch, cm_from_inch,
    SINGLE, NUT_AND_LAST_FRET, ALL,
)

__all__ = [
    "Point", "Vector", "String", "Instrument", "Fretboard", "Quad",
    "presets", "mm_from_inch", "cm_from_inch",
    "SINGLE", "NUT_AND_LAST_FRET", "ALL",
]
