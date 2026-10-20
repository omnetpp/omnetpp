package org.omnetpp.ned.core;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.regex.Pattern;

import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INedErrorStore;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.interfaces.IHasProperties;
import org.omnetpp.ned.model.pojo.PropertyElement;
import org.omnetpp.ned.model.pojo.PropertyKeyElement;

import static org.omnetpp.common.canvas.CanvasFigureConstants.*;

public final class NedCanvasFigureValidator {

    private INedErrorStore errors;
    private PropertyElementEx property;

    public static final List<String> validTypes = Collections.unmodifiableList(Arrays.asList(new String[] {
            FTYPE_LINE, FTYPE_POLYLINE, FTYPE_POLYGON, FTYPE_RECTANGLE, FTYPE_OVAL, FTYPE_RING, FTYPE_PIESLICE, FTYPE_ARC, FTYPE_GROUP, FTYPE_PANEL, FTYPE_TEXT, FTYPE_LABEL, FTYPE_IMAGE, FTYPE_PIXMAP, FTYPE_ICON, FTYPE_PATH }));
    public static final List<String> validBooleans = Collections.unmodifiableList(Arrays.asList(new String[] { "true", "false" }));
    public static final List<String> validAnchors = Collections.unmodifiableList(Arrays.asList(new String[] {
            PVAL_ANCHOR_N, PVAL_ANCHOR_W, PVAL_ANCHOR_S, PVAL_ANCHOR_E, PVAL_ANCHOR_C, PVAL_ANCHOR_CENTER,
            PVAL_ANCHOR_NE, PVAL_ANCHOR_NW, PVAL_ANCHOR_SW, PVAL_ANCHOR_SE,
            PVAL_ANCHOR_START, PVAL_ANCHOR_MIDDLE, PVAL_ANCHOR_END, }));
    public static final List<String> validArrowHeads = Collections.unmodifiableList(Arrays.asList(new String[] { "none", "simple", "triangle", "barbed" }));
    public static final List<String> validLineStyles = Collections.unmodifiableList(Arrays.asList(new String[] { "solid", "dashed", "dotted" }));
    public static final List<String> validJoinStyles = Collections.unmodifiableList(Arrays.asList(new String[] { "bevel", "miter", "round" }));
    public static final List<String> validFillRules = Collections.unmodifiableList(Arrays.asList(new String[] { "evenodd", "nonzero" }));
    public static final List<String> validCapStyles = Collections.unmodifiableList(Arrays.asList(new String[] { "butt", "square", "round" }));
    public static final List<String> validAlignments = Collections.unmodifiableList(Arrays.asList(new String[] { "left", "center", "right" }));
    public static final List<String> validFontStyles = Collections.unmodifiableList(Arrays.asList(new String[] { "bold", "italic", "underline" }));
    public static final List<String> validTransforms = Collections.unmodifiableList(Arrays.asList(new String[] { "translate", "scale", "rotate", "skewx", "skewy", "matrix" }));
    public static final List<String> validInterpolations = Collections.unmodifiableList(Arrays.asList(new String[] { "none", "fast", "best" }));

    private static List<String> validSubmodulesPropertyKeys;
    private static List<String> validLinePropertyKeys;
    private static List<String> validPolylinePropertyKeys;
    private static List<String> validPolygonPropertyKeys;
    private static List<String> validRectanglePropertyKeys;
    private static List<String> validOvalPropertyKeys;
    private static List<String> validRingPropertyKeys;
    private static List<String> validPieslicePropertyKeys;
    private static List<String> validArcPropertyKeys;
    private static List<String> validGroupPropertyKeys;
    private static List<String> validTextPropertyKeys;
    private static List<String> validLabelPropertyKeys;
    private static List<String> validImagePropertyKeys;
    private static List<String> validPixmapPropertyKeys;
    private static List<String> validPathPropertyKeys;
    private static List<String> validPanelPropertyKeys;

    private static Pattern numberPattern; // Double.parseDouble() accepts a trailing f or d, but we shouldn't
    private static Pattern pathAtomPattern;
    private static Pattern transformPattern;

