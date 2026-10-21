package org.omnetpp.figures.misc;

import java.util.List;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionDimension;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.eclipse.draw2d.geometry.PrecisionRectangle;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.Converter;

/**
 * Mainly to replace org.eclipse.swt.graphics.Transform. Most code is taken from ccanvas.c, inversion is from cairo.
 * http://www.opensource.apple.com/source/X11libs/X11libs-60/cairo/cairo-1.10.2/src/cairo-matrix.c
 */
public class Transform implements Cloneable {
    double a, c, t1;
    double b, d, t2;

    public Transform() {
        setElements(1, 0, 0, 1, 0, 0);
    }

    public Transform(List<Double> elem) {
        setElements(elem);
    }

    public Transform(double[] elem) {
        setElements(elem);
    }

    public Transform(double na, double nb, double nc, double nd, double nt1, double nt2) {
        setElements(na, nb, nc, nd, nt1, nt2);
    }

    public Transform setElements(List<Double> elem) {
        a = elem.get(0); b = elem.get(1); c = elem.get(2); d = elem.get(3); t1 = elem.get(4); t2 = elem.get(5);
        return this;
    }

    public Transform setElements(double[] elem) {
        a = elem[0]; b = elem[1]; c = elem[2]; d = elem[3]; t1 = elem[4]; t2 = elem[5];
        return this;
    }

    public Transform setElements(double na, double nb, double nc, double nd, double nt1, double nt2) {
        a = na; b = nb; c = nc; d = nd; t1 = nt1; t2 = nt2;
        return this;
    }

    public double[] getElements() {
        return new double[] { a, b, c, d, t1, t2 };
    }

    @Override
    public Transform clone() {
        return new Transform(a, b, c, d, t1, t2);
    }

    @Override
    public String toString() {
        return String.format("((%s %s) (%s %s) (%s %s))",
                Converter.doubleToString(a), Converter.doubleToString(b),
                Converter.doubleToString(c), Converter.doubleToString(d),
                Converter.doubleToString(t1), Converter.doubleToString(t2));
    };

    private boolean isIdentity() {
        return isIdentity(1e-4);
    }

    private boolean isIdentity(double eps) {
        return (Math.abs(a - 1.0) < eps) && (Math.abs(b)       < eps)
            && (Math.abs(c)       < eps) && (Math.abs(d - 1.0) < eps)
            && (Math.abs(t1)      < eps) && (Math.abs(t2)      < eps);
    }

    protected double determinant() {
        return a * d - b * c;
    }

    protected Transform scalarMultiply(double s) {
        setElements(a * s, b * s, c * s, d * s, t1 * s, t2 * s);
        return this;
    }

    /*
     * This function isn't a correct adjoint in that the implicit 1 in the
     * homogeneous result should actually be ad-bc instead. But, since this
     * adjoint is only used in the computation of the inverse, which divides by
     * det (A)=ad-bc anyway, everything works out in the end.
     */
    protected Transform getAdjoint() {
        return new Transform(d, -b, -c, a, c * t2 - d * t1, b * t1 - a * t2);
    }

    /**
     * Returns true if the transform can be inverted, i.e. it does not collapse the plane
     * into a line or a point (e.g. with a zero scale factor), and all elements are finite.
     */
    public boolean isInvertible() {
        double det = determinant();
        return det != 0 && Double.isFinite(det) && Double.isFinite(t1) && Double.isFinite(t2);
    }

    public Transform getInverse() {
        Transform inverse;

        try {
            // Simple scaling|translation matrices are quite common...
            if (b == 0 && c == 0) {
                inverse = new Transform();

                inverse.t1 = -t1;
                inverse.t2 = -t2;

                inverse.a = 1.0 / a;
                inverse.t1 *= inverse.a;
                inverse.d = 1.0 / d;
                inverse.t2 *= inverse.d;
            } else {
                // inv (A) = 1/det (A) * adj (A)
                inverse = getAdjoint().scalarMultiply(1.0 / determinant());
            }
        } catch (ArithmeticException e) {
            throw new RuntimeException("Can't invert singular matrix", e);
        }

        if (Debug.isDebugging()) {
            Transform iden1 = inverse.clone().multiply(this);
            Transform iden2 = clone().multiply(inverse);

            if (!iden1.isIdentity() || !iden2.isIdentity()) {
                System.out.println("This should be identity: " + iden1);
                System.out.println("This should be identity: " + iden2);
                throw new RuntimeException();
            }
        }

        return inverse;
    }

