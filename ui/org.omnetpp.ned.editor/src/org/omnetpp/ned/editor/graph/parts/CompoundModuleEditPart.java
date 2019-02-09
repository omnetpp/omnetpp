/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IProject;
import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.CompoundSnapToHelper;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.MouseWheelHelper;
import org.eclipse.gef.SnapToGeometry;
import org.eclipse.gef.SnapToHelper;
import org.eclipse.gef.editparts.ViewportMouseWheelHelper;
import org.eclipse.gef.editpolicies.SnapFeedbackPolicy;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.anchors.CompoundModuleGateAnchor;
import org.omnetpp.figures.anchors.GateAnchor;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.omnetpp.ned.editor.graph.figures.CompoundModuleTypeFigure;
import org.omnetpp.ned.editor.graph.parts.policies.CompoundModuleLayoutEditPolicy;
import org.omnetpp.ned.editor.graph.properties.util.TypeNameValidator;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.interfaces.IInterfaceTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * Edit part controlling the appearance of the compound module figure. Note that this
 * editpart handles its own connection part registry and does not use the global registry
 *
 * @author rhornig, andras
 */
public class CompoundModuleEditPart extends ModuleEditPart {
    private static final String PREF_SECTION = "ViewerSettings";
    private static final String PREF_SCALE = ":scale";
    private static final String PREF_ICONSCALE = ":iconScale";
    private float scale = Float.NaN;
    private float zoomByFactor = 1.3f;
    private float iconScale = Float.NaN;

    // stores  the connection model - connection editPart mapping for the compound module
    private final Map<Object, ConnectionEditPart> modelToConnectionPartsRegistry = new HashMap<Object, ConnectionEditPart>();

    @Override
    public void activate() {
        super.activate();
        renameValidator = new TypeNameValidator(getModel());
    }

    @Override
    protected void createEditPolicies() {
        super.createEditPolicies();
        installEditPolicy(EditPolicy.LAYOUT_ROLE, new CompoundModuleLayoutEditPolicy());
        installEditPolicy("Snap Feedback", new SnapFeedbackPolicy());
    }

    /**
     * Creates and returns a new module figure
     */
    @Override
    protected IFigure createFigure() {
        return new CompoundModuleTypeFigure();
    }

    protected IDialogSettings getSettings() {
        IDialogSettings settings = UIUtils.getDialogSettings(NedEditorPlugin.getDefault(), PREF_SECTION);
        return settings;
    }

    /**
     * Convenience method to return the figure object with the correct type
     */
    public CompoundModuleTypeFigure getFigure() {
        return (CompoundModuleTypeFigure)super.getFigure();
    }

    /**
     * Convenience method to return the model object with the correct type
     */
    public CompoundModuleElementEx getModel() {
        return (CompoundModuleElementEx)super.getModel();
    }

    @Override
    public IFigure getContentPane() {
        return getFigure().getSubmoduleArea().getSubmoduleLayer();
    }

    // overridden so submodules are added to the contentPane while TypesEditPart is never added to the contentPane
    // TypesEdit part is always associated with the innerTypesCompartment of the CompoundModuleFigure
    // and should be added directly to the figure (instead of the contentPane)
    @Override
    protected void addChildVisual(EditPart childEditPart, int index) {
        IFigure childFigure = ((GraphicalEditPart)childEditPart).getFigure();
        // submodule figures should be added to the main pane
        if (childEditPart instanceof SubmoduleEditPart)
            getContentPane().add(childFigure);
        // the inner type compartment should be added as the second child (between title and submodules)
        else if (childEditPart instanceof TypesEditPart)
            getFigure().add(childFigure,1); // add it as a second child to be displayed between the title and the submodulesarea
    }

    // overridden so child figures are always removed from their direct parent
    // because both submoduleFigures and TypesFigures can be removed.
    // Submodule figures are removed from the contentPane (SubmoduleLayer)
    // while TypesFigure removed directly from the child list
    @Override
    protected void removeChildVisual(EditPart childEditPart) {
        IFigure childFigure = ((GraphicalEditPart)childEditPart).getFigure();
        if (childFigure.getParent() != null)
            childFigure.getParent().remove(childFigure);
    }

    @Override
    @SuppressWarnings("rawtypes")
    public Object getAdapter(Class key) {
        if (key == MouseWheelHelper.class) return new ViewportMouseWheelHelper(this);
        // snap to grid/guide adaptor
        if (key == SnapToHelper.class) {
            List<SnapToGeometry> snapStrategies = new ArrayList<SnapToGeometry>();
            Boolean val = (Boolean) getViewer().getProperty(SnapToGeometry.PROPERTY_SNAP_ENABLED);
            if (val) snapStrategies.add(new SnapToGeometry(this));

            if (snapStrategies.size() == 0) return null;
            if (snapStrategies.size() == 1) return snapStrategies.get(0);

            SnapToHelper ss[] = new SnapToHelper[snapStrategies.size()];
            for (int i = 0; i < snapStrategies.size(); i++)
                ss[i] = snapStrategies.get(i);
            return new CompoundSnapToHelper(ss);
        }

        return super.getAdapter(key);
    }