    static {
        validSubmodulesPropertyKeys = new ArrayList<String>();
        validSubmodulesPropertyKeys.add(PKEY_ZINDEX);


        List<String> commonFigurePropertyKeys = Arrays.asList(new String[] { PKEY_TAGS, PKEY_VISIBLE, PKEY_TOOLTIP, PKEY_TYPE, PKEY_ZINDEX, PKEY_TRANSFORM });
        List<String> anchoredRectanglePropertyKeys = Arrays.asList(new String[] { PKEY_BOUNDS, PKEY_POS, PKEY_SIZE, PKEY_ANCHOR });
        List<String> commonLinePropertyKeys = Arrays.asList(new String[] { PKEY_LINEWIDTH, PKEY_ZOOMLINEWIDTH, PKEY_LINECOLOR, PKEY_LINEOPACITY, PKEY_LINESTYLE, PKEY_CAPSTYLE });
        List<String> commonShapePropertyKeys = Arrays.asList(new String[] { PKEY_LINEWIDTH, PKEY_ZOOMLINEWIDTH, PKEY_LINECOLOR, PKEY_LINEOPACITY, PKEY_FILLCOLOR, PKEY_FILLOPACITY, PKEY_LINESTYLE, PKEY_CLIPCHILDREN});
        List<String> arrowHeadPropertyKeys = Arrays.asList(new String[] { PKEY_STARTARROWHEAD, PKEY_ENDARROWHEAD });


        validLinePropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validLinePropertyKeys.add(PKEY_POINTS);
        validLinePropertyKeys.addAll(commonLinePropertyKeys);
        validLinePropertyKeys.addAll(arrowHeadPropertyKeys);
        validLinePropertyKeys = Collections.unmodifiableList(validLinePropertyKeys);

        validPolylinePropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validPolylinePropertyKeys.add(PKEY_POINTS);
        validPolylinePropertyKeys.addAll(commonLinePropertyKeys);
        validPolylinePropertyKeys.add(PKEY_JOINSTYLE);
        validPolylinePropertyKeys.add(PKEY_SMOOTH);
        validPolylinePropertyKeys.addAll(arrowHeadPropertyKeys);
        validPolylinePropertyKeys = Collections.unmodifiableList(validPolylinePropertyKeys);

        validPolygonPropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validPolygonPropertyKeys.add(PKEY_POINTS);
        validPolygonPropertyKeys.addAll(commonShapePropertyKeys);
        validPolygonPropertyKeys.add(PKEY_JOINSTYLE);
        validPolygonPropertyKeys.add(PKEY_SMOOTH);
        validPolygonPropertyKeys.add(PKEY_FILLRULE);
        validPolygonPropertyKeys = Collections.unmodifiableList(validPolygonPropertyKeys);

        validRectanglePropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validRectanglePropertyKeys.addAll(anchoredRectanglePropertyKeys);
        validRectanglePropertyKeys.addAll(commonShapePropertyKeys);
        validRectanglePropertyKeys.add(PKEY_CORNERRADIUS);
        validRectanglePropertyKeys = Collections.unmodifiableList(validRectanglePropertyKeys);

        validOvalPropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validOvalPropertyKeys.addAll(anchoredRectanglePropertyKeys);
        validOvalPropertyKeys.addAll(commonShapePropertyKeys);
        validOvalPropertyKeys = Collections.unmodifiableList(validOvalPropertyKeys);

        validRingPropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validRingPropertyKeys.addAll(anchoredRectanglePropertyKeys);
        validRingPropertyKeys.addAll(commonShapePropertyKeys);
        validRingPropertyKeys.add(PKEY_INNERSIZE);
        validRingPropertyKeys = Collections.unmodifiableList(validRingPropertyKeys);

        validPieslicePropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validPieslicePropertyKeys.addAll(anchoredRectanglePropertyKeys);
        validPieslicePropertyKeys.addAll(commonShapePropertyKeys);
        validPieslicePropertyKeys.add(PKEY_STARTANGLE);
        validPieslicePropertyKeys.add(PKEY_ENDANGLE);
        validPieslicePropertyKeys = Collections.unmodifiableList(validPieslicePropertyKeys);

        validArcPropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validArcPropertyKeys.addAll(anchoredRectanglePropertyKeys);
        validArcPropertyKeys.addAll(commonLinePropertyKeys);
        validArcPropertyKeys.add(PKEY_STARTANGLE);
        validArcPropertyKeys.add(PKEY_ENDANGLE);
        validArcPropertyKeys.addAll(arrowHeadPropertyKeys);
        validArcPropertyKeys = Collections.unmodifiableList(validArcPropertyKeys);

        validGroupPropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        // A group has nothing except the common property keys.
        validGroupPropertyKeys = Collections.unmodifiableList(validGroupPropertyKeys);

        validTextPropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validTextPropertyKeys.add(PKEY_POS);
        validTextPropertyKeys.add(PKEY_TEXT);
        validTextPropertyKeys.add(PKEY_FONT);
        validTextPropertyKeys.add(PKEY_ALIGN);
        validTextPropertyKeys.add(PKEY_COLOR);
        validTextPropertyKeys.add(PKEY_OPACITY);
        validTextPropertyKeys.add(PKEY_HALO);
        validTextPropertyKeys.add(PKEY_ANCHOR);
        validTextPropertyKeys = Collections.unmodifiableList(validTextPropertyKeys);

        validLabelPropertyKeys = new ArrayList<String>(validTextPropertyKeys);
        validLabelPropertyKeys.add(PKEY_ANGLE);
        validLabelPropertyKeys = Collections.unmodifiableList(validLabelPropertyKeys);

        validImagePropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validImagePropertyKeys.add(PKEY_IMAGE);
        validImagePropertyKeys.add(PKEY_OPACITY);
        validImagePropertyKeys.add(PKEY_INTERPOLATION);
        validImagePropertyKeys.add(PKEY_TINT);
        validImagePropertyKeys.addAll(anchoredRectanglePropertyKeys);
        validImagePropertyKeys = Collections.unmodifiableList(validImagePropertyKeys);

        validPixmapPropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validPixmapPropertyKeys.add(PKEY_OPACITY);
        validPixmapPropertyKeys.add(PKEY_INTERPOLATION);
        validPixmapPropertyKeys.add(PKEY_TINT);
        validPixmapPropertyKeys.addAll(anchoredRectanglePropertyKeys);
        validPixmapPropertyKeys.add(PKEY_RESOLUTION);
        validPixmapPropertyKeys.add(PKEY_FILLCOLOR);
        validPixmapPropertyKeys = Collections.unmodifiableList(validPixmapPropertyKeys);

        validPathPropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validPathPropertyKeys.addAll(commonShapePropertyKeys);
        validPathPropertyKeys.add(PKEY_PATH);
        validPathPropertyKeys.add(PKEY_OFFSET);
        validPathPropertyKeys.add(PKEY_JOINSTYLE);
        validPathPropertyKeys.add(PKEY_CAPSTYLE);
        validPathPropertyKeys.add(PKEY_FILLRULE);
        validPathPropertyKeys = Collections.unmodifiableList(validPathPropertyKeys);

        validPanelPropertyKeys = new ArrayList<String>(commonFigurePropertyKeys);
        validPanelPropertyKeys.add(PKEY_POS);
        validPanelPropertyKeys.add(PKEY_ANCHORPOINT);
        validPanelPropertyKeys = Collections.unmodifiableList(validPanelPropertyKeys);


        String regexNumber = "([-+]?((\\d*\\.?\\d+)|(\\d+\\.))([eE][-+]?\\d+)?)";

        numberPattern = Pattern.compile("^" + regexNumber + "$");

        String regexPoint = "(\\s*" + regexNumber + "\\s+" + regexNumber + "\\s*)";
        String regexAtom = "\\s*(" +
            "([zZ])" +
            "|([hHvV]\\s+" + regexNumber + "+)" +
            "|([mMlLtT]\\s+" + regexPoint + "+)" +
            "|([qQsS]\\s+(" + regexPoint + "\\s+" + regexPoint + ")+)" +
            "|([cC]\\s+(" + regexPoint + "\\s+" + regexPoint + "\\s+" + regexPoint + ")+)" +
            "|([aA]\\s+(" + regexPoint + "\\s+" + regexNumber + "\\s+[01]\\s+[01]\\s+" + regexPoint + ")+)" +
            ")\\s+"; // + to enforce some whitespace between atoms, but this way we must add a space to the end of the path string

        pathAtomPattern = Pattern.compile(regexAtom);

        // the ((a b) (c d) (t1 t2)) format
        String regexColumn = "(\\(\\s*" + regexNumber+ "\\s+" + regexNumber + "\\s*\\)\\s*)";
        String transformRegex1 = "\\(\\s*" + regexColumn + "{3}\\)";

        // the oper1(arg),oper2(arg1,arg2) format
        String transformRegex2 = "(" + StringUtils.join(validTransforms, "|") + ")"
                + "\\s*\\(((\\s*" + regexNumber + "\\s*,)*\\s*" + regexNumber + ")?\\s*\\)";

        transformPattern = Pattern.compile("^\\s*((" + transformRegex1 + ")|(" + transformRegex2 + "))\\s*$");
    }


