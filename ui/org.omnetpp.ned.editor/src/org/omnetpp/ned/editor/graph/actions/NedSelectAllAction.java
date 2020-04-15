/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.ui.actions.SelectAllAction;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.ned.editor.graph.parts.ModuleEditPart;
import org.omnetpp.ned.editor.graph.parts.TypesEditPart;

/**
 * Same as a the SelectAllAction but handles selects only all the submodules inside a
 * compound module if the compound module or any submodule is selected. If other toplevel
 * types is selected then all top level will be selected.
 *
 * @author rhornig
 */
public class NedSelectAllAction extends SelectAllAction {

    IWorkbenchPart wbp;
    public NedSelectAllAction(IWorkbenchPart part) {
        super(part);
        wbp = part;
    }

    // TODO handle select all connections in the current compound module too
    @Override
    @SuppressWarnings({"rawtypes", "unchecked"})
    public void run() {
        GraphicalViewer viewer = (GraphicalViewer)wbp.getAdapter(GraphicalViewer.class);
        if (viewer != null) {
            List selection = viewer.getSelectedEditParts();
            // by default the we select all children of the top level edit part
            EditPart parentEP = viewer.getContents();
            // or we check if there is a selection. in this case parent is the compound module
            // in which the selection is present
            if (selection.size() > 0 && selection.get(0) instanceof ModuleEditPart)
                parentEP = ((ModuleEditPart)selection.get(0)).getCompoundModulePart();

            // we have the parent where all child part have to be selected
            // Leave out the unselectable ones (e.g. inner types compartment)
            List children = new ArrayList();
            for (Object child : parentEP.getChildren())
                if (!(child instanceof TypesEditPart))
                    children.add(child);

            viewer.setSelection(new StructuredSelection(children));
        }
    }
}
