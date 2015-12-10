/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.gef.commands.Command;
import org.omnetpp.common.displaymodel.DimensionF;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;

/**
 * Change the size and location of a compound module (location cannot be changed)
 *
 * @author rhornig
 */
public class SetCompoundModuleConstraintCommand extends Command {

    private DimensionF newSize;
    private DimensionF oldSize;
    private CompoundModuleElementEx module;

    public SetCompoundModuleConstraintCommand(CompoundModuleElementEx newModule) {
        super();
        module = newModule;
    }

    @Override
    public String getLabel() {
        return "Resize " + module.getName();
    }

    @Override
    public void execute() {
        oldSize = module.getDisplayString().getCompoundSize();
        redo();
    }

    @Override
    public void redo() {
        module.getDisplayString().setCompoundSize(newSize);
    }

    @Override
    public void undo() {
        module.getDisplayString().setCompoundSize(oldSize);
    }

    public void setSize(DimensionF p) {
        newSize = p;
    }
}