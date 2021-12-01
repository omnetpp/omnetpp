//=========================================================================
//  GEOMETRY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_LAYOUT_GEOMETRY_H
#define __OMNETPP_LAYOUT_GEOMETRY_H

#include <cmath>
#include <vector>
#include <algorithm>
#include "common/commonutil.h"
#include "common/exception.h"
#include "layoutdefs.h"

namespace omnetpp {
namespace layout {

/**
 * A three dimensional point. Base plane means z = 0.
 */
class LAYOUT_API Pt
{
    public:
        double x;

        double y;

        double z;

    public:
        Pt() {
            assign(NaN, NaN, NaN);
        }

        Pt(double x, double y, double z) {
            assign(x, y, z);
        }

        Pt(const Pt& pt) {
            assign(pt);
        }

        Pt& assign(const Pt& pt) {
            return assign(pt.x, pt.y, pt.z);
        }

        Pt& assign(double x, double y, double z) {
            this->x = x;
            this->y = y;
            this->z = z;

            return *this;
        }

        Pt copy() const {
            return Pt(*this);
        }

        void setZero() {
            assign(0, 0, 0);
        }

        double getLength() const {
            return sqrt(x * x + y * y + z * z);
        }

        double getLengthSquare() const {
            return x * x + y * y + z * z;
        }

        double getBasePlaneProjectionLength() const {
            return sqrt(x * x + y * y);
        }

        double getBasePlaneProjectionLengthSquare() const {
            return x * x + y * y;
        }

        double getDistance(const Pt& other) const {
            return getDistance(x, y, z, other.x, other.y, other.z);
        }

        double getBasePlaneProjectionDistance(const Pt& other) const {
            return getDistance(x, y, 0, other.x, other.y, 0);
        }

        static double getDistance(double x1, double y1, double z1, double x2, double y2, double z2) {
            double dx = x1 - x2;
            double dy = y1 - y2;
            double dz = z1 - z2;

            return sqrt(dx * dx + dy * dy + dz * dz);
        }

        Pt getBasePlaneProjection() const {
            return Pt(x, y, 0);
        }

        double getBasePlaneProjectionAngle() const {
            return atan2(y, x);
        }

        void basePlaneRotate(double rotateAngle) {
            double angle = getBasePlaneProjectionAngle();

            if (!isNaN(angle)) {
                double length = getBasePlaneProjectionLength();
                angle += rotateAngle;
                x = cos(angle) * length;
                y = sin(angle) * length;
            }
        }

        Pt& basePlaneTranspose() {
            return assign(y, -x, z);
        }

        Pt& normalize() {
            double length = getLength();
            x /= length;
            y /= length;
            z /= length;

            return *this;
        }

        Pt& add(const Pt& pt) {
            return add(pt.x, pt.y, pt.z);
        }

        Pt& add(double x, double y, double z) {
            this->x += x;
            this->y += y;
            this->z += z;

            return *this;
        }

        Pt& subtract(const Pt& pt) {
            return subtract(pt.x, pt.y, pt.z);
        }

        Pt& subtract(double x, double y, double z) {
            this->x -= x;
            this->y -= y;
            this->z -= z;

            return *this;
        }

        Pt& multiply(double d) {
            x *= d;
            y *= d;
            z *= d;

            return *this;
        }

        Pt& multiply(const Pt& pt) {
            this->x *= pt.x;
            this->y *= pt.y;
            this->z *= pt.z;

            return *this;
        }

        Pt& reverse() {
            return multiply(-1);
        }

        Pt& divide(double d) {
            x /= d;
            y /= d;
            z /= d;

            return *this;
        }

        double dotProduct(Pt& other) {
            return x * other.x + y * other.y + z * other.z;
        }

        Pt& crossProduct(Pt& other) {
            assign(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);

            return *this;
        }

        void setNaNToZero() {
            if (isNaN(x))
                x = 0;

            if (isNaN(y))
                y = 0;

            if (isNaN(z))
                z = 0;
        }

        bool isZero() const {
            return x == 0 && y == 0 && z == 0;
        }

