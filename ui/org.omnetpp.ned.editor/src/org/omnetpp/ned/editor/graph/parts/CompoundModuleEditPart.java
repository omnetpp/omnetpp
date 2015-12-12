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
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.figures.anchors.CompoundModuleGateAnchor;
import org.omnetpp.figures.anchors.GateAnchor;
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
 * @author rhornig
 */
public class CompoundModuleEditPart extends ModuleEditPart {
    private float scale = 1.0f;
    private float iconScale = 1.0f;

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
        compoundModuleFigure.setDisplayString(compoundModuleModel.getDisplayString(), project, scale, iconScale);
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

    @Override
    public float getIconScale() {
        return iconScale;
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
