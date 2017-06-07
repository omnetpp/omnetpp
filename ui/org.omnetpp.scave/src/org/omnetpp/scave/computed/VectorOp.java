package org.omnetpp.scave.computed;

import java.util.function.DoublePredicate;
import java.util.function.DoubleUnaryOperator;

/**
 * Vector operations.
 *
 * @author andras
 */
public class VectorOp {
    public static void applyUnary(XYVector array, DoubleUnaryOperator f) {
        int n = array.size();
        for (int i = 0; i < n; i++)
            array.y[i] = f.applyAsDouble(array.y[i]);
    }

    public static void filter(XYVector array, DoublePredicate pred) {
        int n = array.size();
        int dest = 0;
        for (int src = 0; src < n; src++) {
            boolean match = pred.test(array.y[src]);
            if (match) {
                if (src != dest)
                    array.copy(src, dest);
                dest++;
            }
        }
        array.setSize(dest);
    }

    public static void add(XYVector array, double d) {
        int n = array.size();
        for (int i = 0; i < n; i++)
            array.y[i] += d;
    }

    public static void subtract(XYVector array, double d) {
        add(array, -d);
    }

    public static void mul(XYVector array, double d) {
        int n = array.size();
        for (int i = 0; i < n; i++)
            array.y[i] *= d;
    }

    public static void div(XYVector array, double d) {
        mul(array, 1/d);
    }

    public static void neg(XYVector array) {
        int n = array.size();
        for (int i = 0; i < n; i++)
            array.y[i] = -array.y[i];
    }

    public static void mod(XYVector array, double d) {
        int n = array.size();
        for (int i = 0; i < n; i++)
            array.y[i] %= d;
    }

    public static void abs(XYVector array) {
        int n = array.size();
        for (int i = 0; i < n; i++)
            array.y[i] = Math.abs(array.y[i]);
    }

    public static void sum(XYVector array) {
        double sum = 0;
        int n = array.size();
        for (int i = 0; i < n; i++) {
            sum += array.y[i];
            array.y[i] = sum;
        }
    }

    public static void mean(XYVector array) {
        double sum = 0;
        int n = array.size();
        for (int i = 0; i < n; i++) {
            sum += array.y[i];
            array.y[i] = sum / (i+1);
        }
    }

    public static void min(XYVector array) {
        int n = array.size();
        if (n == 0)
            return;
        double min = array.y[0];
        for (int i = 1; i < n; i++) {
            if (array.y[i] < min)
                min = array.y[i];
            else
                array.y[i] = min;
        }
    }

    public static void max(XYVector array) {
        int n = array.size();
        if (n == 0)
            return;
        double max = array.y[0];
        for (int i = 1; i < n; i++) {
            if (array.y[i] > max)
                max = array.y[i];
            else
                array.y[i] = max;
        }
    }

    public static void discardNan(XYVector array) {
        filter(array, (d) -> !Double.isNaN(d));
    }

    public static void discardInf(XYVector array) {
        filter(array, (d) -> !Double.isInfinite(d));
    }

    public static void discardNanAndInf(XYVector array) {
        filter(array, (d) -> Double.isFinite(d));
    }

    public static void difference(XYVector array) {
        int n = array.size();
        if (n == 0)
            return;
        double prevY = array.y[0];
        for (int i = 1; i < n; i++) {
            double tmp = array.y[i];
            array.y[i] -= prevY;
            prevY = tmp;
        }
        //TODO discard y[0]?
    }

    public static void timeDiff(XYVector array) {
        int n = array.size();
        if (n == 0)
            return;
        double prevX = array.x[0];
        for (int i = 1; i < n; i++) {
            array.y[i] = array.x[i] - prevX;
            prevX = array.x[i];
        }
        //TODO discard x[0]?
    }

    public static void movingAverage(XYVector array, double alpha) {
        int n = array.size();
        if (n == 0)
            return;
        double prevY = array.y[0];
        for (int i = 0; i < n; i++)
            array.y[i] = prevY = prevY + alpha*(array.y[i] - prevY);
    }

