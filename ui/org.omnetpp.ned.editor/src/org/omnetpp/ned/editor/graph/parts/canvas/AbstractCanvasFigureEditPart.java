package org.omnetpp.ned.editor.graph.parts.canvas;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionDimension;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.eclipse.gef.requests.ChangeBoundsRequest;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.figures.CompoundModuleFigure.FigureLayer;
import org.omnetpp.figures.canvas.AbstractCanvasFigure;
import org.omnetpp.figures.misc.AnchoredRectangle;
import org.omnetpp.figures.misc.Transform;
import org.omnetpp.figures.misc.AnchoredRectangle.Anchor;
import org.omnetpp.ned.editor.graph.commands.MoveCanvasFigureCommand;
import org.omnetpp.ned.editor.graph.commands.ResizeCanvasFigureCommand;
import org.omnetpp.ned.editor.graph.misc.CanvasFigureUtils;
import org.omnetpp.ned.editor.graph.parts.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.parts.IReadOnlySupport;
import org.omnetpp.ned.editor.graph.parts.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.parts.policies.NedResizeEditPolicy;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.interfaces.INedModelProvider;

import static org.omnetpp.common.canvas.CanvasFigureConstants.*;

public abstract class AbstractCanvasFigureEditPart extends AbstractGraphicalEditPart implements INedModelProvider, IReadOnlySupport {

    private boolean editable = true;

    /**
     * Should return true if the figure type in the parameter is represented by this class.
     * For example, called with "text", only the TextEditPart instances should return true.
     * The parameter must not be null.
     */
    protected abstract boolean representsType(String type);

    @Override
    public PropertyElementEx getModel() {
        PropertyElementEx model = (PropertyElementEx)super.getModel();

        if (model == null) {
            return null;
        }

        String type = model.getValue(PKEY_TYPE);

        // must return null if the PropertyElementEx model no longer has the
        // appropriate figure type value, this way GEF will recreate the
        // EditPart from the Factory, now with the correct class
        return ((type != null) && representsType(type)) ? model : null;
    }

    public AbstractCanvasFigure getFigure() {
        return (AbstractCanvasFigure)super.getFigure();
    }

    public CompoundModuleEditPart getCompoundModulePart() {
        EditPart parent = getParent();
        if (parent instanceof CompoundModuleEditPart) {
            return (CompoundModuleEditPart)parent;
        } else if (parent instanceof AbstractCanvasFigureEditPart) {
            return ((AbstractCanvasFigureEditPart)parent).getCompoundModulePart();
        }
        // this should never happen
        return null;
    }

    public double getScale() {
        return getCompoundModulePart().getScale();
    }

    /**
     * Whether this figure type supports resizing (has a bounds/size property).
     * Subclasses that represent shapes with a defining rectangle should override
     * this to return true.
     */
    public boolean isResizable() {
        return false;
    }

    public Dimension getSize() {
        AnchoredRectangle rect = parseAnchoredRectangle(PKEY_BOUNDS, PKEY_POS, PKEY_SIZE, PKEY_ANCHOR);
        return (rect == null) ? new PrecisionDimension() : rect.getSize();
    }

    public void setSize(Dimension size) {
        AnchoredRectangle rect = parseAnchoredRectangle(PKEY_BOUNDS, PKEY_POS, PKEY_SIZE, PKEY_ANCHOR);
        if (rect == null)
            rect = new AnchoredRectangle();
        rect.setSize(size);
        setAnchoredRectangle(PKEY_BOUNDS, PKEY_POS, PKEY_SIZE, PKEY_ANCHOR, rect);
    }

