package org.omnetpp.scave2.actions;

import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryContentProvider;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.ui.FilterPanel;
import org.omnetpp.scave2.model.FilterParams;
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
		FilterPanel activePanel = editor.getBrowseDataPage().getActivePanel();
		//FIXME tomi: mi van ha activePanel==null???
		ListDialog dialog = new ListDialog(editor.getSite().getShell());
		dialog.setInput(editor.getAnalysis().getDatasets());
		dialog.setContentProvider(new AdapterFactoryContentProvider(editor.getAdapterFactory()));
		dialog.setLabelProvider(new AdapterFactoryLabelProvider(editor.getAdapterFactory()));
		dialog.setMessage("Select dataset:");
		dialog.setTitle("Add to dataset");
		int status = dialog.open();
		if (status == Window.OK) {
			Object[] result = dialog.getResult();
			if (result != null && result.length == 1) {
				Dataset dataset = (Dataset)result[0];
				FilterParams params = activePanel.getFilterParams();
				Add add = ScaveModelUtil.createAdd(params);
				editor.executeCommand(new AddCommand(editor.getEditingDomain(), dataset.getItems(), add));
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return editor.getBrowseDataPage().getActivePanel() != null;
	}
}