    public NedCanvasFigureValidator(PropertyElement node, INedErrorStore e) {
        property = (PropertyElementEx)node;
        errors = e;
    }


    private void addError(String message) {
        errors.addError(property, "In figure '" + property.getIndex() + "': " + message);
    }

    private void addWarning(String message) {
        errors.addWarning(property, "In figure '" + property.getIndex() + "': " + message);
    }

    // also used in the PropertySource, when the type changes, invalid property keys are removed
    public static List<String> getValidPropertyKeysForType(String type) {
        if (type == null) {
            return null;
        }

        if (type.equals(FTYPE_RECTANGLE)) { return validRectanglePropertyKeys; } else
        if (type.equals(FTYPE_OVAL))      { return validOvalPropertyKeys;      } else
        if (type.equals(FTYPE_RING))      { return validRingPropertyKeys;      } else
        if (type.equals(FTYPE_PIESLICE))  { return validPieslicePropertyKeys;  } else
        if (type.equals(FTYPE_ARC))       { return validArcPropertyKeys;       } else
        if (type.equals(FTYPE_LINE))      { return validLinePropertyKeys;      } else
        if (type.equals(FTYPE_POLYLINE))  { return validPolylinePropertyKeys;  } else
        if (type.equals(FTYPE_POLYGON))   { return validPolygonPropertyKeys;   } else
        if (type.equals(FTYPE_TEXT))      { return validTextPropertyKeys;      } else
        if (type.equals(FTYPE_LABEL))     { return validLabelPropertyKeys;     } else
        if (type.equals(FTYPE_GROUP))     { return validGroupPropertyKeys;     } else
        if (type.equals(FTYPE_PANEL))     { return validPanelPropertyKeys;     } else
        if (type.equals(FTYPE_IMAGE))     { return validImagePropertyKeys;     } else
        if (type.equals(FTYPE_PIXMAP))    { return validPixmapPropertyKeys;    } else
        if (type.equals(FTYPE_ICON))      { return validImagePropertyKeys;     } else
        if (type.equals(FTYPE_PATH))      { return validPathPropertyKeys;      } else
                                          { return null;                       }
    }

