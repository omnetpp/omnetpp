package org.omnetpp.ned.editor.graph.parts.outline;

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

import org.omnetpp.ned.editor.graph.parts.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.parts.policies.NedTreeContainerEditPolicy;
import org.omnetpp.ned.editor.graph.parts.policies.NedTreeEditPolicy;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedModelProvider;

/**
 * EditPart for the NED Outline view.
 *
 * @author rhornig
 */
// CHECK do we need the IPropertySourceSupport ?
public class NedTreeEditPart extends AbstractTreeEditPart implements INedModelProvider {
    protected DirectEditManager manager;

    /**
     * Constructor initializes this with the given model.
     */
    public NedTreeEditPart(Object model) {
        super(model);
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

    public INEDElement getNedModel() {
        return (INEDElement)getModel();
    }

    public boolean isEditable() {
    	NedFileElementEx nedFileElement = getNedModel().getContainingNedFileElement();
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

    /**
     * Fully refresh ourselves and all of our children (recursively) visually
     */
    @Override
    public void refresh() {
        // check if the associated model element is still in the model. if not, we should 
        // not refresh the element (ie. we cannot determine the icon for example as it requires the
        // display string, which requires type resolve which requires project context...)
        // of course the top level file element do not need a parent
        if (!(getNedModel() instanceof NedFileElementEx) && getNedModel().getParent() == null)
            return;
        
    	super.refresh();
        for (Object child : getChildren())
            ((NedTreeEditPart)child).refresh();

    }
}