		bool isPositiveQuadrant() const {
			return x >= 0 && y >= 0;
		}

        static Pt getBasePlaneRadial(double radius, double angle) {
            return Pt(cos(angle) * radius, sin(angle) * radius, 0);
        }

        static Pt getNil() {
            return Pt(NaN, NaN, NaN);
        }

        static Pt getZero() {
            return Pt(0, 0, 0);
        }

        bool isNil() const {
            return isNaN(x) && isNaN(y) && isNaN(z);
        }

        bool isFullySpecified() const {
            return !isNaN(x) && !isNaN(y) && !isNaN(z);
        }

        static double determinant(double a1, double a2, double b1, double b2) {
            return a1 * b2 - a2 * b1;
        }

        static double determinant(double a1, double a2, double a3, double b1, double b2, double b3, double c1, double c2, double c3) {
            return a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1;
        }
};

/**
 * A three dimensional line or section defined by two points.
 */
class LAYOUT_API Ln {
    public:
        Pt begin;

        Pt end;

    public:
        Ln() {
            assign(Pt::getNil(), Pt::getNil());
        }

        Ln(double x1, double y1, double z1, double x2, double y2, double z2) {
            assign(Pt(x1, y1, z1), Pt(x2, y2, z2));
        }

        Ln(const Pt& begin, const Pt& end) {
            assign(begin, end);
        }

        Ln& assign(const Pt& begin, const Pt& end) {
            this->begin = begin;
            this->end = end;

            return *this;
        }

        Ln getBasePlaneProjection() const {
            return Ln(begin.getBasePlaneProjection(), end.getBasePlaneProjection());
        }

        Pt getClosestPoint(const Pt& pt) const {
            Pt r(pt);
            r.subtract(begin);

            Pt q = getDirectionVector();
            q.normalize();
            q.multiply(q.dotProduct(r));
            q.add(begin);

            return q;
        }

        Pt getDirectionVector() const {
            Pt v(end);
            v.subtract(begin);

            return v;
        }

        Pt basePlaneProjectionIntersect(const Ln& ln) const {
            double x1 = begin.x;
            double y1 = begin.y;
            double x2 = end.x;
            double y2 = end.y;
            double x3 = ln.begin.x;
            double y3 = ln.begin.y;
            double x4 = ln.end.x;
            double y4 = ln.end.y;
            double a = Pt::determinant(x1, y1, x2, y2);
            double b = Pt::determinant(x3, y3, x4, y4);
            double c = Pt::determinant(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
            double x = Pt::determinant(a, x1 - x2, b, x3 - x4) / c;
            double y = Pt::determinant(a, y1 - y2, b, y3 - y4) / c;

            return Pt(x, y, 0);
        }

        static Ln getNil() {
            return Ln(Pt::getNil(), Pt::getNil());
        }
};

/**
 * A two dimensional rectangular size parallel to the base plane.
 */
class LAYOUT_API Rs {
    public:
        double width;

        double height;

    public:
        Rs() {
            assign(NaN, NaN);
        }

        Rs(double width, double height) {
            assign(width, height);
        }

        Rs& assign(double width, double height) {
            this->width = width;
            this->height = height;

            return *this;
        }

        static Rs getNil() {
            return Rs(NaN, NaN);
        }

        bool isNil() const {
            return isNaN(width) && isNaN(height);
        }

        bool isFullySpecified() const {
            return !isNaN(width) && !isNaN(height);
        }

        double getDiagonalLength() const {
            return sqrt(width * width + height * height);
        }

        double getArea() const {
            return width * height;
        }
};

/**
 * A two dimensional rectangle positioned in three dimensions and parallel to the base plane.
 */
class LAYOUT_API Rc {
    public:
        Pt pt;

        Rs rs;

    public:
        Rc() {
            assign(Pt::getNil(), Rs::getNil());
        }

        Rc(double x, double y, double z, double width, double height) {
            assign(Pt(x, y, z), Rs(width, height));
        }

        Rc(const Pt& pt, const Rs& rs) {
            assign(pt, rs);
        }

