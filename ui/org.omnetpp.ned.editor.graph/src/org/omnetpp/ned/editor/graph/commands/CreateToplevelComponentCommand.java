package org.omnetpp.ned.editor.graph.commands;

import java.util.Set;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileNodeEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.ITopLevelElement;
import org.omnetpp.resources.NEDResourcesPlugin;

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
        
        if ((child instanceof IHasName) && (child instanceof ITopLevelElement)) {
            IHasName namedChild = (IHasName)child;
            // if no name is present set to default
            if (namedChild.getName() == null || "".equals(namedChild.getName()))
                namedChild.setName("unnamed");
            // make the name unique
            Set<String> context = NEDResourcesPlugin.getNEDResources().getReservedNames();
            namedChild.setName(NEDElementUtilEx.getUniqueNameFor(namedChild, context));
        }

        parent.insertChildBefore(insertBefore, child);
    }

    @Override
    public void undo() {
    	child.removeFromParent();
    }

}
