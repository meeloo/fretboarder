//
//  SketchHelpers.cpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#include "Fretboarder.h"
#include "SketchHelpers.hpp"

Ptr<Point3D> create_point(Point point) {
    return Point3D::create(0.1 * point.x, 0.1 * point.y, 0.1 * point.z);
}

void create_closed_polygon(const Ptr<SketchLines>& sketch_lines, const Quad& shape) {
    CHECK2(sketch_lines);
    for (int index = 1; index < 4;  index++) {
        sketch_lines->addByTwoPoints(create_point(shape.points[index - 1]), create_point(shape.points[index]));
    }
    // close
    sketch_lines->addByTwoPoints(create_point(shape.points[3]), create_point(shape.points[0]));
}

void create_line(const Ptr<SketchLines>& sketch_lines, const Vector& vector) {
    CHECK2(sketch_lines);
    sketch_lines->addByTwoPoints(create_point(vector.point1), create_point(vector.point2));
}

Ptr<Sketch> create_radius_circle(const Ptr<Component>& component, const Ptr<ConstructionPlane>& plane, double radius, double thickness) {
    CHECK(component, nullptr);
    CHECK(plane, nullptr);
    auto sketch = component->sketches()->add(plane);
    CHECK(sketch, nullptr);
    sketch->isComputeDeferred(true);
    auto circles = sketch->sketchCurves()->sketchCircles();
    CHECK(circles, nullptr);
    auto center = Point3D::create(-(thickness - radius) * 0.1, 0.0, 0.0);
    CHECK(center, nullptr);
    circles->addByCenterRadius(center, radius * 0.1);
    sketch->isComputeDeferred(false);
    return sketch;
}

double YOnCircleGivenX(double x, Point center, double radius) {
    auto K = (x - center.x);
    return center.y - sqrt(radius * radius + K * K);
}

// A fret cross-section is drawn as a flat 2D sketch on a construction plane that
// Fusion builds perpendicular to the fret path (setByDistanceOnPath). Fusion picks
// that plane's in-plane X/Y axes automatically from the path tangent, and that
// choice is NOT stable across Fusion versions / parametric vs. direct evaluation —
// which is exactly what made every fret and tang come out rotated 90 degrees.
//
// To be orientation-independent we don't draw in the plane's arbitrary axes. We
// compute the frame we actually want in world space:
//   up     = world +Z projected into the plane  -> "out of / into the board"
//   across = tangent x up                        -> the fret-width direction
// and place each profile point via the sketch's own modelToSketchSpace mapping.

static bool compute_profile_frame(const Ptr<ConstructionPlane>& plane,
                                  Ptr<Point3D>& origin,
                                  Ptr<Vector3D>& up,
                                  Ptr<Vector3D>& across) {
    CHECK(plane, false);
    auto geom = plane->geometry();
    CHECK(geom, false);
    origin = geom->origin();
    auto normal = geom->normal();   // tangent to the fret path at this point
    CHECK(origin, false);
    CHECK(normal, false);
    normal->normalize();

    auto z = Vector3D::create(0, 0, 1);
    double zDotN = z->dotProduct(normal);
    auto along = normal->copy();
    along->scaleBy(zDotN);
    up = z->copy();
    up->subtract(along);            // up = Z - (Z . N) N  -> Z projected into the plane
    if (up->length() < 1e-6) {
        // Degenerate: the path tangent is vertical. Fall back to the plane's own axis.
        up = geom->vDirection();
    }
    CHECK(up, false);
    up->normalize();

    across = normal->crossProduct(up);
    CHECK(across, false);
    across->normalize();
    return true;
}

// Map a profile coordinate (height along `up`, width along `across`, both in mm)
// to a point expressed in the sketch's local coordinate space.
static Ptr<Point3D> profile_point(const Ptr<Sketch>& sketch,
                                  const Ptr<Point3D>& origin,
                                  const Ptr<Vector3D>& up,
                                  const Ptr<Vector3D>& across,
                                  double height_mm, double width_mm) {
    auto u = up->copy();      u->scaleBy(height_mm * 0.1);   // mm -> cm (Fusion internal units)
    auto a = across->copy();  a->scaleBy(width_mm * 0.1);
    auto world = origin->copy();
    world->translateBy(u);
    world->translateBy(a);
    return sketch->modelToSketchSpace(world);
}

