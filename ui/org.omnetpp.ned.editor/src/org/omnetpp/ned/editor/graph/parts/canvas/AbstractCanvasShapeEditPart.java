package org.omnetpp.ned.editor.graph.parts.canvas;

import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.figures.canvas.AbstractCanvasShape;
import org.omnetpp.figures.canvas.AbstractCanvasShape.FillRule;
import org.omnetpp.figures.canvas.AbstractCanvasShape.JoinStyle;
import org.omnetpp.figures.canvas.AbstractCanvasShape.LineStyle;

import static org.omnetpp.common.canvas.CanvasFigureConstants.*;

public abstract class AbstractCanvasShapeEditPart extends AbstractCanvasFigureEditPart {

    @Override // For convenience.
    public AbstractCanvasShape getFigure() {
        return (AbstractCanvasShape)super.getFigure();
    }

    @Override
    protected void refreshVisuals() {
        super.refreshVisuals();

        AbstractCanvasShape figure = getFigure();

        Color bgColor = parseColor(PKEY_FILLCOLOR);
        if (bgColor == null) {
            figure.setFill(false);
        } else {
            figure.setFill(true);
            figure.setBackgroundColor(bgColor);
        }

        Color fgColor = parseColor(PKEY_LINECOLOR);
        if (fgColor == null) {
            figure.setForegroundColor(ColorFactory.BLACK);
        } else {
            figure.setForegroundColor(fgColor);
        }

        Boolean zoomLineWidth = parseBoolean(PKEY_ZOOMLINEWIDTH);
        if (zoomLineWidth == null) {
            figure.setZoomLineWidth(false);
        } else {
            figure.setZoomLineWidth(zoomLineWidth);
        }


        Double lineOpacity = parseDouble(PKEY_LINEOPACITY);
        figure.setLineOpacity((lineOpacity == null) ? 1 : lineOpacity);

        Double fillOpacity = parseDouble(PKEY_FILLOPACITY);
        figure.setFillOpacity((fillOpacity == null) ? 1 : fillOpacity);


        Double lineWidth = parseDouble(PKEY_LINEWIDTH);
        figure.setLineWidth((lineWidth == null) ? 1 : lineWidth.floatValue());

        LineStyle lineStyle = parseLineStyle(PKEY_LINESTYLE);
        figure.setLineStyle((lineStyle == null) ? LineStyle.LINE_SOLID : lineStyle);

        JoinStyle joinStyle = parseJoinStyle(PKEY_JOINSTYLE);
        figure.setJoinStyle((joinStyle == null) ? JoinStyle.JOIN_MITER : joinStyle);

        Boolean clipChildren = parseBoolean(PKEY_CLIPCHILDREN);
        figure.setClipChildren((clipChildren == null) ? false : clipChildren);
    }

    // Utility functions for parameter parsing.

    protected LineStyle parseLineStyle(String key) {
        if (getModel() == null) {
            return null;
        }

        String style = getModel().getValue(key);

        if (style == null) {
            return null;
        } else {
            if (style.equals("solid")) {
                return LineStyle.LINE_SOLID;
            } else if (style.equals("dashed")) {
                return LineStyle.LINE_DASHED;
            } else if (style.equals("dotted")) {
                return LineStyle.LINE_DOTTED;
            } else {
                return null;
            }
        }
    }

    protected JoinStyle parseJoinStyle(String key) {
        if (getModel() == null) {
            return null;
        }

        String style = getModel().getValue(key);

        if (style == null) {
            return null;
        } else {
            if (style.equals("bevel")) {
                return JoinStyle.JOIN_BEVEL;
            } else if (style.equals("miter")) {
                return JoinStyle.JOIN_MITER;
            } else if (style.equals("round")) {
                return JoinStyle.JOIN_ROUND;
            } else {
                return null;
            }
        }
    }

    protected FillRule parseFillRule(String key) {
        if (getModel() == null) {
            return null;
        }

        String rule = getModel().getValue(key);

        if (rule == null) {
            return null;
        } else {
            if (rule.equals("evenodd")) {
                return FillRule.FILL_EVENODD;
            } else if (rule.equals("nonzero")) {
                return FillRule.FILL_NONZERO;
            } else {
                return null;
            }
        }
    }
}
