package org.omnetpp.scave2.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.datatable.FilteredDataPanel;

/**
 * ...
 */
public class CopyToClipboardAction extends AbstractScaveAction {
	public CopyToClipboardAction() {
		setText("Copy to clipboard...");
		setToolTipText("Copy data to clipboard in various formats");
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
		if (activePanel != null)
			activePanel.getTable().copySelectionToClipboard();
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return editor.getBrowseDataPage().getActivePanel() != null;
	}
}
