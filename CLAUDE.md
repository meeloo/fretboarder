# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Project Is

Fretboarder is a C++ Fusion 360 add-in (plugin) for luthiers that generates parametric fretboard models. It ships as a `.dylib` (macOS) or `.dll` (Windows) that Fusion 360 loads as a plugin.

## Build Commands

**macOS (Xcode):**
```bash
# Build the plugin dylib
xcodebuild -project Fretboarder.xcodeproj -scheme Fretboarder -configuration Release

# Build debug
xcodebuild -project Fretboarder.xcodeproj -scheme Fretboarder -configuration Debug

# Run tests
xcodebuild test -project Fretboarder.xcodeproj -scheme Tests

# Run a single test
xcodebuild test -project Fretboarder.xcodeproj -scheme Tests -only-testing:Tests/testRightHanded
```

**Windows (Visual Studio):** Use `Fretboarder.sln` with MSBuild or Visual Studio IDE.

## Architecture

The project is split into two layers:

### Core Library (`fretboarderLib/`)
Platform-independent fretboard math, built as `libfretboarderLib.a`:
- **`Geometry.hpp/cpp`** — `Point` and `Vector` primitives with 2D line intersection and offset operations
- **`String.hpp/cpp`** — Represents one guitar string; computes fret positions along the string given scale length and perpendicular fret index
- **`Fretboard.hpp/cpp`** — Main computation class. Takes an `Instrument` struct, constructs all `String` objects, computes fret slot positions, board shape quad, nut shape, construction distances, etc.
- **`json.hpp`** — nlohmann JSON (header-only), used for `.frt` preset file serialization

Key types:
- `Instrument` struct — all configurable parameters (scale lengths, string count, fret count, overhangs, nut dimensions, radius, etc.)
- `Fretboard` class — constructed from an `Instrument`, exposes `fret_slots()`, `fret_lines()`, `board_shape()`, `strings()`, etc.
- `Preset` — named `Instrument` with built-in presets (Telecaster, Stratocaster, Jazz Bass, Boden multiscale, etc.)
- `.frt` files — JSON format, load/save via `Instrument::load()` / `Instrument::save()`

### Fusion 360 UI Layer (`sources/`)
Autodesk Fusion 360 API bindings; only compiles with the Fusion 360 SDK headers:
- **`Fretboarder.cpp/h`** — Plugin entry point; `createFretboard()` drives 3D model generation
- **`CommandCreatedEventHandler`** — Sets up the command dialog
- **`OnInputChangedEventHander`** — Reacts to UI parameter changes
- **`OnExecutePreviewEventHandler`** — Drives the 2D preview sketch in the dialog
- **`Instruments+Inputs`** — Maps Fusion 360 command inputs to/from `Instrument` struct
- **`SketchHelpers`** / **`UIHelpers`** — Wrappers around Fusion 360 sketch and UI APIs

### Tests (`Tests/`)
Objective-C++ XCTest bundle (`Tests.mm`). Tests cover geometry math, `String` fret position calculations, and `Fretboard` construction. Test fixture presets live in `Tests/boards/*.frt`.

## Coordinate System & Units

- All dimensions are in **centimeters**
- X axis runs along the neck: +X toward the body, −X toward the headstock
- Y axis runs across strings: +Y toward the first string (high E on a right-handed guitar)
- The X origin is placed at the perpendicular fret (fret 0 = nut on standard guitars; configurable for multiscale instruments)
