/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.NedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.pojo.ImportElement;

/**
 * Adds a newly created Submodule element to a compound module. Additionally adds an
 * import statement for the type if need.
 *
 * @author rhornig, andras
 */
public class CreateSubmoduleCommand extends Command {
    private String fullyQualifiedTypeName;
    private SubmoduleElementEx child;
    private Rectangle rect;
    private CompoundModuleElementEx parent;
    private ImportElement importElement;


    public CreateSubmoduleCommand(CompoundModuleElementEx parent, SubmoduleElementEx child) {
        Assert.isNotNull(parent);
        Assert.isNotNull(child);
        this.child = child;
        this.parent = parent;
        this.fullyQualifiedTypeName = NedElementUtilEx.getTypeOrLikeType(child);  // redo() destructively modifies child's type
    }

    @Override
    public boolean canExecute() {
        // check that the type exists
        return NedElement.getDefaultNedTypeResolver().lookupNedType(fullyQualifiedTypeName, parent) != null;
    }

    @Override
    public void execute() {
        setLabel("Create " + child.getName());

        redo();

        if (rect != null) {
            // get the scaling factor from the container module
            float scale = parent.getDisplayString().getScale();
            if (rect.width<0 || rect.height<0)
                child.getDisplayString().setLocation(rect.getLocation(), scale);
            else
                child.getDisplayString().setConstraint(rect.getLocation(), rect.getSize(), scale);
        }
    }

    @Override
    public void redo() {
        // make the submodule name unique if needed before inserting into the model
        child.setName(NedElementUtilEx.getUniqueNameFor(child, parent.getSubmodules()));

        // insert
        parent.insertSubmodule(null, child);

        // replace fully qualified type name with simple name + import
        importElement = NedElementUtilEx.addImportFor(child); // note: overwrites "type" (or "like-type") attribute
    }

    @Override
    public void undo() {
        parent.removeSubmodule(child);
        NedElementUtilEx.setTypeOrLikeType(child, fullyQualifiedTypeName); // restore original value (redo() will need it)
        if (importElement != null)
            importElement.removeFromParent();
    }

    public void setConstraint(Rectangle r) {
        rect = r;
    }
}