    public Transform translate(double dx, double dy) {
        t1 += dx;
        t2 += dy;
        return this;
    }

    public Transform scale(double sx, double sy) {
        setElements(a * sx, b * sy, c * sx, d * sy, t1 * sx, t2 * sy);
        return this;
    }

    public Transform scale(double sx, double sy, double cx, double cy) {
        setElements(a * sx, b * sy, c * sx, d * sy, sx * t1 - cx * sx + cx, sy * t2 - cy * sy + cy);
        return this;
    }

    public Transform rotate(double phi) {
        double cosPhi = Math.cos(phi);
        double sinPhi = Math.sin(phi);
        setElements(a  * cosPhi - b  * sinPhi, a  * sinPhi + b  * cosPhi,
                    c  * cosPhi - d  * sinPhi, c  * sinPhi + d  * cosPhi,
                    t1 * cosPhi - t2 * sinPhi, t1 * sinPhi + t2 * cosPhi);
        return this;
    }

    public Transform rotate(double phi, double cx, double cy) {
        double cosPhi = Math.cos(phi);
        double sinPhi = Math.sin(phi);
        setElements(a  * cosPhi - b  * sinPhi,  a  * sinPhi + b  * cosPhi,
                    c  * cosPhi - d  * sinPhi,  c  * sinPhi + d  * cosPhi,
                   -t2 * sinPhi + cy * sinPhi + t1 * cosPhi - cx * cosPhi + cx,
                    t1 * sinPhi - cx * sinPhi + t2 * cosPhi - cy * cosPhi + cy);
        return this;
    }

    public Transform skewx(double coeff) {
        double a_ = b * coeff + a;
        double c_ = d * coeff + c;
        double t1_ = t2 * coeff + t1;
        a = a_;
        c = c_;
        t1 = t1_;
        return this;
    }

    public Transform skewy(double coeff) {
        double b_ = a * coeff + b;
        double d_ = c * coeff + d;
        double t2_ = t1 * coeff + t2;
        b = b_;
        d = d_;
        t2 = t2_;
        return this;
    }

    public Transform skewx(double coeff, double cy) {
        double a_ = b * coeff + a;
        double c_ = d * coeff + c;
        double t1_ = t2 * coeff - cy * coeff + t1;
        a = a_;
        c = c_;
        t1 = t1_;
        return this;
    }

    public Transform skewy(double coeff, double cx) {
        double b_ = a * coeff + b;
        double d_ = c * coeff + d;
        double t2_ = t1 * coeff - cx * coeff + t2;
        b = b_;
        d = d_;
        t2 = t2_;
        return this;
    }

    public Transform leftMultiply(Transform other) {
        setElements(
                a * other.a + b * other.c,
                a * other.b + b * other.d,
                c * other.a + d * other.c,
                c * other.b + d * other.d,
                t1 * other.a + t2 * other.c + other.t1,
                t1 * other.b + t2 * other.d + other.t2);

        return this;
    }

    public Transform multiply(Transform other) {
        setElements(
                other.a * a + other.b * c,
                other.a * b + other.b * d,
                other.c * a + other.d * c,
                other.c * b + other.d * d,
                other.t1 * a + other.t2 * c + t1,
                other.t1 * b + other.t2 * d + t2);

        return this;
    }

    public PrecisionPoint applyTo(Point p) {
        return new PrecisionPoint(
                a * p.preciseX() + c * p.preciseY() + t1,
                b * p.preciseX() + d * p.preciseY() + t2);
    }

    public PrecisionRectangle applyTo(Rectangle rect) {
        Point tbl = applyTo(rect.getBottomLeft());
        Point tbr = applyTo(rect.getBottomRight());
        Point ttl = applyTo(rect.getTopLeft());
        Point ttr = applyTo(rect.getTopRight());

        PrecisionRectangle transformed = new PrecisionRectangle(tbl, new PrecisionDimension(0, 0));
        transformed.union(tbr);
        transformed.union(ttl);
        transformed.union(ttr);

        return transformed;
    }
}