    public static List<String> getValidValuesForPropertyKey(String param) {
        if (param == null) {
            return null;
        }

        if (param.equals(PKEY_ALIGN)) { return validAlignments;
        } else if (param.equals(PKEY_ANCHOR)) { return validAnchors;
        } else if (param.equals(PKEY_CAPSTYLE)) { return validCapStyles;
        } else if (param.equals(PKEY_STARTARROWHEAD) || param.equals(PKEY_ENDARROWHEAD)) { return validArrowHeads;
        } else if (param.equals(PKEY_VISIBLE) || param.equals(PKEY_SMOOTH) || param.equals(PKEY_ZOOMLINEWIDTH) || param.equals(PKEY_HALO) || param.equals(PKEY_CLIPCHILDREN)) { return validBooleans;
        } else if (param.equals(PKEY_JOINSTYLE)) { return validJoinStyles;
        } else if (param.equals(PKEY_LINESTYLE)) { return validLineStyles;
        } else if (param.equals(PKEY_FILLRULE)) { return validFillRules;
        } else if (param.equals(PKEY_TYPE)) { return validTypes;
        } else if (param.equals(PKEY_TRANSFORM)) { return validTransforms;
        } else if (param.equals(PKEY_INTERPOLATION)) { return validInterpolations;
        } else { return null;
        }
    }

    public static boolean getTypeHasBounds(String type) {
        List<String> keys = getValidPropertyKeysForType(type);
        return (keys != null) && keys.contains(PKEY_BOUNDS);
    }