    @Override
    protected List<INedElement> getModelChildren() {
        List<INedElement> result = new ArrayList<INedElement>();
        // add the innerTypes element (if exists)
        TypesElement typesElement = getModel().getFirstTypesChild();
        if (typesElement != null)
            result.add(typesElement);

        // return all submodule including inherited ones
        result.addAll(getModel().getSubmodules());
        return result;
    }

    /**
     * Returns a list of connections for which this is the srcModule.
     */
    @Override
    protected List<ConnectionElementEx> getModelSourceConnections() {
        return getModel().getSrcConnections();
    }

    /**
     * Returns a list of connections for which this is the destModule.
     */
    @Override
    protected List<ConnectionElementEx> getModelTargetConnections() {
        return getModel().getDestConnections();
    }

    /**
     * Updates the visual aspect of this compound module
     */
    @Override
    protected void refreshVisuals() {
        super.refreshVisuals();

        // define the properties that determine the visual appearance
        CompoundModuleTypeFigure compoundModuleFigure = getFigure();
        CompoundModuleElementEx compoundModuleModel = getModel();
        IProject project = compoundModuleModel.getNedTypeInfo().getProject();
        compoundModuleFigure.setName(compoundModuleModel.getName());
        compoundModuleFigure.setNetwork(compoundModuleModel.isNetwork());
        compoundModuleFigure.setInterface(compoundModuleModel instanceof IInterfaceTypeElement);
        compoundModuleFigure.setInnerType(compoundModuleModel.getEnclosingTypeElement() != null);

        if (Float.isNaN(scale)) {
            // determine initial scale before refreshing submodule area
            compoundModuleFigure.setDisplayString(compoundModuleModel.getDisplayString(), project, 1.0f, 1.0f);
            Dimension unscaledSize = compoundModuleFigure.getSubmoduleArea().getPreferredSize();
            scale = getInitialScale(unscaledSize);
        }

        if (Float.isNaN(iconScale))
            iconScale = getInitialIconScale();

        compoundModuleFigure.setDisplayString(compoundModuleModel.getDisplayString(), project, scale, iconScale);
    }

    protected float getInitialScale(Dimension unscaledSize) {
        try {
            return getSettings().getFloat(getModel().getNedTypeInfo().getFullyQualifiedName()+PREF_SCALE);
        } catch (NumberFormatException e) {} // not found

        // No saved preference; compute a default scale.
        // Note: we use constants for maximum sizes because editor area dimensions
        // are probably not available here yet (getViewer().getControl().getSize()).
        final int MAX_WIDTH = 800;
        final int MAX_HEIGHT = 500;
        final int MIN_WIDTH = 300;
        final int MIN_HEIGHT = 200;

        // zoomOutScale: max zoom so that module still fits into the window (MAX_WIDTH, MAX_HEIGHT)
        float zoomOutScale = Math.min(MAX_WIDTH / (float)unscaledSize.width, MAX_HEIGHT / (float)unscaledSize.height);

        // zoomInScale: minimum zoom so that both dimensions of the module are larger than some minimum size
        float zoomInScale = Math.max(MIN_WIDTH / (float)unscaledSize.width, MIN_HEIGHT / (float)unscaledSize.height);

        // try using 1.0, if it is in the acceptable interval, otherwise the one closest to 1.0
        if (zoomOutScale < zoomInScale)
            return zoomOutScale;  // conflicting requirements: zoomOutScale wins (we want the module to fit into the window)
        else if (zoomInScale <= 1.0f && zoomOutScale >= 1.0f)
            return 1.0f;
        else if (zoomOutScale <= 1.0f)
            return zoomOutScale;
        else
            return zoomInScale;
    }

    protected float getInitialIconScale() {
        try {
            return getSettings().getFloat(getModel().getNedTypeInfo().getFullyQualifiedName()+PREF_ICONSCALE);
        }
        catch (NumberFormatException e) { // not found
            return 1.0f;
        }
    }

    /**
     * Returns whether the compound module is selectable (mouse is over the bordering area)
     * for the selection tool based on the current mouse target coordinates.
     * Coordinates are viewport relative.
     */
    public boolean isOnBorder(int x, int y) {
        return getFigure().isOnBorder(x, y);
    }

