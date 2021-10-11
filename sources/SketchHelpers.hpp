//
//  SketchHelpers.hpp
//  Fretboarder
//
//  Created by Sebastien Metrot on 10/10/2021.
//  Copyright © 2021 Autodesk. All rights reserved.
//

#ifndef SketchHelpers_hpp
#define SketchHelpers_hpp

Ptr<Point3D> create_point(Point point);
void create_closed_polygon(const Ptr<SketchLines>& sketch_lines, const Quad& shape);
void create_line(const Ptr<SketchLines>& sketch_lines, const Vector& vector);
Ptr<Sketch> create_radius_circle(const Ptr<Component>& component, const Ptr<ConstructionPlane>& plane, double radius, double thickness);
double YOnCircleGivenX(double x, Point center, double radius);
Ptr<Sketch> create_fretwire_profile(const Instrument& instrument, const Fretboard& fretboard, int fretIndex, const Ptr<Component>& component, const Ptr<Path>& path);
Ptr<Sketch> create_frettang_profile(const Instrument& instrument, const Fretboard& fretboard, int fretIndex, const Ptr<Component>& component, const Ptr<Path>& path);
Ptr<Path> fill_path_from_profile(const Ptr<Component>& component, const Ptr<Path>& path, const Ptr<SketchEntity>& profile, int index, std::vector<Ptr<Sketch>>& profiles);
#endif /* SketchHelpers_hpp */