    public void validate() {
        checkSingleNumeric(PKEY_ZINDEX);

        String index = property.getIndex();

        if (index.equals("submodules")) {
            validateSubmodules();

            return;
        }

        if ((property == null)
                || (property.getParent() == null)
                || (property.getParent().getParent() == null)) {
            return;
        }

        Map<String, PropertyElementEx> figures = ((IHasProperties)property.getParent().getParent()).getProperties().get("figure");

        if (index.contains(".")) {
            String parentIndex = index.substring(0, index.lastIndexOf('.'));

            if (!(figures.containsKey(parentIndex))) {
                addError("This figure is orphaned! It should be the child of " + parentIndex + ", but no figure with that name exists.");
            }
        }

        checkSingleBoolean(PKEY_VISIBLE);
        checkTransform();
        checkAnchor();


        String type = property.getValue(PKEY_TYPE);

        if (type == null) {
            errors.addError(property, "All figures, except [submodules], must have a type.");

            return;
        }

        // NOTE: since "type" can be any value (for custom cFigure subclasses), not passing in `validTypes` here.
        checkSingleValue(PKEY_TYPE);

        checkPropertyKeys(getValidPropertyKeysForType(type));

        if (type.equals(FTYPE_RECTANGLE)) { validateRectangle();
        } else if (type.equals(FTYPE_OVAL)) { validateOval();
        } else if (type.equals(FTYPE_RING)) { validateRing();
        } else if (type.equals(FTYPE_PIESLICE)) { validatePieslice();
        } else if (type.equals(FTYPE_ARC)) { validateArc();
        } else if (type.equals(FTYPE_LINE)) { validateLine();
        } else if (type.equals(FTYPE_POLYLINE)) { validatePolyline();
        } else if (type.equals(FTYPE_POLYGON)) { validatePolygon();
        } else if (type.equals(FTYPE_TEXT)) { validateText();
        } else if (type.equals(FTYPE_LABEL)) { validateLabel();
        } else if (type.equals(FTYPE_GROUP)) { validateGroup();
        } else if (type.equals(FTYPE_PANEL)) { validatePanel();
        } else if (type.equals(FTYPE_IMAGE)) { validateImage();
        } else if (type.equals(FTYPE_PIXMAP)) { validatePixmap();
        } else if (type.equals(FTYPE_ICON)) { validateImage();
        } else if (type.equals(FTYPE_PATH)) { validatePath(); }
    }


    private void validateSubmodules() {
        checkPropertyKeys("[submodules]", validSubmodulesPropertyKeys);
    }


    private void validateGroup() {
        // A group has nothing except the common property keys.
    }

    private void validatePanel() {
        checkPoint(PKEY_POS);
        checkPoint(PKEY_ANCHORPOINT);
    }


    private void validateAbstractShape() {
        checkSingleNumeric(PKEY_LINEWIDTH);
        checkSingleColor(PKEY_LINECOLOR);
        checkOpacity(PKEY_LINEOPACITY);
        checkSingleColor(PKEY_FILLCOLOR);
        checkOpacity(PKEY_FILLOPACITY);
        checkLineStyle();
        checkSingleBoolean(PKEY_CLIPCHILDREN);
    }

    private void validateAbstractLine() {
        checkSingleNumeric(PKEY_LINEWIDTH);
        checkSingleColor(PKEY_LINECOLOR);
        checkOpacity(PKEY_LINEOPACITY);
        checkLineStyle();
        checkCapStyle();
        checkArrowHeads();
    }


    private void validateArc() {
        validateAbstractLine();
        checkAnchoredRectangle();
        checkSingleNumeric(PKEY_STARTANGLE);
        checkSingleNumeric(PKEY_ENDANGLE);
    }

    private void validatePieslice() {
        validateAbstractShape();
        checkAnchoredRectangle();
        checkSingleNumeric(PKEY_STARTANGLE);
        checkSingleNumeric(PKEY_ENDANGLE);
    }

    private void validateText() {
        checkSingleValue(PKEY_TEXT);
        checkRequired(PKEY_TEXT);
        checkSingleValue(PKEY_ALIGN, validAlignments);
        checkSingleColor(PKEY_COLOR);
        checkOpacity(PKEY_OPACITY);
        checkSingleBoolean(PKEY_HALO);
        checkFont();
    }

    private void validateLabel() {
        validateText();
        checkSingleNumeric(PKEY_ANGLE);
    }

    private void validatePolygon() {
        validateAbstractShape();
        checkPointList(PKEY_POINTS);
        checkRequired(PKEY_POINTS);
        checkSingleBoolean(PKEY_SMOOTH);
        checkJoinStyle();
        checkFillRule();
    }

    private void validatePolyline() {
        validateAbstractLine();
        checkPointList(PKEY_POINTS);
        checkRequired(PKEY_POINTS);
        checkSingleBoolean(PKEY_SMOOTH);
        checkJoinStyle();
    }

    private void validateLine() {
        validateAbstractLine();
        checkPointList(PKEY_POINTS);
        checkRequired(PKEY_POINTS);
        List<String> pointValues = property.getValueAsList(PKEY_POINTS);
        if (pointValues != null && pointValues.size() > 4) {
            addError("The coords property key must have only 4 values if the figure is a line. Consider using a polyline.");
        }
    }

    private void validateOval() {
        validateAbstractShape();
        checkAnchoredRectangle();
    }

    private void validateRing() {
        validateAbstractShape();
        checkAnchoredRectangle();
        checkHalfPoint(PKEY_INNERSIZE);
    }

    private void validateRectangle() {
        validateAbstractShape();
        checkAnchoredRectangle();
        checkHalfPoint(PKEY_CORNERRADIUS);
    }

