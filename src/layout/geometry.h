//=========================================================================
//  GEOMETRY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __GEOMETRY_H_
#define __GEOMETRY_H_

#include <math.h>
#include <vector>
#include "exception.h"

#ifndef NDEBUG
#define ASSERT(expr)  \
  ((void) ((expr) ? 0 : \
           (throw opp_runtime_error("ASSERT: condition %s false, %s line %d", \
                             #expr, __FILE__, __LINE__), 0)))
#else
#define ASSERT(expr)  ((void)0)
#endif

extern double NaN;
extern double POSITIVE_INFINITY;

inline bool isNaN(double x) { return x != x; }

class Pt
{
    public:
        double x;

        double y;

    public:
        Pt() {
            assign(NaN, NaN);
        }

        Pt(double x, double y) {
            assign(x, y);
        }

        Pt(const Pt& pt) {
            assign(pt);
        }

        Pt& assign(const Pt& pt) {
            return assign(pt.x, pt.y);
        }

        Pt& assign(double x, double y) {
            this->x = x;
            this->y = y;

            return *this;
        }

        Pt copy() {
            return Pt(*this);
        }

        void setZero() {
            assign(0, 0);
        }

        double getLength() {
            return sqrt(x * x + y * y);
        }

        double getDistance(const Pt& other) const {
            double dx = x - other.x;
            double dy = y - other.y;

            return sqrt(dx * dx + dy * dy);
        }

        double getAngle() {
            return atan2(y, x);
        }

        void rotate(double rotateAngle) {
            double angle = getAngle();

            if (!isNaN(angle)) {
                double length = getLength();
                angle += rotateAngle;
                x = cos(angle) * length;
                y = sin(angle) * length;
            }
        }

        Pt& normalize() {
            double length = getLength();
            x /= length;
            y /= length;

            return *this;
        }

        Pt& add(const Pt& pt) {
            x += pt.x;
            y += pt.y;

            return *this;
        }

        Pt& add(double x, double y) {
            this->x += x;
            this->y += y;

            return *this;
        }

        Pt& subtract(const Pt& pt) {
            x -= pt.x;
            y -= pt.y;

            return *this;
        }

        Pt& multiply(double d) {
            x *= d;
            y *= d;

            return *this;
        }

        Pt& reverse() {
            return multiply(-1);
        }

        Pt& divide(double d) {
            x /= d;
            y /= d;

            return *this;
        }

        double crossProduct(Pt& pt) {
            return x * pt.y - y * pt.x;
        }

        Pt& transpose() {
            return assign(y, -x);
        }

        void setNaNToZero() {
            if (isNaN(x))
                x = 0;

            if (isNaN(y))
                y = 0;
        }

        bool isZero() {
            return x == 0 && y == 0;
        }

        static Pt getRadial(double radius, double angle) {
            return Pt(cos(angle) * radius, sin(angle) * radius);
        }

        static Pt getNil() {
            return Pt(NaN, NaN);
        }

        static Pt getZero() {
            return Pt(0, 0);
        }

        bool isNil() {
            return isNaN(x) && isNaN(y);
        }

        bool isFullySpecified() {
            return !isNaN(x) && !isNaN(y);
        }
};

class Ln {
    public:
        Pt begin;

        Pt end;

    public:
        Ln() {
            assign(Pt::getNil(), Pt::getNil());
        }

        Ln(double x1, double y1, double x2, double y2) {
            assign(Pt(x1, y1), Pt(x2, y2));
        }

        Ln(Pt& begin, Pt& end) {
            assign(begin, end);
        }

        Ln& assign(Pt& begin, Pt& end) {
            this->begin = begin;
            this->end = end;

            return *this;
        }

        Pt getClosestPoint(Pt& pt) {
            Pt n = getDirectionVector().transpose();

            return intersect(Ln(pt, n.add(pt)));
        }

        Pt getDirectionVector() {
            Pt v(end);
            v.subtract(begin);
            v.normalize();

            return v;
        }

        Pt intersect(Ln& ln) {
            double x1 = begin.x;
            double y1 = begin.y;
            double x2 = end.x;
            double y2 = end.y;
            double x3 = ln.begin.x;
            double y3 = ln.begin.y;
            double x4 = ln.end.x;
            double y4 = ln.end.y;
            double a = determinant(x1, y1, x2, y2);
            double b = determinant(x3, y3, x4, y4);
            double c = determinant(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
            double x = determinant(a, x1 - x2, b, x3 - x4) / c;
            double y = determinant(a, y1 - y2, b, y3 - y4) / c;

            return Pt(x, y);
        }

        double determinant(double a, double b, double c, double d) {
            return a * d - b * c;
        }
};

class Rs {
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

        bool isNil() {
            return isNaN(width) && isNaN(height);
        }

        double getDiagonalLength()
        {
            return sqrt(width * width + height * height);
        }

        double getArea()
        {
            return width * height;
        }
};

class Rc {
    public:
        Pt pt;

        Rs rs;

    public:
        Rc() {
            assign(Pt::getNil(), Rs::getNil());
        }

        Rc(double x, double y, double width, double height) {
            assign(Pt(x, y), Rs(width, height));
        }

        Rc(Pt& pt, Rs& rs) {
            assign(pt, rs);
        }

        Rc& assign(Pt& pt, Rs& rs) {
            this->pt = pt;
            this->rs = rs;

            return *this;
        }

        bool intersects(Rc rc2) {
            return
                rc2.contains(getTopLeft()) ||
                rc2.contains(getTopRight()) ||
                rc2.contains(getBottomLeft()) ||
                rc2.contains(getBottomRight());
        }

        bool contains(Pt& p) {
            return pt.x <= p.x && p.x <= pt.x + rs.width && pt.y <= p.y && p.y <= pt.y + rs.height;
        }

        double getLeft() {
            return pt.x;
        }

        double getRight() {
            return pt.x + rs.width;
        }

        double getTop() {
            return pt.y;
        }

        double getBottom() {
            return pt.y + rs.height;
        }
        Pt getTopLeft() {
            return Pt(pt.x, pt.y);
        }

        Pt getTopRight() {
            return Pt(pt.x + rs.width, pt.y);
        }

        Pt getBottomLeft() {
            return Pt(pt.x, pt.y + rs.height);
        }

        Pt getBottomRight() {
            return Pt(pt.x + rs.width, pt.y + rs.height);
        }

        static Rc getNil() {
            return Rc(Pt::getNil(), Rs::getNil());
        }

        bool isNil() {
            return pt.isNil() && rs.isNil();
        }
};

class Cc {
    public:
        Pt origin;

        double radius;

    public:
        Cc() {
            assign(Pt::getNil(), NaN);
        }

        Cc(double x, double y, double radius) {
            assign(Pt(x, y), radius);
        }

        Cc(Pt& origin, double radius) {
            assign(origin, radius);
        }

        Cc& assign(Pt& origin, double radius) {
            this->origin = origin;
            this->radius = radius;

            return *this;
        }

        bool contains(Pt& pt) {
            return origin.getDistance(pt) <= radius;
        }

        std::vector<Pt> intersect(Cc& other) {
            double R2 = radius * radius;
            double r2 = other.radius * other.radius;
            double d = origin.getDistance(other.origin);
            double d2 = d * d;
            double a = (d2 - r2 + R2);

            if (d2 == 0)
                return std::vector<Pt>();

            double y2 = (4 * d2 * R2 - a * a) / (4 * d2);

            if (y2 < 0)
                return std::vector<Pt>();

            double y = sqrt(y2);
            double x = (d2 - r2 + R2) / (2 * d);
            Pt pt1 = Pt(x, y);
            Pt pt2 = Pt(x, -y);
            double angle = other.origin.copy().subtract(origin).getAngle();
            pt1.rotate(angle);
            pt2.rotate(angle);
            pt1.add(origin);
            pt2.add(origin);

            std::vector<Pt> pts;
            pts.push_back(pt1);
            pts.push_back(pt2);

            return pts;
        }

        static Cc getEnclosingCircle(std::vector<Cc>& circles) {
            Cc cc = circles[0];

            for (std::vector<Cc>::iterator it = circles.begin(); it != circles.end(); it++)
                cc = cc.getEnclosingCircle(*it);

            return cc;
        }

        Cc getEnclosingCircle(Cc& other) {
            double distance = origin.getDistance(other.origin);
            double d = distance + std::max(radius, other.radius - distance) + std::max(other.radius, radius - distance);
            Pt pt(d / 2 - std::max(radius, other.radius - distance), 0);
            double angle = other.origin.copy().subtract(origin).getAngle();
            pt.rotate(angle);
            pt.add(origin);

            return Cc(pt, d / 2);
        }

        Pt getCenterTop() {
            return Pt(origin.x, origin.y - radius);
        }

        Pt getCenterBottom() {
            return Pt(origin.x, origin.y + radius);
        }

        Pt getLeftCenter() {
            return Pt(origin.x - radius, origin.y);
        }

        Pt getRightCenter() {
            return Pt(origin.x + radius, origin.y);
        }
};

#endif
