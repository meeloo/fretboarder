"""2D/3D point and line-segment primitives (port of fretboarderLib/Geometry).

All fretboard math lives in the XY plane (z = 0); the 3D forms are kept so the
line-line intersection matches the C++ exactly.
"""
import math


class Point:
    __slots__ = ("x", "y", "z")

    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x = float(x)
        self.y = float(y)
        self.z = float(z)

    def distance_from(self, p):
        dx = self.x - p.x
        dy = self.y - p.y
        dz = self.z - p.z
        return math.sqrt(dx * dx + dy * dy + dz * dz)

    def __add__(self, p):
        return Point(self.x + p.x, self.y + p.y, self.z + p.z)

    def __sub__(self, p):
        return Point(self.x - p.x, self.y - p.y, self.z - p.z)

    def cross(self, p):
        return Point(self.y * p.z - p.y * self.z,
                     self.z * p.x - p.z * self.x,
                     self.x * p.y - p.x * self.y)

    def scaled(self, v):
        return Point(self.x * v, self.y * v, self.z * v)

    def __eq__(self, p):
        return isinstance(p, Point) and self.x == p.x and self.y == p.y and self.z == p.z

    def __repr__(self):
        return "Point(%g, %g, %g)" % (self.x, self.y, self.z)


def _dot(u, v):
    return u.x * v.x + u.y * v.y + u.z * v.z


def _norm2(v):
    return v.x * v.x + v.y * v.y + v.z * v.z


def _norm(v):
    return math.sqrt(_norm2(v))


class Vector:
    """A line segment defined by two points (used as an infinite line for intersections)."""
    __slots__ = ("point1", "point2")

    def __init__(self, p1=None, p2=None):
        self.point1 = p1 if p1 is not None else Point()
        self.point2 = p2 if p2 is not None else Point()

    def intersection(self, other):
        # http://mathworld.wolfram.com/Line-LineIntersection.html
        # 3D form; also correct in 2D when z components are 0.
        da = self.point2 - self.point1
        db = other.point2 - other.point1
        dc = other.point1 - self.point1
        cross_da_db = da.cross(db)
        s = _dot(dc.cross(db), cross_da_db) / _norm2(cross_da_db)
        return self.point1 + da.scaled(s)

    def offset2D(self, offset):
        """A parallel line at orthogonal distance abs(offset); +offset is above."""
        x = self.point2.x - self.point1.x
        y = self.point2.y - self.point1.y
        perp = Point(y, -x, 0.0)
        n = _norm(perp)
        if n == 0:
            return Vector(self.point1, self.point2)
        off = offset / n
        perp = Point(perp.x * off, perp.y * off, 0.0)
        return Vector(self.point1 + perp, self.point2 + perp)

    def offset2D_between(self, offset0, vector0, offset1, vector1):
        """C++ four-argument offset2D: offset each end along a bounding line."""
        p0 = self.intersection(vector0)
        p1 = self.intersection(vector1)
        e0 = vector0.sized_vector(offset0)
        e1 = vector1.sized_vector(offset1)
        return Vector(p0 + e0.point2, p1 + e1.point2)

    def unit_vector(self):
        return self.sized_vector(1.0)

    def sized_vector(self, size):
        d = self.point1.distance_from(self.point2)
        if d == 0:
            return Vector(self.point1, self.point1)
        n = size / d
        return Vector(Point(),
                      Point(n * (self.point1.x - self.point2.x),
                            n * (self.point1.y - self.point2.y),
                            n * (self.point1.z - self.point2.z)))
