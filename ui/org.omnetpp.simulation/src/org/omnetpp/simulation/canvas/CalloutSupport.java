package org.omnetpp.simulation.canvas;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.AncestorListener;
import org.eclipse.draw2d.FigureListener;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Polygon;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PointList;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.figures.misc.ISelectionHandleBounds;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.inspectors.IInspectorPart;
import org.omnetpp.simulation.model.cObject;


/**
 *
 * @author Andras
 */
//FIXME after closing an inspector, callout from its child is NOT redirected to the grandparent! (because there's no selction change!!!)
public class CalloutSupport {
    private SimulationCanvas simulationCanvas;
    private Color backgroundColor = ColorFactory.LIGHT_BLUE4;
    private Color foregroundColor = ColorFactory.LIGHT_BLUE;
    private int alpha = 100;

    private class CalloutInfo {
        IInspectorPart sourceInspector;
        IInspectorPart targetInspector;
        IFigure sourceFigure; // base of the triangle (main figure of sourceInspector)
        IFigure targetFigure; // tip of the triangle (subfigure or main figure of targetInspector)
        Polygon calloutFigure;
        FigureListener figureListener;
        AncestorListener ancestorListener;
    }

    private List<CalloutInfo> calloutInfoList = new ArrayList<CalloutSupport.CalloutInfo>();

    /**
     * Constructor
     */
    public CalloutSupport(SimulationCanvas canvas) {
        this.simulationCanvas = canvas;
    }

    public SimulationCanvas getSimulationCanvas() {
        return simulationCanvas;
    }

    public Color getBackgroundColor() {
        return backgroundColor;
    }

    public void setBackgroundColor(Color backgroundColor) {
        this.backgroundColor = backgroundColor;
        updateCallouts();
    }

    public Color getForegroundColor() {
        return foregroundColor;
    }

    public void setForegroundColor(Color foregroundColor) {
        this.foregroundColor = foregroundColor;
        updateCallouts();
    }

    public int getAlpha() {
        return alpha;
    }

    public void setAlpha(int alpha) {
        this.alpha = alpha;
        updateCallouts();
    }

    protected void updateCallouts() {
        for (CalloutInfo info : calloutInfoList) {
            info.calloutFigure.setForegroundColor(foregroundColor);
            info.calloutFigure.setBackgroundColor(backgroundColor);
            info.calloutFigure.setAlpha(alpha);
        }
    }

    public void removeAllCallouts() {
        for (CalloutInfo info : calloutInfoList.toArray(new CalloutInfo[]{}))
            removeCallout(info);
    }

    public void removeCallout(IInspectorPart sourceInspector) {
        removeCallout(sourceInspector, null);
    }

    public void removeCallout(IInspectorPart sourceInspector, IInspectorPart targetInspector) {
        for (CalloutInfo info : calloutInfoList.toArray(new CalloutInfo[]{}))
            if (info.sourceInspector == sourceInspector && (targetInspector == null || info.targetInspector == targetInspector))
                removeCallout(info);
    }

    protected void removeCallout(CalloutInfo info) {
        info.sourceFigure.removeFigureListener(info.figureListener);
        info.sourceFigure.removeAncestorListener(info.ancestorListener);
        info.targetFigure.removeFigureListener(info.figureListener);
        info.targetFigure.removeAncestorListener(info.ancestorListener);
        IFigure parent = info.calloutFigure.getParent();
        if (parent != null)
            parent.remove(info.calloutFigure);
        calloutInfoList.remove(info);
    }

    public void addCalloutFor(IInspectorPart sourceInspector, boolean allTheWayUp) throws CommunicationException {
        // find an inspector that represents the nearest ancestor (i.e. container object) for sourceInspector's object
        cObject object = sourceInspector.getObject();
        object.loadIfUnfilled();
        cObject firstInspectedAncestor = null;
        for (cObject o = object.getOwner(); o != null; o = o.getOwner()) {
            o.loadIfUnfilled();
            if (simulationCanvas.findInspectorFor(o) != null) {
                firstInspectedAncestor = o;
                break;
            }
        }

        // and add a callout to point there
        if (firstInspectedAncestor != null) {
            IInspectorPart targetInspector = simulationCanvas.findInspectorFor(firstInspectedAncestor);
            addCallout(sourceInspector, targetInspector);
            if (allTheWayUp)
                addCalloutFor(targetInspector, true);
        }
    }