    public static void timeShift(XYVector array, double dt) {
        int n = array.size();
        for (int i = 0; i < n; i++)
            array.x[i] += dt;
    }

    public static void linearTrend(XYVector array, double a, double b) {
        int n = array.size();
        for (int i = 0; i < n; i++)
            array.y[i] += a * array.x[i] + b;
    }

    //TODO removeLinearTrend

    public static void crop(XYVector array, double startTime, double endTime) {
//TODO
//            if (d.x >= from && d.x <= to)
//                out()->write(&d, 1);
    }

    public static void clamp(XYVector array, double min, double max) {
        int n = array.size();
        for (int i = 0; i < n; i++)
            array.y[i] = Math.min(max, Math.max(min, array.y[i]));
    }

    public static void discardOutliers(XYVector array, double min, double max) {
        filter(array, (y) -> (y >= min && y <= max));
    }

    public static void removeRepeats(XYVector array) {
        int n = array.size();
        if (n == 0)
            return;
        int dest = 0;
        double prevY = array.y[0];
        for (int src = 0; src < n; src++) {
            if (array.y[src] != prevY) {
                prevY = array.y[src];
                if (src != dest)
                    array.copy(src, dest);
                dest++;
            }
        }
        array.setSize(dest);
    }

    //note: CompareNode is skipped

    public enum InterpolationMode { SAMPLE_HOLD, BACKWARD_SAMPLE_HOLD, LINEAR };

    public static void integrate(XYVector array, InterpolationMode interpolationMode) {
        int n = array.size();
        if (n == 0)
            return;
        double integral = 0;
        double prevx = array.x[0];
        double prevy = array.y[0];
        array.y[0] = 0;
        for (int i = 1; i < n; i++) {
            double dx = array.x[i] - prevx;
            switch (interpolationMode) {
            case SAMPLE_HOLD:
                integral += prevy * dx;
                break;

            case BACKWARD_SAMPLE_HOLD:
                integral += array.y[i] * dx;
                break;

            case LINEAR:
                integral += (prevy + array.y[i]) / 2 * dx;
                break;
            }
            prevx = array.x[i];
            prevy = array.y[i];
            array.y[i] = integral;
        }
    }

    public static void timeAverage(XYVector array, InterpolationMode interpolationMode) {
        int n = array.size();
        if (n == 0)
            return;
        double integral = 0;
        double x0 = array.x[0];
        double prevx = array.x[0];
        double prevy = array.y[0];
        for (int i = 0; i < n; i++) {
            double dx = array.x[i] - prevx;
            switch (interpolationMode) {
            case SAMPLE_HOLD:
                integral += prevy * dx;
                break;

            case BACKWARD_SAMPLE_HOLD:
                integral += array.y[i] * dx;
                break;

            case LINEAR:
                integral += (prevy + array.y[i]) / 2 * dx;
                break;
            }
            prevx = array.x[i];
            prevy = array.y[i];

            // Note: the following line results in the first few values (where x==x0)
            // being replaced by NaN; it is debatable whether this is a good thing.
            // Other options would be: 1. leaving them intact (questionable); or
            // 2. filtering them out.
            array.y[i] = integral / (array.x[i] - x0);
        }
    }

    public static void divideByTime(XYVector array) {
        int n = array.size();
        for (int i = 0; i < n; i++)
            array.y[i] /= array.x[i];
    }

    public static void timeToSerial(XYVector array) {
        int n = array.size();
        for (int i = 0; i < n; i++)
            array.x[i] = i;
    }

    public static void subtractFirstValue(XYVector array) {
        int n = array.size();
        int i;
        for (i = 0; i < n; i++)
            if (Double.isFinite(array.y[i]))
                break;
        if (i == n)
            return;
        double firstValue = array.y[i];
        for ( ; i < n; i++)
            array.y[i] -= firstValue;
    }

}