        Rc& assign(const Pt& pt, const Rs& rs) {
            this->pt = pt;
            this->rs = rs;

            return *this;
        }

        bool basePlaneProjectionIntersects(Rc rc2, bool strictly = false) const {
            return
                rc2.basePlaneProjectionContains(getLeftTop(), strictly) ||
                rc2.basePlaneProjectionContains(getRightTop(), strictly) ||
                rc2.basePlaneProjectionContains(getLeftBottom(), strictly) ||
                rc2.basePlaneProjectionContains(getRightBottom(), strictly);
        }

        bool basePlaneProjectionContains(const Pt& p, bool strictly = false) const {
            if (strictly)
                return pt.x < p.x && p.x < pt.x + rs.width && pt.y < p.y && p.y < pt.y + rs.height;
            else
                return pt.x <= p.x && p.x <= pt.x + rs.width && pt.y <= p.y && p.y <= pt.y + rs.height;
        }

        double getLeft() const {
            return pt.x;
        }

        double getRight() const {
            return pt.x + rs.width;
        }

        double getTop() const {
            return pt.y;
        }

        double getBottom() const {
            return pt.y + rs.height;
        }

        Pt getLeftTop() const {
            return Pt(pt.x, pt.y, pt.z);
        }

        Pt getCenterTop() const {
            return Pt(pt.x + rs.width / 2, pt.y, pt.z);
        }

        Pt getRightTop() const {
            return Pt(pt.x + rs.width, pt.y, pt.z);
        }

        Pt getLeftCenter() const {
            return Pt(pt.x, pt.y + rs.height / 2, pt.z);
        }

        Pt getCenterCenter() const {
            return Pt(pt.x + rs.width / 2, pt.y + rs.height / 2, pt.z);
        }

        Pt getRightCenter() const {
            return Pt(pt.x + rs.width, pt.y + rs.height / 2, pt.z);
        }

        Pt getLeftBottom() const {
            return Pt(pt.x, pt.y + rs.height, pt.z);
        }

        Pt getCenterBottom() const {
            return Pt(pt.x + rs.width / 2, pt.y + rs.height, pt.z);
        }

        Pt getRightBottom() const {
            return Pt(pt.x + rs.width, pt.y + rs.height, pt.z);
        }

        Ln getBasePlaneProjectionDistance(const Rc &other, double &distance) const {
            double x1 = pt.x;
            double y1 = pt.y;
            double z = pt.z;
            double x2 = pt.x + rs.width;
            double y2 = pt.y + rs.height;
            double x3 = other.pt.x;
            double y3 = other.pt.y;
            double zOther = other.pt.z;
            double x4 = other.pt.x + other.rs.width;
            double y4 = other.pt.y + other.rs.height;

            int bx;
            if (x2 <= x3)
                bx = 0;
            else if (x4 <= x1)
                bx = 2;
            else
                bx = 1;

            int by;
            if (y2 <= y3)
                by = 0;
            else if (y4 <= y1)
                by = 2;
            else
                by = 1;

            int b = by * 3 + bx;
            switch (b) {
                case 0:
                    distance = Pt::getDistance(x2, y2, 0, x3, y3, 0);
                    return Ln(x2, y2, z, x3, y3, zOther);
                case 1:
                    distance = y3 - y2;
                    return Ln(NaN, y2, z, NaN, y3, zOther);
                case 2:
                    distance = Pt::getDistance(x1, y2, 0, x4, y3, 0);
                    return Ln(x1, y2, z, x4, y3, zOther);
                case 3:
                    distance = x3 - x2;
                    return Ln(x2, NaN, z, x3, NaN, zOther);
                case 4:
                    distance = 0;
                    return Ln::getNil();
                case 5:
                    distance = x1 - x4;
                    return Ln(x1, NaN, z, x4, NaN, zOther);
                case 6:
                    distance = Pt::getDistance(x2, y1, 0, x3, y4, 0);
                    return Ln(x2, y1, z, x3, y4, zOther);
                case 7:
                    distance = y1 - y4;
                    return Ln(NaN, y1, z, NaN, y4, zOther);
                case 8:
                    distance = Pt::getDistance(x1, y1, 0, x4, y4, 0);
                    return Ln(x1, y1, z, x4, y4, zOther);
                default:
                    distance = NaN;
                    return Ln::getNil();
            }
        }

