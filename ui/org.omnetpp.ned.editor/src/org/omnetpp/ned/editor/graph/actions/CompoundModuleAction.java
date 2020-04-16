/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import java.util.List;

import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.ned.editor.graph.parts.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.parts.NedFileEditPart;
import org.omnetpp.ned.editor.graph.parts.SubmoduleEditPart;

/**
 * Base class for actions that require exactly one compound module to be selected.
 * Subclasses should override run().
 *
 * @author andras
 */
public class CompoundModuleAction extends org.eclipse.gef.ui.actions.SelectionAction {
    public CompoundModuleAction(IWorkbenchPart part) {
        super(part);
    }

    /**
     * This command can be executed when a single compound module or a submodule is selected
     */
    protected CompoundModuleEditPart getSelectionCompoundModule() {
        List<?> selectedObjects = getSelectedObjects();
        if (selectedObjects.size() > 0) {
            Object selected = selectedObjects.get(0);
            if (selected instanceof CompoundModuleEditPart)
                return (CompoundModuleEditPart)selected;
            if (selected instanceof SubmoduleEditPart)
                return ((SubmoduleEditPart)selected).getCompoundModulePart();
            if (selected instanceof NedFileEditPart) {
                for (Object child : ((NedFileEditPart)selected).getChildren())
                    if (child instanceof CompoundModuleEditPart)
                        return (CompoundModuleEditPart)child;
            }
        }
        return null;
    }

    @Override
    protected boolean calculateEnabled() {
        return getSelectionCompoundModule() != null;
    }
}
