package org.omnetpp.figures.misc;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.StringUtils;

/**
 * Represents a single element of the canvas figures' transform parameters.
 * For example, a translate(10,20) or a scale(3).
 */
public class TransformDescription {
    private String operation;
    private List<Double> args;

    /**
     * Since Strings and Doubles are immutable, this counts as a deep copy constructor.
     */
    public TransformDescription(TransformDescription other) {
        operation = other.operation;
        args = new ArrayList<Double>(other.args);
    }

    public TransformDescription(String operation, List<Double> args) {
        if (!isValid(operation, args)) {
            throw new IllegalArgumentException("Invalid TransformDescription.");
        }
        this.operation = operation;
        this.args = new ArrayList<Double>(args);
    }

    public String getOperation() {
        return operation;
    }

    public void setOperation(String operation) {
        this.operation = operation;
    }

    @Override
    public String toString() {
        String[] argStrings = new String[args.size()];
        for (int i = 0; i < args.size(); ++i) {
            argStrings[i] = Converter.doubleToString(args.get(i));
        }
        return operation + "(" + StringUtils.join(argStrings, ",") + ")";
    }

    public boolean isValid() {
        return isValid(operation, args);
    }

    public static boolean isValid(String operation, List<Double> args) {
        if (operation.equals("translate")) {
            return args.size() == 2;
        } else if (operation.equals("rotate")) {
            return args.size() == 1 || args.size() == 3;
        } else if (operation.equals("scale")) {
            return args.size() >= 1 && args.size() <= 4;
        } else if (operation.equals("skewx")) {
            return args.size() == 1 || args.size() == 2;
        } else if (operation.equals("skewy")) {
            return args.size() == 1 || args.size() == 2;
        } else if (operation.equals("matrix")) {
            return args.size() == 6;
        } else {
            return false;
        }
    }

    public static TransformDescription parse(String desc) {
        String[] parts;

        String operation = "";
        List<Double> arguments = new ArrayList<Double>();

        if (desc.trim().startsWith("(")) {
            operation = "matrix";

            parts = desc.split("[()\\s]");
            for (int i = 0; i < parts.length; ++i) {
                if (parts[i].isEmpty())
                    continue;

                Double arg = Converter.stringToOptionalDouble(parts[i]);
                if (arg == null) {
                    return null; // not a number
                } else {
                    arguments.add(arg);
                }
            }

        } else {
            parts = desc.split("\\(|,|\\)");

            if (parts.length < 2) {
                return null;
            }

            operation = parts[0].trim();

            for (int i = 1; i < parts.length; ++i) {
                if (parts[i].isEmpty())
                    continue;

                Double arg = Converter.stringToDouble(parts[i]);
                if (arg == null) {
                    return null; // not a number
                } else {
                    arguments.add(arg);
                }
            }
        }

        if (parts.length < 2) {
            return null; // any operation without a name or any arguments is treated as invalid
        }


        return isValid(operation, arguments) ? new TransformDescription(operation, arguments) :
            null;
    }

    public Transform getTransform() {
        Transform tr = new Transform();

        if (!isValid()) {
            return tr;
        }

        if (operation.equals("translate")) {
            tr.translate(args.get(0), args.get(1));
        } else if (operation.equals("rotate")) {
            if (args.size() >= 3)
                tr.rotate(Math.toRadians(args.get(0)), args.get(1), args.get(2));
            else
                tr.rotate(Math.toRadians(args.get(0)));
        } else if (operation.equals("scale")) {
            // same as cFigure::parseTransform(): scale(s), scale(sx,sy), scale(s,cx,cy), scale(sx,sy,cx,cy)
            if (args.size() == 1)
                tr.scale(args.get(0), args.get(0));
            else if (args.size() == 2)
                tr.scale(args.get(0), args.get(1));
            else if (args.size() == 3)
                tr.scale(args.get(0), args.get(0), args.get(1), args.get(2));
            else
                tr.scale(args.get(0), args.get(1), args.get(2), args.get(3));
        } else if (operation.equals("skewx")) {
            // skewx(coeff) or skewx(coeff,cy)
            if (args.size() == 1)
                tr.skewx(args.get(0));
            else
                tr.skewx(args.get(0), args.get(1));
        } else if (operation.equals("skewy")) {
            // skewy(coeff) or skewy(coeff,cx)
            if (args.size() == 1)
                tr.skewy(args.get(0));
            else
                tr.skewy(args.get(0), args.get(1));
        } else if (operation.equals("matrix")) {
            tr.setElements(args);
        }

        return tr;
    }
}