        static Rc getRcFromCenterSize(const Pt& center, const Rs& size) {
            return Rc(center.x - size.width / 2, center.y - size.height / 2, size.width, size.height, center.z);
        }

        static Rc getNil() {
            return Rc(Pt::getNil(), Rs::getNil());
        }

        bool isNil() const {
            return pt.isNil() && rs.isNil();
        }
};

/**
 * A two dimensional circle positioned in three dimensions and parallel to the base plane.
 */
class LAYOUT_API Cc {
    public:
        Pt origin;

        /**
         * Radius is always interpreted parallel to the base plane.
         */
        double radius;

    public:
        Cc() {
            assign(Pt::getNil(), NaN);
        }

        Cc(double x, double y, double z, double radius) {
            assign(Pt(x, y, z), radius);
        }

        Cc(const Pt& origin, double radius) {
            assign(origin, radius);
        }

        Cc& assign(const Pt& origin, double radius) {
            this->origin = origin;
            this->radius = radius;

            return *this;
        }

        Cc getBasePlaneProjection() const {
            return Cc(Pt(origin.x, origin.y, 0), radius);
        }

        bool basePlaneProjectionContains(Pt& pt, bool strictly = false) const {
            double distance = origin.getBasePlaneProjectionDistance(pt);
            return distance < radius || (!strictly && distance == radius);
        }

        std::vector<Pt> basePlaneProjectionIntersect(const Cc& other) const {
            double R2 = radius * radius;
            double r2 = other.radius * other.radius;
            double d = origin.getBasePlaneProjectionDistance(other.origin);
            double d2 = d * d;
            double a = (d2 - r2 + R2);

            if (d2 == 0)
                return std::vector<Pt>();

            double y2 = (4 * d2 * R2 - a * a) / (4 * d2);

            if (y2 < 0)
                return std::vector<Pt>();

            double y = sqrt(y2);
            double x = (d2 - r2 + R2) / (2 * d);
            Pt pt1 = Pt(x, y, 0);
            Pt pt2 = Pt(x, -y, 0);
            double angle = other.origin.copy().subtract(origin).getBasePlaneProjectionAngle();
            pt1.basePlaneRotate(angle);
            pt2.basePlaneRotate(angle);
            pt1.add(origin);
            pt2.add(origin);

            std::vector<Pt> pts;
            pts.push_back(pt1);
            pts.push_back(pt2);

            return pts;
        }

        static Cc getBasePlaneProjectionEnclosingCircle(const std::vector<Cc>& circles) {
            Cc cc = circles[0];

            for (std::vector<Cc>::const_iterator it = circles.begin(); it != circles.end(); ++it)
                cc = cc.getBasePlaneProjectionEnclosingCircle(*it);

            return cc;
        }

        Cc getBasePlaneProjectionEnclosingCircle(const Cc& other) const {
            double distance = origin.getDistance(other.origin);
            double d = distance + std::max(radius, other.radius - distance) + std::max(other.radius, radius - distance);
            Pt pt(d / 2 - std::max(radius, other.radius - distance), 0, 0);
            double angle = other.origin.copy().subtract(origin).getBasePlaneProjectionAngle();
            pt.basePlaneRotate(angle);
            pt.add(origin);

            return Cc(pt, d / 2);
        }

        Pt getCenterTop() const {
            return Pt(origin.x, origin.y - radius, origin.z);
        }

        Pt getCenterBottom() const {
            return Pt(origin.x, origin.y + radius, origin.z);
        }

        Pt getLeftCenter() const {
            return Pt(origin.x - radius, origin.y, origin.z);
        }

        Pt getRightCenter() const {
            return Pt(origin.x + radius, origin.y, origin.z);
        }
};

}  // namespace layout
}  // namespace omnetpp


#endif
