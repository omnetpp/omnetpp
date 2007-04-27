package org.omnetpp.ned.editor.graph.edit.outline;

import java.util.Arrays;
import java.util.List;

import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.RootEditPart;
import org.eclipse.gef.editparts.AbstractTreeEditPart;
import org.eclipse.gef.editpolicies.RootComponentEditPolicy;
import org.eclipse.gef.tools.DirectEditManager;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedTreeContainerEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedTreeEditPolicy;
import org.omnetpp.ned.editor.graph.properties.IPropertySourceSupport;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.ChannelInterfaceNodeEx;
import org.omnetpp.ned.model.ex.ChannelNodeEx;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.NedFileNode;

/**
 * EditPart for Logic components in the Tree.
 */
public class NedTreeEditPart extends AbstractTreeEditPart implements
        INEDChangeListener, IPropertySourceSupport, IModelProvider {

    private IPropertySource propertySource;
    protected DirectEditManager manager;


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
        if (getModel() instanceof NedFileNode)
            NEDResourcesPlugin.getNEDResources().getNEDModelChangeListenerList().add(this);
    }

    @Override
    public void deactivate() {
        if (getModel() instanceof NedFileNode)
            NEDResourcesPlugin.getNEDResources().getNEDModelChangeListenerList().remove(this);
        super.deactivate();
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
        	getModel() instanceof CompoundModuleNodeEx ||
            getModel() instanceof ChannelNodeEx ||
            getModel() instanceof ChannelInterfaceNodeEx ||
            getModel() instanceof ModuleInterfaceNodeEx) {
        	removeEditPolicy(EditPolicy.TREE_CONTAINER_ROLE);
        }

        // delete support
        installEditPolicy(EditPolicy.COMPONENT_ROLE, new NedComponentEditPolicy());
        // reorder support
    }

    public INEDElement getNEDModel() {
        return (INEDElement)getModel();
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
        totalRefresh();
    }

    /**
     * Fully refresh ourselves an all of our children (recursively) visually
     */
    protected void totalRefresh() {
        // refresh ourselves
        refresh();
        // delegate to all children and refresh all their appearence
        for(Object child : getChildren())
            ((NedTreeEditPart)child).totalRefresh();

    }

    public IPropertySource getPropertySource() {
        return propertySource;
    }

    public void setPropertySource(IPropertySource propertySource) {
        this.propertySource = propertySource;
    }
}