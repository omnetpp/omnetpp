package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.DataTable;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.ui.BrowseDataPage;

/**
 * This action is the handler of the global "Select All" action, when
 * a Scave editor is active.
 * Currently it enabled only on the BrowseDataPage and selects all items of
 * the table.
 *
 * @author tomi
 */
public class SelectAllAction extends AbstractScaveAction {

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		BrowseDataPage page = scaveEditor.getBrowseDataPage();
		if (page != null) {
			FilteredDataPanel panel = page.getActivePanel();
			if (panel != null) {
				DataTable table = panel.getTable();
				if (table != null) {
					table.setFocus();
					table.selectAll();
				}
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor,
			IStructuredSelection selection) {
		if (editor.getActiveEditorPage() == editor.getBrowseDataPage()) {
			BrowseDataPage page = editor.getBrowseDataPage();
			return page != null && page.getActivePanel() != null && page.getActivePanel().getTable() != null;
			
		}
		return false;
	}
}
