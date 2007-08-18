package org.omnetpp.ned.editor.graph.actions;

import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.ui.actions.SelectAllAction;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.IWorkbenchPart;

import org.omnetpp.ned.editor.graph.edit.ModuleEditPart;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class GNEDSelectAllAction extends SelectAllAction {

    IWorkbenchPart wbp;
    public GNEDSelectAllAction(IWorkbenchPart part) {
        super(part);
        wbp = part;
    }

	@Override
	@SuppressWarnings("unchecked")
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
            viewer.setSelection(new StructuredSelection(parentEP.getChildren()));
        }
    }
}
