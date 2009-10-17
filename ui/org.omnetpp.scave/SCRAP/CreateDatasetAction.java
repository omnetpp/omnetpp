/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.SCRAP;

import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class CreateDatasetAction extends AbstractScaveAction {
	public CreateDatasetAction() {
		setText("Create dataset...");
		setToolTipText("Create dataset from current data");
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();

		CreateDatasetDialog dialog = new CreateDatasetDialog(editor.getSite().getShell(), "Create dataset");
		dialog.setFilterParams(activePanel.getFilterParams());
		dialog.setFilterHints(activePanel.getFilterHints());
		dialog.setUseFilter(activePanel.getTable().getSelectionCount() == 0);

		int result = dialog.open();
		if (result == Window.OK) {
			Dataset dataset = dialog.useFilter() ?
				ScaveModelUtil.createDataset(
						dialog.getDatasetName(),
						activePanel.getTable().getDataType(),
						dialog.getFilterParams()) :
				ScaveModelUtil.createDataset(
						dialog.getDatasetName(),
						activePanel.getTable().getDataType(),
						activePanel.getTable().getSelectedItems(),
						dialog.getRunIdFields());
	
			editor.executeCommand(AddCommand.create(
					editor.getEditingDomain(),
					editor.getAnalysis().getDatasets(),
					ScaveModelPackage.eINSTANCE.getDatasets_Datasets(),
					dataset));
			editor.openDataset(dataset);
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return editor.getBrowseDataPage().getActivePanel() != null;
	}
}
