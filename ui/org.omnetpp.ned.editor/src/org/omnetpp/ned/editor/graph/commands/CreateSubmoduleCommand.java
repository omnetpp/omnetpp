package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Rectangle;

import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;

/**
 * Adds a newly created Submodule element to a compound module.
 *
 * @author rhornig
 */
public class CreateSubmoduleCommand extends org.eclipse.gef.commands.Command {

    private SubmoduleElementEx child;
    private Rectangle rect;
    private CompoundModuleElementEx parent;
    // TODO substitute index based positioning with sibling based one
    private int index = -1;


    public CreateSubmoduleCommand(CompoundModuleElementEx parent, SubmoduleElementEx child) {
        Assert.isNotNull(parent);
        Assert.isNotNull(child);
    	this.child = child;
    	this.parent = parent;
    }

    @Override
    public boolean canExecute() {
        // check if the type exist and the name is valid
        boolean isGlobalType = NEDElement.getDefaultTypeResolver().getModuleNames().contains(child.getEffectiveType());
        boolean isLocalType = parent.getNEDTypeInfo().getInnerTypes().containsKey(child.getEffectiveType());
        return isGlobalType || isLocalType;
    }

    @Override
    public void execute() {
        setLabel("Create " + child.getName());

        if (rect != null) {
            // get the scaling factor from the container module
            float scale = parent.getDisplayString().getScale();
            child.getDisplayString().setConstraint(rect.getLocation(), rect.getSize(), scale);
        }
        redo();
    }

    @Override
    public void redo() {
        //make the submodule name unique if needed before inserting into the model
        child.setName(NEDElementUtilEx.getUniqueNameFor(child, parent.getSubmodules()));
        parent.insertSubmodule(index, child);
    }

    @Override
    public void undo() {
        parent.removeSubmodule(child);
    }

    public void setLocation(Rectangle r) {
        rect = r;
    }
}