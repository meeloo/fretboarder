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

Ptr<Sketch> create_fretwire_profile(const Instrument& instrument, const Fretboard& fretboard, int fretIndex, const Ptr<Component>& component, const Ptr<Path>& path) {
    // Create a construction plane at the end of the path.
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

    auto sketch_curves = fret_wire_profile->sketchCurves();
    CHECK(sketch_curves, nullptr);

    auto sketchLines = sketch_curves->sketchLines();
    CHECK(sketchLines, nullptr);
    auto sketchArcs = sketch_curves->sketchArcs();
    CHECK(sketchArcs, nullptr);
    auto crownW = instrument.fret_crown_width / 2;
    auto crownH = instrument.fret_crown_height;
    auto arc = sketchArcs->addByThreePoints(create_point(Point(crownW, 0, 0)), create_point(Point(0, (0 + crownH), 0)), create_point(Point(-crownW, 0, 0)));
    CHECK(arc, nullptr);
    sketchLines->addByTwoPoints(create_point(Point(crownW, 0, 0)), create_point(Point(-crownW, 0, 0)));

    return fret_wire_profile;
}

Ptr<Sketch> create_frettang_profile(const Instrument& instrument, const Fretboard& fretboard, int fretIndex, const Ptr<Component>& component, const Ptr<Path>& path) {
    // Create a construction plane at the end of the path.
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

    auto sketch_curves = fret_tang_profile->sketchCurves();
    CHECK(sketch_curves, nullptr);

    auto sketchLines = sketch_curves->sketchLines();
    CHECK(sketchLines, nullptr);
    auto sketchArcs = sketch_curves->sketchArcs();
    CHECK(sketchArcs, nullptr);
    auto tangW = instrument.fret_slots_width / 2;
    auto tangH = instrument.fret_slots_height;
    sketchLines->addTwoPointRectangle(Point3D::create((-tangW) * 0.1, 0.01 * 0.1, 0), Point3D::create(tangW * 0.1, -tangH * 0.1, 0));
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