Ptr<Sketch> create_fretwire_profile(const Instrument& instrument, const Fretboard& fretboard, int fretIndex, const Ptr<Component>& component, const Ptr<Path>& path) {
    // Create a construction plane at the start of the path.
    CHECK(component, nullptr);
    CHECK(path, nullptr);
    Ptr<ConstructionPlaneInput> planeInput = component->constructionPlanes()->createInput();
    CHECK(planeInput, nullptr);
    planeInput->setByDistanceOnPath(path, ValueInput::createByReal(0));
    Ptr<ConstructionPlane> profPlane = component->constructionPlanes()->add(planeInput);
    CHECK(profPlane, nullptr);
    auto fret_wire_profile = component->sketches()->add(profPlane);
    CHECK(fret_wire_profile, nullptr);
    fret_wire_profile->name("Fret Wire Profile");

    Ptr<Point3D> origin;
    Ptr<Vector3D> up, across;
    CHECK(compute_profile_frame(profPlane, origin, up, across), nullptr);

    auto sketch_curves = fret_wire_profile->sketchCurves();
    CHECK(sketch_curves, nullptr);

    auto sketchLines = sketch_curves->sketchLines();
    CHECK(sketchLines, nullptr);
    auto sketchArcs = sketch_curves->sketchArcs();
    CHECK(sketchArcs, nullptr);
    auto crownW = instrument.fret_crown_width / 2;
    auto crownH = instrument.fret_crown_height;

    // Crown: a dome rising along +up (away from the board surface), symmetric across
    // the fret width. The base chord sits on the board top surface (height 0).
    auto p_start = profile_point(fret_wire_profile, origin, up, across, 0,      crownW);
    auto p_apex  = profile_point(fret_wire_profile, origin, up, across, crownH, 0);
    auto p_end   = profile_point(fret_wire_profile, origin, up, across, 0,     -crownW);
    auto arc = sketchArcs->addByThreePoints(p_start, p_apex, p_end);
    CHECK(arc, nullptr);
    sketchLines->addByTwoPoints(p_start, p_end);

    return fret_wire_profile;
}

Ptr<Sketch> create_frettang_profile(const Instrument& instrument, const Fretboard& fretboard, int fretIndex, const Ptr<Component>& component, const Ptr<Path>& path) {
    // Create a construction plane at the start of the path.
    CHECK(component, nullptr);
    CHECK(path, nullptr);
    Ptr<ConstructionPlaneInput> planeInput = component->constructionPlanes()->createInput();
    CHECK(planeInput, nullptr);
    planeInput->setByDistanceOnPath(path, ValueInput::createByReal(0));
    Ptr<ConstructionPlane> profPlane = component->constructionPlanes()->add(planeInput);
    CHECK(profPlane, nullptr);
    auto fret_tang_profile = component->sketches()->add(profPlane);
    CHECK(fret_tang_profile, nullptr);
    fret_tang_profile->name("Fret Tang Profile");

    Ptr<Point3D> origin;
    Ptr<Vector3D> up, across;
    CHECK(compute_profile_frame(profPlane, origin, up, across), nullptr);

    auto sketch_curves = fret_tang_profile->sketchCurves();
    CHECK(sketch_curves, nullptr);

    auto sketchLines = sketch_curves->sketchLines();
    CHECK(sketchLines, nullptr);
    auto tangW = instrument.fret_slots_width / 2;
    auto tangH = instrument.fret_slots_height;

    // Tang: a rectangle that starts just above the board surface (+0.01 mm, to
    // guarantee a clean boolean cut) and drops straight down into the board (-tangH
    // along up), spanning the slot width across the fret. Built from four explicit
    // corners/lines so the rectangle stays oriented to up/across regardless of the
    // sketch's own axes (addTwoPointRectangle would axis-align to the sketch frame).
    auto c_top_left     = profile_point(fret_tang_profile, origin, up, across,  0.01, -tangW);
    auto c_top_right    = profile_point(fret_tang_profile, origin, up, across,  0.01,  tangW);
    auto c_bottom_right = profile_point(fret_tang_profile, origin, up, across, -tangH,  tangW);
    auto c_bottom_left  = profile_point(fret_tang_profile, origin, up, across, -tangH, -tangW);
    sketchLines->addByTwoPoints(c_top_left, c_top_right);
    sketchLines->addByTwoPoints(c_top_right, c_bottom_right);
    sketchLines->addByTwoPoints(c_bottom_right, c_bottom_left);
    sketchLines->addByTwoPoints(c_bottom_left, c_top_left);
    return fret_tang_profile;
}

Ptr<Path> fill_path_from_profile(const Ptr<Component>& component, const Ptr<Path>& path, const Ptr<SketchEntity>& profile, int index, std::vector<Ptr<Sketch>>& profiles) {
    CHECK(component, nullptr);
    CHECK(profile, nullptr);
    Ptr<Path> _path = path;
    auto fret_profile = component->sketches()->add(component->yZConstructionPlane());
    CHECK(fret_profile, nullptr);
    profiles.push_back(fret_profile);
    auto arc3D = profile->cast<SketchArc>();
    if (arc3D) {
        auto nurbsCurve = arc3D->geometry()->asNurbsCurve();
        std::stringstream str;
        fret_profile->sketchCurves()->sketchFittedSplines()->addByNurbsCurve(nurbsCurve);
    }
    auto earc3D = profile->cast<SketchEllipticalArc>();
    if (earc3D) {
        auto nurbsCurve = earc3D->geometry()->asNurbsCurve();
        std::stringstream str;
        fret_profile->sketchCurves()->sketchFittedSplines()->addByNurbsCurve(nurbsCurve);
    }

    if (!_path) {
        _path = component->features()->createPath(fret_profile->sketchCurves()->item(0), true);
        CHECK(_path, nullptr);
    } else {
        auto res = _path->addCurves(fret_profile->sketchCurves()->item(0), connectedChainedCurves);
        CHECK(res, nullptr);
    }
    for (int c = 1; c < fret_profile->sketchCurves()->count(); c++) {
        auto res = _path->addCurves(fret_profile->sketchCurves()->item(c), connectedChainedCurves);
        CHECK(res, nullptr);
    }

    return _path;
}