    protected Command getTransformedMoveCommand(Request request, Transform t) {
        if (!t.isInvertible())
            return UnexecutableCommand.INSTANCE; // the figure is collapsed into a line or a point
        if (request instanceof ChangeBoundsRequest) {
            ChangeBoundsRequest boundsRequest = (ChangeBoundsRequest)request;

            Point originalLocation = getLocation();
            Point transformedLocation = t.applyTo(originalLocation);
            Point delta = boundsRequest.getMoveDelta();
            transformedLocation.translate(delta);
            Point location = t.getInverse().applyTo(transformedLocation);

            IFigure figureLayer = getFigure().getParent();
            while (!(figureLayer instanceof FigureLayer)) {
                figureLayer = figureLayer.getParent();
            }

            Rectangle layerArea = figureLayer.getClientArea();
            figureLayer.translateFromParent(layerArea);
            figureLayer.translateToAbsolute(layerArea);

            Point requestLocation = boundsRequest.getLocation();
            if (requestLocation != null && !layerArea.contains(requestLocation))
                return UnexecutableCommand.INSTANCE;

            double scale = getScale();
            PrecisionPoint roundedLocation = new PrecisionPoint(location);
            CanvasFigureUtils.roundForZoom(roundedLocation, scale);
            Command c;
            Dimension sizeDelta = boundsRequest.getSizeDelta();
            if (isResizable() && sizeDelta != null && (sizeDelta.width != 0 || sizeDelta.height != 0)) {
                Dimension originalSize = getSize();
                PrecisionDimension newSize = new PrecisionDimension(
                        originalSize.preciseWidth() + sizeDelta.preciseWidth() / scale,
                        originalSize.preciseHeight() + sizeDelta.preciseHeight() / scale);
                if (newSize.preciseWidth() < 1)
                    newSize.setPreciseWidth(1);
                if (newSize.preciseHeight() < 1)
                    newSize.setPreciseHeight(1);
                CanvasFigureUtils.roundForZoom(newSize, scale);
                c = new ResizeCanvasFigureCommand(this, roundedLocation, newSize);
            } else {
                c = new MoveCanvasFigureCommand(this, new PrecisionPoint(roundedLocation));
            }

            boundsRequest.setConstrainedMove(false);
            return c;
        } else {
            return UnexecutableCommand.INSTANCE;
        }
    }

    public void setEditable(boolean editable) {
        this.editable = editable;
    }

    public boolean isEditable() {
        if (!editable)
            return false;
        // editable only if the figure is defined in this compound module (not inherited)
        if (getModel() != null && getCompoundModulePart() != null)
            return getModel().getSelfOrEnclosingTypeElement() == getCompoundModulePart().getModel();
        return true;
    }

    @Override
    public Command getCommand(Request request) {
        if (!isEditable())
            return UnexecutableCommand.INSTANCE;
        return request.getType().equals("delete")
                ? super.getCommand(request)
                : getTransformedMoveCommand(request, getFigure().getCascadedTransform());
    }

    public Point getLocation() {
        AnchoredRectangle rect = parseAnchoredRectangle(PKEY_BOUNDS, PKEY_POS, PKEY_SIZE, PKEY_ANCHOR);
        return (rect == null) ? new PrecisionPoint() : rect.getLocation();
    }

    public void setLocation(Point loc) {
        AnchoredRectangle rect = parseAnchoredRectangle(PKEY_BOUNDS, PKEY_POS, PKEY_SIZE, PKEY_ANCHOR);
        rect.setLocation(loc);
        setAnchoredRectangle(PKEY_BOUNDS, PKEY_POS, PKEY_SIZE, PKEY_ANCHOR, rect);
    }

    @Override
    protected List<PropertyElementEx> getModelChildren() {
        List<PropertyElementEx> children = new ArrayList<PropertyElementEx>();

        if ((getModel() == null)
                || (getModel().getParent() == null)
                || (getModel().getParent().getParent() == null)) {
            return children;
        }

        // All the figure properties of the CompoundModule which contains this figure.
        Map<String, PropertyElementEx> figures = ((CompoundModuleElementEx)getModel().getParent().getParent()).getProperties().get("figure");

        if (figures == null) {
            return children;
        }

        for (PropertyElementEx figure : figures.values()) {
            String type = figure.getValue(PKEY_TYPE);

            if ((type != null) &&
                    CanvasFigureUtils.getClosestAncestor(figure) == getModel()) {
                children.add(figure);
            }
        }

        return children;
    }

