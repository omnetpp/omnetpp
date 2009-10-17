/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.SCRAP;

import java.util.List;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Adds items selected on the BrowseDataPage to a dataset.
 * The user is asked to select the target dataset.
 */
public class AddToDatasetAction extends AbstractScaveAction {
	public AddToDatasetAction() {
		setText("Add to dataset...");
		setToolTipText("Add to dataset");
	}

	@SuppressWarnings("unchecked")
	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
		List datasets = getDatasets(editor);
		if (activePanel == null || datasets == null)
			return;

		AddToDatasetDialog dialog = new AddToDatasetDialog(editor.getSite().getShell(), (Dataset[])datasets.toArray());
		dialog.setFilterParams(activePanel.getFilterParams());
		dialog.setFilterHints(activePanel.getFilterHints());
		dialog.setUseFilter(activePanel.getTable().getSelectionCount() == 0);
	
		int status = dialog.open();
		if (status == Window.OK) {
			Dataset dataset = dialog.getSelectedDataset();
			if (dataset != null) {
				// add the Add node before the first chart or group,
				// so they are affected by this action
				int index = 0;
				List<DatasetItem> items = dataset.getItems();
				for (; index < items.size(); ++index) {
					DatasetItem item = items.get(index);
					if (item instanceof Chart || item instanceof Group)
						break;
				}
			
				Command command = dialog.useFilter() ?
					AddCommand.create(
							editor.getEditingDomain(),
							dataset,
							ScaveModelPackage.eINSTANCE.getDataset_Items(),
							ScaveModelUtil.createAdd(dialog.getFilterParams()),
							index) :
					AddCommand.create(
							editor.getEditingDomain(),
							dataset,
							ScaveModelPackage.eINSTANCE.getDataset_Items(),
							ScaveModelUtil.createAdds(
									activePanel.getTable().getSelectedItems(),
									dialog.getRunIdFields()),
							index);
				editor.executeCommand(command);
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return editor.getBrowseDataPage().getActivePanel() != null && getDatasets(editor) != null;
	}

	private List getDatasets(ScaveEditor editor) {
		Analysis analysis = editor.getAnalysis();
		if (analysis.getDatasets() != null &&
			analysis.getDatasets().getDatasets() != null &&
			analysis.getDatasets().getDatasets().size() > 0)
			return analysis.getDatasets().getDatasets();
		else
			return null;
	}
}
