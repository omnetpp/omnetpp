package org.omnetpp.ned.editor.graph.commands;

import java.util.LinkedList;
import java.util.List;
import java.util.Set;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * Clones a set of {@link INEDElement} (copy operation)
 *
 * @author rhornig
 */
public class CloneCommand extends Command {

    private List<INEDElement> newNodes;
    // nodes that should be cloned by the command
    private List<INEDElement> nodes;
    // parent node where cloned nodes should go
    private INEDElement parent;
    // sibling node before the cloned nodes should be inserted (null means insertion at the end)
    private INEDElement insertBefore;

    /**
     * Creates a generic cloning command that is able to clon any number of nodes and add them
     * to the provided parent node.
     * @param parent The parent node where the newly cloned nodes should go
     * @param insertBefore A sibling in the parent before we should insert the cloned nodes
     *        <code>null</code> should be used to insert at the end of the childlist
     */
    public CloneCommand(INEDElement parent, INEDElement insertBefore) {
        super("Clone");
        this.parent = parent;
        this.insertBefore = insertBefore;
        nodes = new LinkedList<INEDElement>();
    }

    /**
     * Add a new {@link INEDElement} to the cloning list.
     * @param nodeToBeCloned
     */
    public void addPart(INEDElement nodeToBeCloned) {
        nodes.add(nodeToBeCloned);
    }

    @Override
    public boolean canExecute() {
    	return parent != null && nodes.size() > 0;
    }

    // clone just a single node and insert it into the correct position
    protected INEDElement clonePart(INEDElement oldNode ) {
        // duplicate the subtree but do not add to the new parent yet
    	INEDElement newNode = oldNode.deepDup();
        // set a unique name is this is a named toplevel element
        if ((newNode instanceof IHasName) && (newNode instanceof INedTypeElement)) {
            Set<String> context = NEDResourcesPlugin.getNEDResources().getReservedComponentNames();
            ((IHasName)newNode).setName(NEDElementUtilEx.getUniqueNameFor((IHasName)newNode, context));
        }

    	// insert into the parent at the correct position
        parent.insertChildBefore(insertBefore, newNode);

        // keep track of the new modules so we can delete them in undo
        newNodes.add(newNode);

        return newNode;
    }

    @Override
    public void execute() {
        redo();
    }

    @Override
    public void redo() {
        newNodes = new LinkedList<INEDElement>();
        for (INEDElement toBeCloned : nodes)
        	clonePart(toBeCloned);
    }

    @Override
    public void undo() {
        for (INEDElement mod : newNodes)
            mod.removeFromParent();
    }

}