    /**
     * Compute the source connection anchor to be assigned based on the current mouse
     * location and available gates.
     * @param p current mouse coordinates
     * @return The selected connection anchor
     */
    @Override
    public ConnectionAnchor getConnectionAnchorAt(Point p) {
        return new CompoundModuleGateAnchor(getFigure().getSubmoduleArea());
    }

    /**
     * Returns a connection anchor registered for the given gate
     */
    @Override
    public GateAnchor getConnectionAnchor(ConnectionElementEx connection, String gate) {
        CompoundModuleGateAnchor gateAnchor = new CompoundModuleGateAnchor(getFigure().getSubmoduleArea());
        gateAnchor.setEdgeConstraint(getRoutingConstraintPosition(connection.getDisplayString().getAsString(Prop.ROUTING_CONSTRAINT)));
        return gateAnchor;
    }

    private int getRoutingConstraintPosition(String routingConstraint) {
        int position;
        if (routingConstraint == null || routingConstraint.equals("a"))
            position = PositionConstants.NSEW;
        else if (routingConstraint.equals("n"))
            position = PositionConstants.NORTH;
        else if (routingConstraint.equals("s"))
            position = PositionConstants.SOUTH;
        else if (routingConstraint.equals("e"))
            position = PositionConstants.EAST;
        else if (routingConstraint.equals("w"))
            position = PositionConstants.WEST;
        else
            // default;
            position = PositionConstants.NSEW;
        return position;
    }

    /**
     * Returns the current scaling factor of the compound module
     */
    @Override
    public float getScale() {
        return scale;
    }

    public void setScale(float scale) {
        this.scale = scale;
        getSettings().put(getModel().getNedTypeInfo().getFullyQualifiedName()+PREF_SCALE, scale);
    }

    public void zoomIn() {
        if (canZoomIn())
            setScale(snapScaleTo1(getScale() * zoomByFactor));
    }

    public void zoomOut() {
        if (canZoomOut())
            setScale(snapScaleTo1(getScale() / zoomByFactor));
    }

    private float snapScaleTo1(float scale) {
        // snap to 1 if it's close to 1.0
        return (scale > 1.0f/zoomByFactor && scale < zoomByFactor) ? 1.0f : scale;
    }

    public boolean canZoomIn() {
        // safety measure: prevent scale overflowing into infinity in any case
        if (scale > 1e10f)
            return false;

        Dimension currentSize = getFigure().getSubmoduleArea().getSize();
        if (currentSize.width == 0 && currentSize.height == 0)  // not layouted yet
            return true;

        // Limit zooming in, because at large sizes integer overflows kick in
        // in the drawing library and produce weird artifacts
        final int MAXSIZE = 15000;
        return currentSize.width*zoomByFactor <= MAXSIZE && currentSize.height*zoomByFactor <= MAXSIZE;
    }

    public boolean canZoomOut() {
        // safety measure: prevent scale underflowing into 0 in any case
        if (scale < 1e-10f)
            return false;

        Dimension currentSize = getFigure().getSubmoduleArea().getSize();
        if (currentSize.width == 0 && currentSize.height == 0) // not layouted yet
            return true;

        // In practice, figure will often refuse to shrink below a certain size (say ~50 pixels)
        // due to contained icons and labels, regardless of scale being arbitrarily small.
        // Put a reasonable lower limit to scale to prevent it from getting unnecessarily
        // (and ineffectively) very small.
        if (scale/zoomByFactor < 1e-3 && currentSize.width < 800 && currentSize.height < 600)
            return false;

        // Makes no sense to have both dimensions smaller than this (if it could at all, see above)
        final int MINSIZE = 100;
        return currentSize.width/zoomByFactor >= MINSIZE || currentSize.height/zoomByFactor >= MINSIZE;
    }

    public float getIconScale() {
        return iconScale;
    }

    public void setIconScale(float iconScale) {
        if (Math.abs(iconScale-1.0f) < 0.05f)  // snap to 1
            iconScale = 1.0f;
        this.iconScale = iconScale;
        getSettings().put(getModel().getNedTypeInfo().getFullyQualifiedName()+PREF_ICONSCALE, iconScale);
    }

    public void changeLayout() {
        CompoundModuleFigure figure = getFigure().getSubmoduleArea();
        figure.changeLayout();
    }

    @Override
    public CompoundModuleEditPart getCompoundModulePart() {
        return this;
    }

    /**
     * Returns the MAP that contains the connection model - controller associations
     */
    public Map<Object, ConnectionEditPart> getModelToConnectionPartsRegistry() {
        return modelToConnectionPartsRegistry;
    }

    /* (non-Javadoc)
     * @see org.omnetpp.ned.editor.graph.edit.NedEditPart#getTypeNameForDblClickOpen()
     * open the first base component for double click
     */
    @Override
    public INedTypeElement getNedTypeElementToOpen() {
        return getModel().getSuperType();
    }
}
