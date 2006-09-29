package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned2.model.INamed;
import org.omnetpp.ned2.model.ITopLevelElement;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NedFileNodeEx;

/**
 * @author rhornig
 * Allows the creation of new top level model element like, channels, simple mosules 
 * and compound modules
 */
public class CreateToplevelComponentCommand extends Command {
    private NEDElement child;
    private NEDElement parent;
    private NEDElement insertBefore;

    
    public CreateToplevelComponentCommand(NEDElement parent, NEDElement where, NEDElement child) {
    	this.child = child;
    	this.parent = parent;
    	this.insertBefore = where;
    }
    
    @Override
    public boolean canExecute() {
        return child != null && parent != null && 
        		parent instanceof NedFileNodeEx &&
        		child instanceof ITopLevelElement;
    }

    @Override
    public void execute() {
        setLabel("Create");
        redo();
    }

    @Override
    public void redo() {
        parent.insertChildBefore(insertBefore, child);
        if (child instanceof INamed) {
        	((INamed)child).setName("unnamed");
        	// TODO make the element name unique if needed
        }
    }

    @Override
    public void undo() {
    	child.removeFromParent();
    }

}