    private void validateImage() {
        checkOpacity(PKEY_OPACITY);
        checkColor(PKEY_TINT);
        checkRange(PKEY_TINT, 1, 0.0, 1.0);
        checkSingleValue(PKEY_IMAGE);
        checkRequired(PKEY_IMAGE);

        List<String> sizeValues = property.getValueAsList(PKEY_SIZE);
        if (sizeValues != null && sizeValues.size() > 2) {
            addWarning("The size property key must have 1 or 2 values. The rest is ignored.");
        }

        if (sizeValues != null) {
            for (String value : sizeValues) {
                String trimmed = value.trim();
                if (!trimmed.isEmpty()) {
                    /*if (Converter.stringToDouble(trimmed) == null) {
                        addError("All values of the size property key must be numeric.");
                    }*/
                }
            }
        }

        List<String> images = property.getValueAsList(PKEY_IMAGE);
        if (images != null && !images.isEmpty()) {
            ImageFactory factory = ImageFactory.of(property.getSelfOrEnclosingTypeElement().getNedTypeInfo().getProject());
            Image img = factory.getImage(images.get(0));

            if ((img == factory.getImage(ImageFactory.UNKNOWN)) && (!images.get(0).equals(ImageFactory.UNKNOWN))) {
                addError("Unknown image name: " + images.get(0));
            }
        }
    }

    private void validatePixmap() {
        checkOpacity(PKEY_OPACITY);
        checkColor(PKEY_TINT);
        checkRange(PKEY_TINT, 1, 0.0, 1.0);
        checkSingleValue(PKEY_INTERPOLATION, validInterpolations);

        List<String> resolution = property.getValueAsList(PKEY_RESOLUTION);
        if (resolution != null) {
            checkAllNumeric(PKEY_RESOLUTION);
            if (resolution.size() != 2) {
                addError("The resolution property key must have 2 values: width and height.");
            }
        }
        checkSingleColor(PKEY_FILLCOLOR);

        checkAnchoredRectangle();
    }

    private void validatePath() {
        validateAbstractShape();
        checkSingleValue(PKEY_PATH);
        checkRequired(PKEY_PATH);
        checkPoint(PKEY_OFFSET);
        checkJoinStyle();
        checkCapStyle();
        checkFillRule();

        String path = property.getValue(PKEY_PATH);
        if (path == null) {
            return;
        }

        // the space is to make the last atom always end with whitespace
        String[] mismatches = pathAtomPattern.split(path + " ");

        for (String e : mismatches) {
            String t = e.trim();
            if (!t.isEmpty()) {
                addError("Invalid path atom: " + t);
            }
        }

        if (!path.trim().startsWith("M")) {
            addError("The first path atom must be M.");
        }

        char lastAtom = ' ';

        for (int i = 0; i < path.length(); ++i) {
            char currentChar = Character.toUpperCase(path.charAt(i));

            if ((currentChar == 'T') && (lastAtom != 'Q') && (lastAtom != 'T')) {
                addError("Every T atom must be preceded by a Q, ar an other T atom.");
            }

            if ((currentChar == 'S') && (lastAtom != 'C') && (lastAtom != 'S')) {
                addError("Every S atom must be preceded by a C, ar an other S atom.");
            }

            if (Character.isLetter(currentChar)) {
                lastAtom = currentChar;
            }
        }
    }

    private void checkPropertyKeys(List<String> list) {
        checkPropertyKeys(property.getValue(PKEY_TYPE), list);
    }

    private void checkPropertyKeys(String type, List<String> list) {
        if (list == null) {
            return;
        }

        for (PropertyKeyElement element = property.getFirstPropertyKeyChild();
                element != null; element = element.getNextPropertyKeySibling()) {

            String name = element.getName();
            if (!name.isEmpty() && !list.contains(name) && !name.startsWith("x-")) {
                addWarning("Unknown property key for " + type + ": " + name + ". Valid property keys are: " + StringUtils.join(list, ", ") + ".");
            }
        }
    }

    private void checkSingleValue(String key) {
        List<String> values = property.getValueAsList(key);
        if (values != null && values.size() > 1) {
            addError("The " + key + " property key must have a single value only.");
        }
    }

    private void checkSingleBoolean(String key) {
        checkSingleValue(key, validBooleans);
    }

    private void checkNumeric(String key, int index) {
        List<String> values = property.getValueAsList(key);
        if (values == null || values.size() <= index) {
            return;
        }
        if (!numberPattern.matcher(values.get(index).trim()).matches()) {
            addError("The " + (index+1) + ". value of the " + key + " property key must be numeric.");
        }
    }

