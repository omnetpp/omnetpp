/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * Allows the creation of new top level or inner type model element like: channels, simple modules
 * and compound modules
 *
 * @author rhornig
 */
public class CreateNedTypeElementCommand extends Command {
    private INedTypeElement child;
    private INedElement parent;
    private INedElement insertBefore;

    /**
     * @param parent. The parent element Must be NedFileelementEx (top level) or TypesElement (Inner types)
     * @param where - the sibling before the element will be inserted. If NULL, it will be appended at the end
     * @param child - element to insert
     */
    public CreateNedTypeElementCommand(INedElement parent, INedElement where, INedTypeElement child) {
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

        // make the name unique. If parent is TypesElement the context is CompoundModule otherwise NedFile
        INedTypeLookupContext context = (parent instanceof TypesElement) ?
                (CompoundModuleElementEx)(parent.getParent()) : (NedFileElementEx)parent ;
        namedChild.setName(NedElementUtilEx.getUniqueNameForType(namedChild.getName(), context));

        // insert
        parent.insertChildBefore(insertBefore, child);
    }

    @Override
    public void undo() {
        child.removeFromParent();
    }

}
