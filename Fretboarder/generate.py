"""Build the fretboard 3D geometry in Fusion from an Instrument
(port of sources/Fretboarder.cpp createFretboard + SketchHelpers.cpp).

The Instrument passed in must already be scaled to millimetres (caller does
instrument.scale(10)); create_point converts mm -> cm (Fusion internal units).
"""
import adsk.core
import adsk.fusion

from luthier import Fretboard, Vector

WALNUT_LIB = "C1EEA57C-3F56-45FC-B8CB-A9EC46A9994C"  # Fusion 360 Material Library
WALNUT_MAT = "PrismMaterial-271"
CHROME_MAT = "PrismMaterial-069"


def create_point(p):
    return adsk.core.Point3D.create(0.1 * p.x, 0.1 * p.y, 0.1 * p.z)


def create_closed_polygon(sketch_lines, quad):
    pts = quad.points
    for i in range(1, 4):
        sketch_lines.addByTwoPoints(create_point(pts[i - 1]), create_point(pts[i]))
    sketch_lines.addByTwoPoints(create_point(pts[3]), create_point(pts[0]))


def create_line(sketch_lines, vector):
    sketch_lines.addByTwoPoints(create_point(vector.point1), create_point(vector.point2))


def create_radius_circle(component, plane, radius, thickness):
    sketch = component.sketches.add(plane)
    sketch.isComputeDeferred = True
    center = adsk.core.Point3D.create(-(thickness - radius) * 0.1, 0.0, 0.0)
    sketch.sketchCurves.sketchCircles.addByCenterRadius(center, radius * 0.1)
    sketch.isComputeDeferred = False
    return sketch


def y_on_circle_given_x(x, center, radius):
    import math
    k = x - center.x
    return center.y - math.sqrt(radius * radius + k * k)


def _offset_plane(component, base_plane, offset_cm, name):
    planes = component.constructionPlanes
    inp = planes.createInput()
    inp.setByOffset(base_plane, adsk.core.ValueInput.createByReal(offset_cm))
    plane = planes.add(inp)
    plane.name = name
    return plane