    private void checkAllNumeric(String key) {
        List<String> values = property.getValueAsList(key);
        if (values == null)
            return;
        for (String s : values) {
            if (!numberPattern.matcher(s.trim()).matches()) {
                addError("The " + key + " property key must have numeric values only.");
                break;
            }
        }
    }

    private void checkSingleNumeric(String key) {
        checkSingleValue(key);
        checkAllNumeric(key);
    }

    private void checkRange(String key, int index, double min, double max) {
        checkNumeric(key, index);

        List<String> values = property.getValueAsList(key);
        if (values == null || values.size() <= index) {
            return;
        }

        double value = 0;
        try {
            value = Double.parseDouble(values.get(index));
        } catch (NumberFormatException e) {
            // Invalid numerals are already reported in checkNumeric();
            return;
        }

        if ((value < min) || (value > max)) {
            addError("The value of " + key + " must be between " + min + " and " + max + " (inclusive).");
        }
    }

    private void checkOpacity(String key) {
        checkRange(key, 0, 0.0, 1.0);
    }

    private void checkFirstValue(String key, List<String> list) {
        String firstValue = property.getValue(key);

        if ((firstValue != null) && !list.contains(firstValue)) {
            addError("Invalid value for the " + key + " property key. Valid values are: " + StringUtils.join(list, ", ") + ".");
        }
    }

    private void checkRequired(String key) {
        List<String> values = property.getValueAsList(key);
        if (values == null || values.isEmpty()) {
            addError("The " + key + " property key is required.");
        }
    }

    private void checkSingleValue(String key, List<String> list) {
        checkSingleValue(key);
        checkFirstValue(key, list);
    }

    private void checkAnchoredRectangle() {
        List<String> bounds = property.getValueAsList(PKEY_BOUNDS);

        if (bounds != null && bounds.size() > 0) {
            if (bounds.size() != 4)
                addError("The bounds property key must have exactly 4 values: x, y, width, height.");
            checkAllNumeric(PKEY_BOUNDS);

            List<String> pos = property.getValueAsList(PKEY_POS);
            List<String> size = property.getValueAsList(PKEY_SIZE);
            List<String> anchor = property.getValueAsList(PKEY_ANCHOR);

            if (!((pos == null || pos.isEmpty()) && (size == null || size.isEmpty()) && (anchor == null || anchor.isEmpty()))) {
                addError("If the rectangle is specified with the bounds property key, the pos, size and anchor keys are forbidden.");
            }
        } else {
            checkAnchor();
            checkLocationAndSize(PKEY_POS, PKEY_SIZE);
        }
    }

    private void checkAnchor() {
        checkSingleValue(PKEY_ANCHOR, validAnchors);
    }

    private void checkLocationAndSize(String coordsKey, String sizeKey) {
        checkRequired(coordsKey);
        checkRequired(sizeKey);
        checkPoint(sizeKey);
        checkPoint(coordsKey);
    }

    private void checkHalfPoint(String key) { // couldn't come up with a better name
        checkAllNumeric(key);

        List<String> coords = property.getValueAsList(key);

        if (coords != null && coords.size() > 2) {
            addError("The " + key + " property key must have 1 or 2 values.");
        }
    }

    private void checkPoint(String key) {
        checkAllNumeric(key);

        List<String> coords = property.getValueAsList(key);

        if (coords != null && coords.size() != 2) {
            addError("The " + key + " property key must have 2 values.");
        }
    }

    private void checkPointList(String key) {
        checkAllNumeric(key);

        List<String> coords = property.getValueAsList(key);
        if (coords == null)
            return;

        if (coords.size() < 4) {
            addError("The " + key + " property key must have at least 4 values, as it is a point list here.");
        }

        if ((coords.size() % 2) == 1) {
            addWarning("The " + key + " property key has an odd number of values, the last one is ignored.");
        }
    }

    private void checkSingleColor(String key) {
        checkSingleValue(key);
        checkColor(key);
    }

    private void checkColor(String key) {
        String color = property.getValue(key);

        if ((color != null) && (ColorFactory.asColor(color) == null)) {
            addError("Invalid value for the " + key + " property key. Valid values are English color names and hex codes as #RRGGBB or @HHSSBB.");
        }
    }

    private void checkArrowHeads() {
        checkSingleValue(PKEY_STARTARROWHEAD, validArrowHeads);
        checkSingleValue(PKEY_ENDARROWHEAD, validArrowHeads);
    }

