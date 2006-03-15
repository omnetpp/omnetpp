package org.omnetpp.ned.editor.graph.edit.outline;

import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.RootEditPart;
import org.eclipse.gef.editpolicies.RootComponentEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedContainerEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedTreeContainerEditPolicy;

/**
 * Tree EditPart for the Container.
 */
public class NedFileTreeEditPart extends NedTreeEditPart {

    /**
     * Constructor, which initializes this using the model given as input.
     */
    public NedFileTreeEditPart(Object model) {
        super(model);
    }

    /**
     * Creates and installs pertinent EditPolicies.
     */
    protected void createEditPolicies() {
        super.createEditPolicies();
        installEditPolicy(EditPolicy.CONTAINER_ROLE, new NedContainerEditPolicy());
        installEditPolicy(EditPolicy.TREE_CONTAINER_ROLE, new NedTreeContainerEditPolicy());
        // If this editpart is the contents of the viewer, then it is not
        // deletable!
        if (getParent() instanceof RootEditPart)
            installEditPolicy(EditPolicy.COMPONENT_ROLE, new RootComponentEditPolicy());
    }

}