    @Override
    @SuppressWarnings("unchecked")
    public void refresh() {
        for (AbstractGraphicalEditPart child : (List<AbstractGraphicalEditPart>)getChildren()) {
            child.refresh();
        }
        super.refresh();
    }

    @Override
    protected void refreshVisuals() {
        super.refreshVisuals();

        if (getModel() == null) {
            return;
        }

        AbstractCanvasFigure figure = getFigure();

        Map<String, PropertyElementEx> figures = getCompoundModulePart().getModel().getProperties().get("figure");
        figure.setOrdinal(figures != null ? CanvasFigureUtils.getOrdinal(figures, getModel().getIndex()) : 0);

        figure.setScale(getScale());
        figure.setTransform(parseTransform(PKEY_TRANSFORM));

        Double zIndex = parseDouble(PKEY_ZINDEX);
        figure.setZIndex((zIndex == null) ? 0 : zIndex);

        Boolean visible = parseBoolean(PKEY_VISIBLE);
        figure.setCanvasFigureVisible((visible == null) ? true : visible);
    }

    @Override
    protected void createEditPolicies() {
       NedResizeEditPolicy policy = new NedResizeEditPolicy();
       policy.setResizeDirections(PositionConstants.NONE); // To allow selection, but disallow resizing.
       installEditPolicy(EditPolicy.SELECTION_FEEDBACK_ROLE, policy);
       installEditPolicy(EditPolicy.COMPONENT_ROLE, new NedComponentEditPolicy());
    }

    // Utility function relays for easier parameter parsing.

    protected Boolean parseBoolean(String key) {
        if (getModel() == null) {
            return null;
        }

        String value = getModel().getValue(key);
        return (value == null) ? null : Boolean.parseBoolean(value);
    }

    protected Double parseDouble(String key) {
        return parseDouble(key, 0);
    }

    protected Double parseDouble(String key, int index) {
        if (getModel() == null) {
            return null;
        }

        List<String> values = getModel().getValueAsList(key);
        String value = (values != null && values.size() > index) ? values.get(index) : null;
        try {
            return (value == null) ? null : Double.parseDouble(value);
        } catch (NumberFormatException e) {
            return null;
        }
    }

    protected String parseString(String key) {
        return ((getModel() == null) ? null : getModel().getValue(key));
    }

    protected List<Point> parsePointList(String key) {
        return CanvasFigureUtils.parsePointList(getModel(), key);
    }

    protected void setPointList(String key, List<Point> pointList) {
        CanvasFigureUtils.setPointList(getModel(), key, pointList);
    }

    protected PrecisionPoint parsePoint(String key) {
        return CanvasFigureUtils.parsePoint(getModel(), key);
    }

    protected void setPoint(String key, Point point) {
        CanvasFigureUtils.setPoint(getModel(), key, point);
    }

    protected PrecisionDimension parseDimension(String key) {
        return CanvasFigureUtils.parseDimension(getModel(), key);
    }

    protected Anchor parseAnchor(String key) {
        return CanvasFigureUtils.parseAnchor(getModel(), key);
    }

    protected AnchoredRectangle parseAnchoredRectangle(String boundsKey, String coordsKey, String sizeKey, String anchorKey) {
        return CanvasFigureUtils.parseAnchoredRectangle(getModel(), boundsKey, coordsKey, sizeKey, anchorKey);
    }

    protected void setAnchoredRectangle(String boundsKey, String coordsKey, String sizeKey, String anchorKey, AnchoredRectangle rect) {
        CanvasFigureUtils.setAnchoredRectangle(getModel(), boundsKey, coordsKey, sizeKey, anchorKey, rect);
    }

    protected Color parseColor(String key) {
        return ((getModel() == null) ? null : ColorFactory.asColor(getModel().getValue(key)));
    }

    protected Transform parseTransform(String key) {
        return CanvasFigureUtils.parseTransform(getModel(), key);
    }
}
