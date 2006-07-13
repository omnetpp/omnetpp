package org.omnetpp.scave2.actions;

import java.util.List;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave2.editors.ui.AddToDatasetDialog;
import org.omnetpp.scave2.model.ScaveModelUtil;

/**
 * Adds items selected on the BrowseDataPage to a dataset.
 * The user is asked to select the target dataset.
 */
public class AddToDatasetAction extends AbstractScaveAction {
	public AddToDatasetAction() {
		setText("Add to dataset...");
		setToolTipText("Add to dataset");
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
		List datasets = getDatasets(editor);
		if (activePanel == null || datasets == null)
			return;

		AddToDatasetDialog dialog = new AddToDatasetDialog(editor.getSite().getShell(), (Dataset[])datasets.toArray());
		dialog.setFilterParams(activePanel.getFilterParams());
		dialog.setFilterHints(activePanel.getFilterHints());
		
		int status = dialog.open();
		if (status == Window.OK) {
			Dataset dataset = dialog.getSelectedDataset();
			if (dataset != null) {
				Command command = dialog.useFilter() ?
					AddCommand.create(
							editor.getEditingDomain(),
							dataset,
							ScaveModelPackage.eINSTANCE.getDataset_Items(),
							ScaveModelUtil.createAdd(dialog.getFilterParams())) :
					AddCommand.create(
							editor.getEditingDomain(),
							dataset,
							ScaveModelPackage.eINSTANCE.getDataset_Items(),
							ScaveModelUtil.createAdds(
									activePanel.getTable().getSelectedItems(),
									dialog.getRunIdKind()));
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
