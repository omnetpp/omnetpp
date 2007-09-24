package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.commands.Command;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * Allows the creation of new top level model element like, channels, simple modules
 * and compound modules
 *
 * @author rhornig
 */
public class CreateNedTypeElementCommand extends Command {
    private INedTypeElement child;
    private INEDElement parent;
    private INEDElement insertBefore;


    public CreateNedTypeElementCommand(INEDElement parent, INEDElement where, INedTypeElement child) {
        Assert.isTrue(parent instanceof NedFileElementEx || parent instanceof TypesElement, "The parent of a type must be a NedFile or a Types element");
    	this.child = child;
    	this.parent = parent;
    	this.insertBefore = where;
    }

    @Override
    public boolean canExecute() {
        return child != null && parent != null;
    }

    @Override
    public void execute() {
        setLabel("Create");
        redo();
    }

    @Override
    public void redo() {
        INedTypeElement namedChild = child;

        // if no name is present set to default
        if (namedChild.getName() == null || "".equals(namedChild.getName()))
            namedChild.setName("Unnamed");

        // make the name unique
        NedFileElementEx nedFile = parent.getContainingNedFileElement();
        namedChild.setName(NEDElementUtilEx.getUniqueNameForToplevelType(namedChild.getName(), nedFile));

        // insert
        parent.insertChildBefore(insertBefore, child);
    }

    @Override
    public void undo() {
    	child.removeFromParent();
    }

}
