package org.omnetpp.ned.editor.graph.edit.outline;

import java.util.Arrays;
import java.util.List;

import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.RootEditPart;
import org.eclipse.gef.editparts.AbstractTreeEditPart;
import org.eclipse.gef.editpolicies.RootComponentEditPolicy;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedTreeContainerEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedTreeEditPolicy;
import org.omnetpp.ned.editor.graph.properties.IPropertySourceSupport;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDModelEvent;

/**
 * EditPart for Logic components in the Tree.
 */
public class NedTreeEditPart extends AbstractTreeEditPart implements
        INEDChangeListener, IPropertySourceSupport, IModelProvider {

    private long lastEventSerial;
    private IPropertySource propertySource;
    

    /**
     * Constructor initializes this with the given model.
     * 
     * @param model
     *            Model for this.
     */
    public NedTreeEditPart(Object model) {
        super(model);
    }

    @Override
    public void activate() {
        super.activate();
        if (getModel() != null)
            ((NEDElement)getModel()).getListeners().add(this);
    }

    /**
     * Creates and installs pertinent EditPolicies for this.
     */
    @Override
    protected void createEditPolicies() {
        // install root policy to disable deleting of the root node
        if (getParent() instanceof RootEditPart)
            installEditPolicy(EditPolicy.COMPONENT_ROLE, new RootComponentEditPolicy());

    	installEditPolicy(EditPolicy.PRIMARY_DRAG_ROLE, new NedTreeEditPolicy());
    	installEditPolicy(EditPolicy.TREE_CONTAINER_ROLE, new NedTreeContainerEditPolicy());

        // we do not allow the reordering of the content (children) of the following node types 
    	if (getModel() instanceof SubmoduleNodeEx || 
        	getModel() instanceof CompoundModuleNodeEx ) {
        	removeEditPolicy(EditPolicy.TREE_CONTAINER_ROLE);
        }
    	
        // delete support
        installEditPolicy(EditPolicy.COMPONENT_ROLE, new NedComponentEditPolicy());
        // reorder support
    }

    @Override
    public void deactivate() {
        if (getModel() != null)
            ((NEDElement)getModel()).getListeners().remove(this);
        super.deactivate();
    }

    /**
     * Returns the children of this from the model element
     * 
     * @return List of children.
     */
    @Override
    protected List getModelChildren() {
        return Arrays.asList(NEDTreeUtil.getNedModelContentProvider().getChildren(getModel()));
    }

    /**
	 * Refreshes the visual properties of the TreeItem for this part.
     */
    @Override
    protected void refreshVisuals() {
        if (getWidget() instanceof Tree) return;
    	setWidgetImage(NEDTreeUtil.getNedModelLabelProvider().getImage(getModel()));
    	setWidgetText(NEDTreeUtil.getNedModelLabelProvider().getText(getModel()));
    }

    public void modelChanged(NEDModelEvent event) {
        // skip the event processing if te last serial is greater or equal. only newer
        // events should be processed. this prevent the processing of the same event multiple times
        if (lastEventSerial >= event.getSerial())
            return;
        else // process the even and remeber this serial
            lastEventSerial = event.getSerial();

        refreshChildren();
    }

    public IPropertySource getPropertySource() {
        return propertySource;
    }

    public void setPropertySource(IPropertySource propertySource) {
        this.propertySource = propertySource;
    }
}