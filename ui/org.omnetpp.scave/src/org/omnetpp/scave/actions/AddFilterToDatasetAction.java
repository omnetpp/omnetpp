package org.omnetpp.scave.actions;

import java.util.List;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.ui.DatasetSelectionDialog;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Active on the "Browse Data" page. It adds the filter string to
 * a dataset. The dataset selection dialog also allows creating
 * a new dataset.
 *
 * @author Andras
 */
public class AddFilterToDatasetAction extends AbstractScaveAction {

	public AddFilterToDatasetAction() {
		setText("Add Filter Expression to Dataset...");
		setToolTipText("Add filter expression to dataset");
		setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_ADDFILTERTODATASET));
	}

	@Override
	protected void doRun(final ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
		if (activePanel == null)
			return;

		if (!activePanel.isFilterPatternValid()) {
			MessageDialog.openWarning(editor.getSite().getShell(), "Error in Filter Expression", "Current filter expression is invalid, please fix that first.");
			return;
		}

		String filterPattern = activePanel.getFilter().getFilterPattern();

		if (filterPattern.length()==0 || filterPattern.equals("*")) {
			MessageDialog.openInformation(
					editor.getSite().getShell(),
					"Filter is Empty",
					"The filter expression is currently empty (matches everything), which means the resulting dataset "+
					"would include all data from all input files. This is probably not what you intended. "+
					"Please set some filter condition first.");
			return;
		}

		DatasetSelectionDialog dlg = new DatasetSelectionDialog(editor);
		dlg.setMessage(
				"Please select the target dataset below, or create new one.\n"+
				"This dataset will be modified to include all data from Inputs\n"+
				"that match the following filter:\n\n  "+
				filterPattern);
		if (dlg.open() == Window.OK) {
			Dataset dataset = (Dataset) dlg.getFirstResult();
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

				ResultType type = editor.getBrowseDataPage().getActivePanelType();
				Add addItem = ScaveModelUtil.createAdd(filterPattern, type);
				Command command = AddCommand.create(
							editor.getEditingDomain(),
							dataset,
							ScaveModelPackage.eINSTANCE.getDataset_Items(),
							addItem,
							index);
				editor.executeCommand(command);

				// show the dataset
				editor.showDatasetsPage(); // or: editor.openDataset(dataset);
				editor.setSelection(new StructuredSelection(addItem));
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		String filterString = editor.getBrowseDataPage().getActivePanel().getFilter().getFilterPattern();
		return !filterString.equals("");
	}
}