    public void addCallout(IInspectorPart sourceInspector, IInspectorPart targetInspector) {
        IFigure sourceFigure = sourceInspector.getFigure();
        IFigure targetFigure = targetInspector.findFigureContaining(sourceInspector.getObject());

        Polygon calloutFigure = new Polygon() {
            @Override
            public boolean containsPoint(int x, int y) {
                return false; // let mouse clicks through
            }
        };
        PointList points = computeCalloutCoordinates(sourceFigure, targetFigure);
        calloutFigure.setPoints(points);
        calloutFigure.setFill(true);
        calloutFigure.setBackgroundColor(backgroundColor);
        calloutFigure.setForegroundColor(foregroundColor);
        calloutFigure.setAlpha(alpha);

        // add the callout to the inspectors layer. Note: it cannot be placed on a separate foreground layer,
        // because it would look wrong when sourceFigure is obscured by another (overlapping) inspector,
        // resulting in an seemingly "orphaned" (detached) callout polygon
        simulationCanvas.getInspectorsLayer().add(calloutFigure);

        // manage Z-order. There are some rules that we need to follow, otherwise
        // the result can be awkward:
        //  1. the callout must be ABOVE the target inspector figure
        //  2. if another inspector overlaps with sourceFigure, it MUST NOT be between
        //     the sourceFigure and the callout figure in Z-order
        //FIXME TODO

        // create info
        final CalloutInfo info = new CalloutInfo();
        info.calloutFigure = calloutFigure;
        info.sourceInspector = sourceInspector;
        info.targetInspector = targetInspector;
        info.sourceFigure = sourceFigure;
        info.targetFigure = targetFigure;
        // note: listeners to be filled in

        // update polygon when source or target figure moves
        FigureListener figureListener = new FigureListener() {
            @Override
            public void figureMoved(IFigure source) {
                PointList points = computeCalloutCoordinates(info.sourceFigure, info.targetFigure);
                info.calloutFigure.setPoints(points);
            }
        };
        sourceFigure.addFigureListener(figureListener);
        targetFigure.addFigureListener(figureListener);

        // remove callout when source or target figure is taken out of the figure hierarchy
        // (likely cause: inspector closed or submodule deleted)
        AncestorListener ancestorListener = new AncestorListener.Stub() {
            @Override
            public void ancestorRemoved(IFigure ancestor) {
                Display.getCurrent().asyncExec(new Runnable() {
                    @Override
                    public void run() {
                        removeCallout(info);
                    }
                });
            }
        };
        sourceFigure.addAncestorListener(ancestorListener);
        targetFigure.addAncestorListener(ancestorListener);

        // finish filling in the info
        info.figureListener = figureListener;
        info.ancestorListener = ancestorListener;
        calloutInfoList.add(info);
    }

    protected PointList computeCalloutCoordinates(IFigure sourceFigure, IFigure targetFigure) {
        Rectangle sourceRect = sourceFigure.getBounds();
        Point targetPoint = targetFigure instanceof ISelectionHandleBounds ?
                ((ISelectionHandleBounds)targetFigure).getHandleBounds().getCenter() :
                    targetFigure.getBounds().getCenter();
        targetFigure.getParent().translateToAbsolute(targetPoint);
        targetPoint = simulationCanvas.translateCanvasToAbsoluteFigureCoordinates(targetPoint.x, targetPoint.y);

        // where is the point compared to the rectangle
        boolean isLeft = targetPoint.x < sourceRect.x;
        boolean isRight = targetPoint.x > sourceRect.right();
        boolean isAbove = targetPoint.y < sourceRect.y;
        boolean isBelow = targetPoint.y > sourceRect.bottom();

        PointList points = new PointList();
        if (!isRight)
            points.addPoint(targetPoint);
        if (isAbove || isRight)
            points.addPoint(sourceRect.getTopRight());
        if (isAbove || isLeft)
            points.addPoint(sourceRect.getTopLeft());
        if (isRight)
            points.addPoint(targetPoint);
        if (isBelow || isLeft)
            points.addPoint(sourceRect.getBottomLeft());
        if (isBelow || isRight)
            points.addPoint(sourceRect.getBottomRight());
        return points;
    }
}
