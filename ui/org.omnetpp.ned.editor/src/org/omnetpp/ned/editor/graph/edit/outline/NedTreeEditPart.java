package org.omnetpp.ned.editor.graph.edit.outline;

import java.util.Arrays;
import java.util.List;

import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.RootEditPart;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editparts.AbstractTreeEditPart;
import org.eclipse.gef.editpolicies.RootComponentEditPolicy;
import org.eclipse.gef.tools.DirectEditManager;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedTreeContainerEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedTreeEditPolicy;
import org.omnetpp.ned.editor.graph.properties.IPropertySourceSupport;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDBeginModelChangeEvent;
import org.omnetpp.ned.model.notification.NEDEndModelChangeEvent;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.NedFileElement;

/**
 * EditPart for the NED Outline view.
 *
 * @author rhornig
 */
public class NedTreeEditPart extends AbstractTreeEditPart implements
        INEDChangeListener, IPropertySourceSupport, IModelProvider {

    private IPropertySource propertySource;
    protected DirectEditManager manager;

    // open NEDBeginChangeEvent notifications
    private int nedBeginChangeCount = 0;

    /**
     * Constructor initializes this with the given model.
     */
    public NedTreeEditPart(Object model) {
        super(model);
    }

    @Override
    public void activate() {
        super.activate();
        if (getModel() instanceof NedFileElement)
            NEDResourcesPlugin.getNEDResources().addNEDModelChangeListener(this);
    }

    @Override
    public void deactivate() {
        if (getModel() instanceof NedFileElement)
            NEDResourcesPlugin.getNEDResources().removeNEDModelChangeListener(this);
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
    	if (getModel() instanceof SubmoduleElementEx ||
        	getModel() instanceof CompoundModuleElementEx ||
            getModel() instanceof ChannelElementEx ||
            getModel() instanceof ChannelInterfaceElementEx ||
            getModel() instanceof ModuleInterfaceElementEx) {
        	removeEditPolicy(EditPolicy.TREE_CONTAINER_ROLE);
        }

        // delete support
        installEditPolicy(EditPolicy.COMPONENT_ROLE, new NedComponentEditPolicy());
        // reorder support
    }

    public INEDElement getNEDModel() {
        return (INEDElement)getModel();
    }

    public boolean isEditable() {
    	NedFileElementEx nedFileElement = getNEDModel().getContainingNedFileElement();
       	return nedFileElement == null || (!nedFileElement.isReadOnly() && !nedFileElement.hasSyntaxError());
    }

	@Override
    public Command getCommand(Request request) {
    	return isEditable() ? super.getCommand(request) : UnexecutableCommand.INSTANCE;
    }

    /**
     * Returns the children of the model element of this editpart.
     */
    @Override
    protected List<Object> getModelChildren() {
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
        // count begin/end nesting
        if (event instanceof NEDBeginModelChangeEvent)
            nedBeginChangeCount++;
        else if (event instanceof NEDEndModelChangeEvent)
            nedBeginChangeCount--;

        // we do a full refresh in response of a change
        // if we are in a background thread, refresh later when UI thread is active
        if (nedBeginChangeCount == 0) {
        	DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
                public void run() {
                    refresh();
                }
            });
        }
    }

    /**
     * Fully refresh ourselves an all of our children (recursively) visually
     */
    @Override
    public void refresh() {
    	super.refresh();
        for (Object child : getChildren())
            ((NedTreeEditPart)child).refresh();

    }

    public IPropertySource getPropertySource() {
        return propertySource;
    }

    public void setPropertySource(IPropertySource propertySource) {
        this.propertySource = propertySource;
    }
}
