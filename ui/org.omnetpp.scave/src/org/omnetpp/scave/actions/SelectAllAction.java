package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Tree;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.DataTable;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.ui.BrowseDataPage;

/**
 * This action is the handler of the global "Select All" action, when
 * a Scave editor is active.
 * Works for focused tables, trees, texts and combos on the active page.
 *
 * @author tomi
 */
public class SelectAllAction extends AbstractScaveAction {

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		Composite activePage = scaveEditor.getActiveEditorPage();
		if (activePage == null)
			return;

		Control focusControl = Display.getDefault().getFocusControl();
		if ((focusControl instanceof Table || focusControl instanceof Tree ||
			 focusControl instanceof Text || focusControl instanceof Combo) 
				&& hasAncestor(focusControl, activePage)) {
			if (focusControl instanceof Table)
				((Table)focusControl).selectAll();
			else if (focusControl instanceof Tree)
				((Tree)focusControl).selectAll();
			else if (focusControl instanceof Text)
				((Text)focusControl).selectAll();
			else if (focusControl instanceof Combo) {
				Combo combo = (Combo)focusControl;
				combo.setSelection(new Point(0, combo.getText().length()));
			}
			focusControl.notifyListeners(SWT.Selection, null);
		}
		else if (activePage == scaveEditor.getBrowseDataPage()) {
			FilteredDataPanel panel = ((BrowseDataPage)activePage).getActivePanel();
			if (panel != null) {
				DataTable table = panel.getTable();
				if (table != null) {
					table.setFocus();
					table.selectAll();
					table.notifyListeners(SWT.Selection, null);
				}
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor,
			IStructuredSelection selection) {
		Control focusControl = Display.getDefault().getFocusControl();
		Composite activePage = editor.getActiveEditorPage();
		return activePage != null &&
			((focusControl != null && hasAncestor(focusControl, activePage)) ||
					activePage == editor.getBrowseDataPage());
	}
	
	private boolean hasAncestor(Control control, Composite composite) {
		Composite ancestor = control.getParent();
		while (ancestor != null && ancestor != composite && !(ancestor instanceof Shell))
			ancestor = ancestor.getParent();
		return ancestor == composite;
	}
}
