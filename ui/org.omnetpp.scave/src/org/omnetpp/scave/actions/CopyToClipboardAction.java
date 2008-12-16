/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;

/**
 * ...
 */
public class CopyToClipboardAction extends AbstractScaveAction {
	public CopyToClipboardAction() {
		setText("Copy to Clipboard");
		setToolTipText("Copy Data to Clipboard"); //TODO  in various formats!!!!
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
