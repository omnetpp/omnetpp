package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NedFileNodeEx;
import org.omnetpp.ned2.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned2.model.pojo.ChannelNode;
import org.omnetpp.ned2.model.pojo.InterfaceNameNode;
import org.omnetpp.ned2.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned2.model.pojo.SimpleModuleNode;

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
        		(child instanceof CompoundModuleNodeEx ||
        		 child instanceof SimpleModuleNode ||
        		 child instanceof ChannelNode ||
        		 child instanceof ModuleInterfaceNode ||
        		 child instanceof ChannelInterfaceNode);
    }

    @Override
    public void execute() {
        setLabel("Create");
        redo();
    }

    @Override
    public void redo() {
        parent.insertChildBefore(insertBefore, child);
    }

    @Override
    public void undo() {
    	child.removeFromParent();
    }

}
