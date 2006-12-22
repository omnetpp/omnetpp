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

        Pt copy() const {
            return Pt(*this);
        }

        void setZero() {
            assign(0, 0);
        }

        double getLength() const {
            return sqrt(x * x + y * y);
        }

        double getDistance(const Pt& other) const {
            return getDistance(x, y, other.x, other.y);
        }

        static double getDistance(double x1, double y1, double x2, double y2) {
            double dx = x1 - x2;
            double dy = y1 - y2;

            return sqrt(dx * dx + dy * dy);
        }

        double getAngle() const {
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

        double crossProduct(const Pt& pt) const {
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

        bool isZero() const {
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

        bool isNil() const {
            return isNaN(x) && isNaN(y);
        }

        bool isFullySpecified() const {
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

        Ln(const Pt& begin, const Pt& end) {
            assign(begin, end);
        }

        Ln& assign(const Pt& begin, const Pt& end) {
            this->begin = begin;
            this->end = end;

            return *this;
        }

        Pt getClosestPoint(const Pt& pt) const {
            Pt n = getDirectionVector().transpose();

            return intersect(Ln(pt, n.add(pt)));
        }

        Pt getDirectionVector() const {
            Pt v(end);
            v.subtract(begin);

            return v;
        }

        Pt intersect(const Ln& ln) const {
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

        double determinant(double a, double b, double c, double d) const {
            return a * d - b * c;
        }

        static Ln getNil() {
            return Ln(Pt::getNil(), Pt::getNil());
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

        bool isNil() const {
            return isNaN(width) && isNaN(height);
        }

        double getDiagonalLength() const {
            return sqrt(width * width + height * height);
        }

        double getArea() const {
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

        Rc(const Pt& pt, const Rs& rs) {
            assign(pt, rs);
        }

        Rc& assign(const Pt& pt, const Rs& rs) {
            this->pt = pt;
            this->rs = rs;

            return *this;
        }

        bool intersects(Rc rc2) const {
            return
                rc2.contains(getTopLeft()) ||
                rc2.contains(getTopRight()) ||
                rc2.contains(getBottomLeft()) ||
                rc2.contains(getBottomRight());
        }

        bool contains(Pt& p) const {
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

        Pt getTopLeft() const {
            return Pt(pt.x, pt.y);
        }

        Pt getTopRight() const {
            return Pt(pt.x + rs.width, pt.y);
        }

        Pt getBottomLeft() const {
            return Pt(pt.x, pt.y + rs.height);
        }

        Pt getBottomRight() const {
            return Pt(pt.x + rs.width, pt.y + rs.height);
        }

        Ln getDistance(const Rc &other, double &distance) const {
            double x1 = pt.x;
            double y1 = pt.y;
            double x2 = pt.x + rs.width;
            double y2 = pt.y + rs.height;
            double x3 = other.pt.x;
            double y3 = other.pt.y;
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
                    distance = Pt::getDistance(x2, y2, x3, y3);
                    return Ln(x2, y2, x3, y3);
                case 1:
                    distance = y3 - y2;
                    return Ln(NaN, y2, NaN, y3);
                case 2:
                    distance = Pt::getDistance(x1, y2, x4, y3);
                    return Ln(x1, y2, x4, y3);
                case 3:
                    distance = x3 - x2;
                    return Ln(x2, NaN, x3, NaN);
                case 4:
                    distance = 0;
                    return Ln::getNil();
                case 5:
                    distance = x1 - x4;
                    return Ln(x1, NaN, x4, NaN);
                case 6:
                    distance = Pt::getDistance(x2, y1, x3, y4);
                    return Ln(x2, y1, x3, y4);
                case 7:
                    distance = y1 - y4;
                    return Ln(NaN, y1, NaN, y4);
                case 8:
                    distance = Pt::getDistance(x1, y1, x4, y4);
                    return Ln(x1, y1, x4, y4);
                default:
                    distance = NaN;
                    return Ln::getNil();
            }
        }

        static Rc getRcFromCenterSize(const Pt& center, const Rs& size) {
            return Rc(center.x - size.width / 2, center.y - size.height / 2, size.width, size.height);
        }

        static Rc getNil() {
            return Rc(Pt::getNil(), Rs::getNil());
        }

        bool isNil() const {
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

        Cc(const Pt& origin, double radius) {
            assign(origin, radius);
        }

        Cc& assign(const Pt& origin, double radius) {
            this->origin = origin;
            this->radius = radius;

            return *this;
        }

        bool contains(Pt& pt) const {
            return origin.getDistance(pt) <= radius;
        }

        std::vector<Pt> intersect(const Cc& other) const {
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

        static Cc getEnclosingCircle(const std::vector<Cc>& circles) {
            Cc cc = circles[0];

            for (std::vector<Cc>::const_iterator it = circles.begin(); it != circles.end(); it++)
                cc = cc.getEnclosingCircle(*it);

            return cc;
        }

        Cc getEnclosingCircle(const Cc& other) const {
            double distance = origin.getDistance(other.origin);
            double d = distance + std::max(radius, other.radius - distance) + std::max(other.radius, radius - distance);
            Pt pt(d / 2 - std::max(radius, other.radius - distance), 0);
            double angle = other.origin.copy().subtract(origin).getAngle();
            pt.rotate(angle);
            pt.add(origin);

            return Cc(pt, d / 2);
        }

        Pt getCenterTop() const {
            return Pt(origin.x, origin.y - radius);
        }

        Pt getCenterBottom() const {
            return Pt(origin.x, origin.y + radius);
        }

        Pt getLeftCenter() const {
            return Pt(origin.x - radius, origin.y);
        }

        Pt getRightCenter() const {
            return Pt(origin.x + radius, origin.y);
        }
};

#endif