    private void checkLineStyle() {
        checkSingleValue(PKEY_LINESTYLE, validLineStyles);
    }

    private void checkJoinStyle() {
        checkSingleValue(PKEY_JOINSTYLE, validJoinStyles);
    }

    private void checkCapStyle() {
        checkSingleValue(PKEY_CAPSTYLE, validCapStyles);
    }

    private void checkFillRule() {
        checkSingleValue(PKEY_FILLRULE, validFillRules);
    }

    private void checkTransform() {
        List<String> transforms = property.getValueAsList(PKEY_TRANSFORM);
        if (transforms == null)
            return;

        List<String> errors = checkTransform(transforms);

        for (String error : errors) {
            addError(error);
        }
    }

    public static List<String> checkTransform(List<String> transforms) {
        List<String> errors = new ArrayList<String>();

        outerLoop:
        for (String tr : transforms) {
            // it is not in the ((a b) (c d) (t1 t2)) form, checking the operations
            if (!tr.trim().startsWith("(")) {
                String[] temp = tr.trim().split("\\(");

                String operation = temp.length > 0 ? temp[0].trim() : null;

                if (temp.length < 2 || !validTransforms.contains(operation)) {
                    errors.add("Invalid transformation: " + operation + ". Valid transformations are: " + StringUtils.join(validTransforms, ",  ") + ".");
                    continue;
                }

                temp = temp[1].split("\\(|,|\\)");

                for (int i = 1; i < temp.length; ++i) {
                    String param = temp[i].trim();
                    if (!numberPattern.matcher(param.trim()).matches()) {
                        errors.add("The " + (i+1) + ". argument ('" + param + "') of '" + operation + "' is not numeric.");
                        continue outerLoop;
                    }
                }

                if (operation.equals("translate")) {
                    if (temp.length != 2) {
                        errors.add("The translate operation must have 2 arguments.");
                    }
                } else if (operation.equals("rotate")) {
                    if ((temp.length != 1) && (temp.length != 3)) {
                        errors.add("The rotate operation must have 1 or 3 arguments.");
                    }
                } else if (operation.equals("scale")) {
                    if ((temp.length < 1) || (temp.length > 4)) {
                        errors.add("The scale operation must have 1, 2, 3 or 4 arguments.");
                    }
                } else if (operation.equals("skewx") || operation.equals("skewy")) {
                    if ((temp.length != 1) && (temp.length != 2)) {
                        errors.add("The " + operation + " operation must have 1 or 2 arguments.");
                    }
                } else if (operation.equals("matrix")) {
                    if ((temp.length != 6)) {
                        errors.add("The matrix operation must have 6 arguments.");
                    }
                }
            }

            if (!transformPattern.matcher(tr.trim()).matches()
                    /* || TransformDescription.parse(tr) == null
                     * // ^ This would be better I guess.
                     * But would need an inappropriate Package-Import ( ned.core -> figures.misc ) */
                    ) {
                errors.add("Invalid transformation format: '" + tr + "'. The correct format is either transform=oper1(arg),oper2(arg1,arg2); or transform=((a b) (c d) (t1 t2));.");
                continue;
            }
        }

        return errors;
    }

    private void checkFont() {
        List<String> font = property.getValueAsList(PKEY_FONT);

        // If the font property key is omitted, or only the typeface name is given, we can't do much.
        if (font == null || font.size() < 2) {
            return;
        }

        String fontSizeString = font.get(1);

        if (!fontSizeString.isEmpty()) {
            double fontSize = 0;

            try {
                fontSize = Double.parseDouble(fontSizeString);
            } catch (NumberFormatException e) {
                addError("The second value of the font property key must be numeric, or empty, as it is the font size.");
                return;
            }

            if (fontSize < 0) {
                addError("The second value of the font property key must be non-negative, or empty, as it is the font size.");
                return;
            }
        }

        if (font.size() < 3) {
            return;
        }

        String fontStyle = font.get(2);

        if (!fontStyle.equals("normal")) {
            List<String> fontStyles = Arrays.asList(fontStyle.split(" "));

            for (String style : fontStyles) {
                if (!style.isEmpty() && !validFontStyles.contains(style)) {
                    addError("Invalid font style: " + style
                            + ". The third value of the font property key must be either normal, or a space-separated list of the following strings: "
                            + StringUtils.join(validFontStyles, ", ") + ".");
                    break;
                }
            }
        }

        if (font.size() > 3) {
            addWarning("The font property key should have only 3 values: name, size, style. The rest is ignored.");
        }
    }
}