def create_fretboard(design, instrument, component=None):
    """Build the fretboard geometry into `component` (root by default). Returns
    (component, main_body). `instrument` must be millimetre-scaled."""
    if component is None:
        component = design.rootComponent
    fretboard = Fretboard(instrument)

    features = component.features

    # Strings area sketch (hidden helper polygon).
    strings_area = component.sketches.add(component.xYConstructionPlane)
    strings_area.name = "Strings area"
    strings_area.isComputeDeferred = True
    create_closed_polygon(strings_area.sketchCurves.sketchLines, fretboard.strings_shape())
    strings_area.isComputeDeferred = False
    strings_area.isVisible = False

    # Contour sketch (board outline).
    contour = component.sketches.add(component.xYConstructionPlane)
    contour.name = "Contour"
    contour.isComputeDeferred = True
    create_closed_polygon(contour.sketchCurves.sketchLines, fretboard.board_shape())
    contour.isComputeDeferred = False

    # Fret-slot and fret-line sketches, on a raised plane (hidden helpers for the frets stage).
    slots_plane = _offset_plane(component, component.xYConstructionPlane,
                                (instrument.fretboard_thickness + 10.0) * 0.1, "Fret slots plane")
    fret_slots_sketch = component.sketches.add(slots_plane)
    fret_slots_sketch.name = "Fret slots as lines"
    fret_slots_sketch.isComputeDeferred = True
    for v in fretboard.fret_slots():
        create_line(fret_slots_sketch.sketchCurves.sketchLines, v)
    fret_slots_sketch.isComputeDeferred = False
    fret_slots_sketch.isVisible = False

    fret_lines_sketch = component.sketches.add(slots_plane)
    fret_lines_sketch.name = "Fret lines as lines"
    fret_lines_sketch.isComputeDeferred = True
    for v in fretboard.fret_lines():
        create_line(fret_lines_sketch.sketchCurves.sketchLines, v)
    fret_lines_sketch.isComputeDeferred = False
    fret_lines_sketch.isVisible = False

    # Construction planes across the neck.
    yz = component.yZConstructionPlane
    plane_nut_side = _offset_plane(component, yz, fretboard.construction_distance_at_nut_side() * 0.1, "Nut Side")
    _offset_plane(component, yz, fretboard.construction_distance_at_nut() * 0.1, "Nut")
    _offset_plane(component, yz, fretboard.construction_distance_at_last_fret() * 0.1, "Last Fret")
    plane_heel = _offset_plane(component, yz, fretboard.construction_distance_at_heel() * 0.1, "Heel Side")
    _offset_plane(component, yz, fretboard.construction_distance_at_12th_fret() * 0.1, "12th Fret")

    # Radiused top: loft between a radius circle at the nut side and one at the heel.
    radius_1 = create_radius_circle(component, plane_nut_side, instrument.radius_at_nut, instrument.fretboard_thickness)
    radius_4 = create_radius_circle(component, plane_heel, instrument.radius_at_last_fret, instrument.fretboard_thickness)

    loft_input = features.loftFeatures.createInput(adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
    loft_input.loftSections.add(radius_1.profiles.item(0))
    loft_input.loftSections.add(radius_4.profiles.item(0))
    loft_input.isSolid = True
    loft = features.loftFeatures.add(loft_input)
    main_body = loft.bodies.item(0)
    main_body.name = "Main body"

    app = adsk.core.Application.get()
    matlib = app.materialLibraries.itemById(WALNUT_LIB)
    if matlib:
        walnut = matlib.materials.itemById(WALNUT_MAT)
        if walnut:
            main_body.material = walnut

    radius_1.isVisible = False
    radius_4.isVisible = False

    # Trim the plank to the board contour by intersecting an extrude of the contour.
    features.extrudeFeatures.addSimple(
        contour.profiles.item(0),
        adsk.core.ValueInput.createByReal(instrument.fretboard_thickness),
        adsk.fusion.FeatureOperations.IntersectFeatureOperation)

    # Nut slot.
    if instrument.carve_nut_slot:
        nut_sketch = component.sketches.add(component.xYConstructionPlane)
        nut_sketch.name = "Nut"
        nut_sketch.isComputeDeferred = True
        create_closed_polygon(nut_sketch.sketchCurves.sketchLines, fretboard.nut_slot_shape())
        nut_sketch.isComputeDeferred = False
        nut_extrude = features.extrudeFeatures.addSimple(
            nut_sketch.profiles.item(0),
            adsk.core.ValueInput.createByReal(instrument.fretboard_thickness * 0.1),
            adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        nut_body = nut_extrude.bodies.item(0)
        transform = adsk.core.Matrix3D.create()
        transform.translation = adsk.core.Vector3D.create(0.0, 0.0, instrument.nut_height_under * 0.1)
        items = adsk.core.ObjectCollection.create()
        items.add(nut_body)
        move_input = features.moveFeatures.createInput(items, transform)
        features.moveFeatures.add(move_input)
        items = adsk.core.ObjectCollection.create()
        items.add(nut_body)
        combine_input = features.combineFeatures.createInput(main_body, items)
        combine_input.operation = adsk.fusion.FeatureOperations.CutFeatureOperation
        features.combineFeatures.add(combine_input)

    _create_frets(component, main_body, fretboard, instrument, fret_slots_sketch, fret_lines_sketch)
    return component, main_body


def _top_surface(component):
    ents = component.findBRepUsingRay(
        adsk.core.Point3D.create(1, 0, 1), adsk.core.Vector3D.create(0, 0, -1),
        adsk.fusion.BRepEntityTypes.BRepFaceEntityType)
    return ents.item(0) if ents and ents.count > 0 else None


def _compute_profile_frame(plane):
    """World-space frame of the fret cross-section: up = +Z projected into the
    plane, across = tangent x up. Orientation-independent (see PR #3 note)."""
    geom = plane.geometry
    origin = geom.origin
    normal = geom.normal  # tangent to the fret path
    normal.normalize()
    z = adsk.core.Vector3D.create(0, 0, 1)
    along = normal.copy()
    along.scaleBy(z.dotProduct(normal))
    up = z.copy()
    up.subtract(along)
    if up.length < 1e-6:
        up = geom.vDirection
    up.normalize()
    across = normal.crossProduct(up)
    across.normalize()
    return origin, up, across


def _profile_point(sketch, origin, up, across, height_mm, width_mm):
    u = up.copy()
    u.scaleBy(height_mm * 0.1)
    a = across.copy()
    a.scaleBy(width_mm * 0.1)
    world = origin.copy()
    world.translateBy(u)
    world.translateBy(a)
    return sketch.modelToSketchSpace(world)


def _profile_plane(component, path):
    inp = component.constructionPlanes.createInput()
    inp.setByDistanceOnPath(path, adsk.core.ValueInput.createByReal(0))
    return component.constructionPlanes.add(inp)


def _fretwire_profile(instrument, component, path):
    plane = _profile_plane(component, path)
    sk = component.sketches.add(plane)
    sk.name = "Fret Wire Profile"
    origin, up, across = _compute_profile_frame(plane)
    crownW = instrument.fret_crown_width / 2
    crownH = instrument.fret_crown_height
    p_start = _profile_point(sk, origin, up, across, 0, crownW)
    p_apex = _profile_point(sk, origin, up, across, crownH, 0)
    p_end = _profile_point(sk, origin, up, across, 0, -crownW)
    sk.sketchCurves.sketchArcs.addByThreePoints(p_start, p_apex, p_end)
    sk.sketchCurves.sketchLines.addByTwoPoints(p_start, p_end)
    return sk


def _frettang_profile(instrument, component, path):
    plane = _profile_plane(component, path)
    sk = component.sketches.add(plane)
    sk.name = "Fret Tang Profile"
    origin, up, across = _compute_profile_frame(plane)
    tangW = instrument.fret_slots_width / 2
    tangH = instrument.fret_slots_height
    tl = _profile_point(sk, origin, up, across, 0.01, -tangW)
    tr = _profile_point(sk, origin, up, across, 0.01, tangW)
    br = _profile_point(sk, origin, up, across, -tangH, tangW)
    bl = _profile_point(sk, origin, up, across, -tangH, -tangW)
    lines = sk.sketchCurves.sketchLines
    lines.addByTwoPoints(tl, tr)
    lines.addByTwoPoints(tr, br)
    lines.addByTwoPoints(br, bl)
    lines.addByTwoPoints(bl, tl)
    return sk


def _fill_path_from_profile(component, path, projected, profiles):
    """Rebuild a Path from a projected (curved) fret profile via a fitted spline."""
    fret_profile = component.sketches.add(component.yZConstructionPlane)
    profiles.append(fret_profile)
    arc = adsk.fusion.SketchArc.cast(projected)
    if arc:
        fret_profile.sketchCurves.sketchFittedSplines.addByNurbsCurve(arc.geometry.asNurbsCurve)
    earc = adsk.fusion.SketchEllipticalArc.cast(projected)
    if earc:
        fret_profile.sketchCurves.sketchFittedSplines.addByNurbsCurve(earc.geometry.asNurbsCurve)

    curves = fret_profile.sketchCurves
    if path is None:
        path = component.features.createPath(curves.item(0), True)
    else:
        path.addCurves(curves.item(0), adsk.fusion.ChainedCurveOptions.connectedChainedCurves)
    for c in range(1, curves.count):
        path.addCurves(curves.item(c), adsk.fusion.ChainedCurveOptions.connectedChainedCurves)
    return path


def _create_fret_element(profile_sketch, path, component, name):
    p = profile_sketch.profiles.item(0)
    inp = component.features.sweepFeatures.createInput(p, path, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
    inp.orientation = adsk.fusion.SweepOrientationTypes.PerpendicularOrientationType
    sweep = component.features.sweepFeatures.add(inp)
    for j in range(sweep.bodies.count):
        sweep.bodies.item(j).name = name
    return sweep


def _create_frets(component, main_body, fretboard, instrument, fret_slots_sketch, fret_lines_sketch):
    top = _top_surface(component)
    if not top:
        raise RuntimeError("fretboard top surface not found")

    app = adsk.core.Application.get()
    matlib = app.materialLibraries.itemById(WALNUT_LIB)
    chrome = matlib.materials.itemById(CHROME_MAT) if matlib else None

    # Build the frets in the same component (keeps everything under one custom feature).
    frets = component

    faces = [top]
    C = fret_slots_sketch.sketchCurves
    L = fret_lines_sketch.sketchCurves
    z_axis = frets.zConstructionAxis

    for i in range(C.count):
        projected_sketch = frets.sketches.add(frets.yZConstructionPlane)
        projected_sketch.name = "Fret Profile %d" % i
        profiles = []

        proj_s = projected_sketch.projectToSurface(faces, [C.item(i)], adsk.fusion.SurfaceProjectTypes.AlongVectorSurfaceProjectType, z_axis)
        path_s = _fill_path_from_profile(frets, None, proj_s[0], profiles)
        proj_l = projected_sketch.projectToSurface(faces, [L.item(i)], adsk.fusion.SurfaceProjectTypes.AlongVectorSurfaceProjectType, z_axis)
        path_l = _fill_path_from_profile(frets, None, proj_l[0], profiles)

        if not instrument.carve_fret_slots and not instrument.draw_frets:
            continue

        # Tang: sweep and cut into the plank (keeping the tang tool bodies).
        tang_profile = _frettang_profile(instrument, frets, path_s)
        fret_tang = _create_fret_element(tang_profile, path_s, frets, "fret tang %d" % i)
        tang_items = adsk.core.ObjectCollection.create()
        for j in range(fret_tang.bodies.count):
            tang_items.add(fret_tang.bodies.item(j))
        cut = frets.features.combineFeatures.createInput(main_body, tang_items)
        cut.isKeepToolBodies = True
        cut.operation = adsk.fusion.FeatureOperations.CutFeatureOperation
        frets.features.combineFeatures.add(cut)
        tang_profile.isVisible = False

        if instrument.draw_frets:
            wire_profile = _fretwire_profile(instrument, frets, path_l)
            fret = _create_fret_element(wire_profile, path_l, frets, "fret %d" % i)

            # Fillet the crown's small end edges.
            body = fret.bodies.item(0)
            fillet_input = frets.features.filletFeatures.createInput()
            edges = adsk.core.ObjectCollection.create()
            max_radius = instrument.fret_crown_height * 1.1
            for e in range(body.edges.count):
                edge = body.edges.item(e)
                g = edge.geometry
                ea = adsk.core.EllipticalArc3D.cast(g)
                if ea and ea.majorRadius < max_radius:
                    edges.add(edge)
                    continue
                a3 = adsk.core.Arc3D.cast(g)
                if a3 and a3.radius < max_radius:
                    edges.add(edge)
            if edges.count > 0:
                fillet_input.addConstantRadiusEdgeSet(edges, adsk.core.ValueInput.createByReal(0.1), False)
                fillet_input.isG2 = False
                fillet_input.isTangentChain = False
                fillet_input.isRollingBallCorner = True
                frets.features.filletFeatures.add(fillet_input)

            # Merge the tang tool bodies into the fret body (fret = crown + tang).
            merge_items = adsk.core.ObjectCollection.create()
            for j in range(fret_tang.bodies.count):
                merge_items.add(fret_tang.bodies.item(j))
            bodies = frets.bRepBodies
            merge = frets.features.combineFeatures.createInput(bodies.item(bodies.count - 1), merge_items)
            merge.isKeepToolBodies = False
            # Merge tang into the fret body. The C++ used NewBodyFeatureOperation,
            # which current Fusion rejects for a combine; Join is the intent.
            merge.operation = adsk.fusion.FeatureOperations.JoinFeatureOperation
            frets.features.combineFeatures.add(merge)

            if chrome:
                frets.bRepBodies.item(frets.bRepBodies.count - 1).material = chrome
            wire_profile.isVisible = False

        projected_sketch.isVisible = False
        for pf in profiles:
            pf.isVisible = False

    return frets
