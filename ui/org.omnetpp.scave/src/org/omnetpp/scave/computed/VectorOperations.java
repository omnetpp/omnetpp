package org.omnetpp.scave.computed;

import java.util.HashMap;
import java.util.Map;

/**
 * Vector operations.
 *
 * @author andras
 */
//TODO revise names (they are too long at times, and changed names from previous version
public class VectorOperations {
    public static final VectorOperation ADD = new VectorOperation(
            "add", "add(c)",
            VectorOp.class, "add", 
            "Adds a constant to the input: yout[k] = y[k] + c");
    public static final VectorOperation SUBTRACT = new VectorOperation(
            "subtract", "subtract(c)", 
            VectorOp.class, "subtract", 
            "Subtracts a constant from the input: yout[k] = y[k] - c");
    public static final VectorOperation MUL = new VectorOperation(
            "mul", "mul(c)", 
            VectorOp.class, "mul", 
            "Multiplies the input by a constant: yout[k] = y[k] * c");
    public static final VectorOperation DIV = new VectorOperation(
            "div", "div(c)", 
            VectorOp.class, "div", 
            "Divides the input by a constant: yout[k] = y[k] / c");
    public static final VectorOperation NEG = new VectorOperation(
            "neg", "neg()", 
            VectorOp.class, "neg", 
            "Multiplies the input by -1: yout[k] = -yout[k]");
    public static final VectorOperation MOD = new VectorOperation(
            "mod", "mod(c)", 
            VectorOp.class, "mod", 
            "Computes the input modulo a constant: yout[k] = y[k] % c");
    public static final VectorOperation ABS = new VectorOperation(
            "abs", "abs()", 
            VectorOp.class, "abs", 
            "Computes the absolute value of the input: yout[k] = abs(y[k])");
    public static final VectorOperation SUM = new VectorOperation(
            "sum", "sum()", 
            VectorOp.class, "sum", 
            "Computes the running total of the input, i.e. the sum of the values y[0]..y[k]: yout[k] = y[0] + y[1] + ... + y[k]");
    public static final VectorOperation MEAN = new VectorOperation(
            "mean", "mean()", 
            VectorOp.class, "mean", 
            "Computes the cumulative running average of the input, i.e. the mean of values y[0]..y[k]: yout[k] = (y[0] + y[1] + ... + y[k]) / (k+1)");
    public static final VectorOperation MIN = new VectorOperation(
            "min", "min()", 
            VectorOp.class, "min", 
            "Computes the minimum of the values y[0]..y[k]: yout[k] = min(y[0], y[1], ..., y[k])");
    public static final VectorOperation MAX = new VectorOperation(
            "max", "max()", 
            VectorOp.class, "max", 
            "Computes the maximum of the values y[0]..y[k]: yout[k] = max(y[0], y[1], ..., y[k])");
    public static final VectorOperation DISCARD_NAN = new VectorOperation(
            "discardNan", "discardNan()", 
            VectorOp.class, "discardNan", 
            "Removes NaN values from the input: yout[k] = yout[k] if !isnan(yout[k]) else delete");
    public static final VectorOperation DISCARD_INF = new VectorOperation(
            "discardInf", "discardInf()", 
            VectorOp.class, "discardInf", 
            "Removes positive and negative infinite values from the input: yout[k] = yout[k] if !isinf(yout[k]) else delete");
    public static final VectorOperation DISCARD_NAN_INF = new VectorOperation(
            "discardNanAndInf", "discardNanAndInf()", 
            VectorOp.class, "discardNanAndInf", 
            "Removes NaN and positive and negative infinite values from the input: yout[k] = yout[k] if isfinite(yout[k]) else delete");
    public static final VectorOperation DIFFERENCE = new VectorOperation(
            "difference", "difference()", 
            VectorOp.class, "difference", 
            "Subtracts the previous value from every value: yout[k] = y[k] - y[k-1]");
    public static final VectorOperation TIMEDIFF = new VectorOperation(
            "timeDiff", "timeDiff()", 
            VectorOp.class, "timeDiff", 
            "Computes the time difference between the current and the previous value: yout[k] = t[k] - t[k-1]"); 
    public static final VectorOperation MOVING_AVERAGE = new VectorOperation(
            "movingAverage", "movingAverage(alpha)", 
            VectorOp.class, "movingAverage", 
            "Applies the exponentially weighted moving average filter: " +
            "yout[k] = yout[k-1] + alpha*(y[k]-yout[k-1]). " +
            "alpha is the smoothing constant.");
    public static final VectorOperation TIMESHIFT = new VectorOperation(
            "timeShift", "timeShift(dt)", 
            VectorOp.class, "timeShift", 
            "Shifts the input series in time by a constant: tout[k] = t[k] + dt");
    public static final VectorOperation LINEARTREND = new VectorOperation(
            "linearTrend", "linearTrend(a, b)", 
            VectorOp.class, "linearTrend", 
            "Adds linear component to input series: yout[k] = y[k] + a * t[k] + b");
    public static final VectorOperation CROP = new VectorOperation(
            "crop", "crop(startTime, endTime)", 
            VectorOp.class, "crop", 
            "Removes values that have time stamps that are outside the limits: yout[k] = yout[k] if (t[k]>=startTime and t[k]<=endTime) else delete");
    public static final VectorOperation CLAMP = new VectorOperation(
            "clamp", "clamp(min, max)", 
            VectorOp.class, "clamp", 
            "Clamps the input between the given limits: yout[k] = min(max, max(min, yout[k]))");
    public static final VectorOperation DISCARDOUTLIERS = new VectorOperation(
            "discardOutliers", "discardOutliers(min, max)", 
            VectorOp.class, "discardOutliers", 
            "Removes values from the input that are outside the given limits: yout[k] = yout[k] if (yout[k]>=min and yout[k]<=max) else delete");
    public static final VectorOperation REMOVEREPEATS = new VectorOperation(
            "removeRepeats", "removeRepeats()", 
            VectorOp.class, "removeRepeats", 
            "Removes repeated y values from the input: yout[k] = yout[k] if yout[k] != yout[k-1] else delete"); 
    public static final VectorOperation INTEGRATE = new VectorOperation(
            "integrate", "integrate(interpolationMode)", 
            VectorOp.class, "integrate", 
            "Integrates the input as a step function (with sample-hold or backward-sample-hold iterpolation) or with linear interpolation." +
            "The value of the interpolationMode parameter may be 'sample-hold', 'backward-sample-hold', or 'linear'.\n");
    public static final VectorOperation TIMEAVERAGE = new VectorOperation(
            "timeAverage", "timeAverage(interpolationMode)", 
            VectorOp.class, "timeAverage", 
            "Calculates the time average of the input (integral divided by time)." +
            "The value of the interpolationMode parameter may be 'sample-hold', 'backward-sample-hold', or 'linear'.\n");
    public static final VectorOperation DIVIDEBYTIME = new VectorOperation(
            "divideByTime", "divideByTime()", 
            VectorOp.class, "divideByTime", 
            "Divides the input by the associated timestamp: yout[k] = y[k] / t[k]");
    public static final VectorOperation TIMETOSERIAL = new VectorOperation(
            "timeToSerial", "timeToSerial()", 
            VectorOp.class, "timeToSerial", 
            "Replaces time values with their indices: tout[k] = k");
    public static final VectorOperation SUBTRACTFIRSTVALUE = new VectorOperation(
            "subtractFirstValue", "subtractFirstValue()", 
            VectorOp.class, "subtractFirstValue", 
            "Subtract the first value from every subsequent value: yout[k] = y[k] - y[0]");

    public static final VectorOperation[] ALL = new VectorOperation[] {
            ADD, SUBTRACT, MUL, DIV, NEG, MOD, ABS, SUM, MEAN, MIN, MAX, DISCARD_NAN, 
            DISCARD_INF, DISCARD_NAN_INF, DIFFERENCE, TIMEDIFF, MOVING_AVERAGE, 
            TIMESHIFT, LINEARTREND, CROP, CLAMP, DISCARDOUTLIERS, REMOVEREPEATS, INTEGRATE, 
            TIMEAVERAGE, DIVIDEBYTIME, TIMETOSERIAL, SUBTRACTFIRSTVALUE
    };

    private static Map<String,VectorOperation> operations = new HashMap<>();

    static {
        for (VectorOperation op : ALL)
            operations.put(op.getName(), op);
    }
    
    private VectorOperations() {
    }

    public static VectorOperation get(String name) {
        return operations.get(name);
    }
}

