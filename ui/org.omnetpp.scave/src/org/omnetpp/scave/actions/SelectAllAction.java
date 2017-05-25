/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.dialogs.MessageDialog;
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
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.datatable.IDataControl;
import org.omnetpp.scave.editors.ui.BrowseDataPage;

/**
 * This action is the handler of the global "Select All" action, when
 * a Scave editor is active.
 * Works for focused tables, trees, texts and combos on the active page.
 *
 * @author tomi
 */
public class SelectAllAction extends AbstractScaveAction {
    protected int limit = 10000;

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        Composite activePage = scaveEditor.getActiveEditorPage();
        if (activePage == null)
            return;

        Control focusControl = Display.getDefault().getFocusControl();
        if ((focusControl instanceof IDataControl ||
             focusControl instanceof Table || focusControl instanceof Tree ||
             focusControl instanceof Text || focusControl instanceof Combo)
                && hasAncestor(focusControl, activePage)) {
            if (focusControl instanceof IDataControl) {
                IDataControl control = (IDataControl)focusControl;
                // note: getItemCount() is not good for a tree: it tells the number of root items.
                if (control.getIDList().size() <= limit || confirm(focusControl.getShell(), "table")) {
                    control.setFocus();
                    control.selectAll();
                    control.notifyListeners(SWT.Selection, null);
                }
            }
            else if (focusControl instanceof Table) {
                Table table = (Table)focusControl;
                if (table.getItemCount() <= limit || confirm(table.getShell(), "table"))
                    table.selectAll();
            }
            else if (focusControl instanceof Tree) {
                Tree tree = (Tree)focusControl;
                if (tree.getItemCount() <= limit || confirm(tree.getShell(), "tree")) // note:
                    tree.selectAll();
            }
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
                IDataControl control = panel.getDataControl();
                if (control != null) {
                    // note: getItemCount() is not good for a tree: it tells the number of root items.
                    if (control.getIDList().size() <= limit || confirm(panel.getShell(), "table")) {
                        control.setFocus();
                        control.selectAll();
                        control.notifyListeners(SWT.Selection, null);
                    }
                }
            }
        }
        else if (activePage == scaveEditor.getChartsPage()) {
            scaveEditor.getChartsPage().getViewer().selectAll();
        }
    }

    protected boolean confirm(Shell shell, String controlName) {
        String message = StringUtils.capitalize(controlName) + " contains more than " + limit + " items, and selection may take a long time. Do you want to continue?";
        return MessageDialog.openConfirm(shell, "Confirm", message);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
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
