package org.omnetpp.scave.actions;

import java.util.Collection;
import java.util.Collections;
import java.util.List;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.DataTable;
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
 * Active on the "Browse Data" page. It adds the selected elements to
 * a dataset. The dataset selection dialog also allows creating
 * a new dataset.
 *
 * @author Andras
 */
public class AddSelectedToDatasetAction extends AbstractScaveAction {

	public AddSelectedToDatasetAction() {
		setText("Add Selected Data to Dataset...");
		setToolTipText("Add selected data to dataset");
		setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_ADDSELECTEDTODATASET));
	}

	@Override
	protected void doRun(final ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
		if (activePanel == null)
			return;
		
		DataTable table = activePanel.getTable();
		String filterPattern = activePanel.getFilter().getFilterPattern();
		ResultType type = editor.getBrowseDataPage().getActivePanelType();
		
		boolean addByFilter = false;
		if (table.getSelectionCount() == table.getItemCount()) {
			addByFilter = MessageDialog.openQuestion(editor.getSite().getShell(),
							"Add all",
							"All items are selected from the table. " +
							"It is more terse to specify them by the current filter expression, " +
							"than adding them one-by-one.\n\n" +
							"Would you like to use the current filter expression?");
		}
		
		DatasetSelectionDialog dlg = new DatasetSelectionDialog(editor);
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

				Collection<Add> addItems = addByFilter ?
											Collections.singletonList(
													ScaveModelUtil.createAdd(filterPattern, type)) :
											ScaveModelUtil.createAdds(
													table.getSelectedItems(),
													null);

				Command command = AddCommand.create(
							editor.getEditingDomain(),
							dataset,
							ScaveModelPackage.eINSTANCE.getDataset_Items(),
							addItems,
							index);
				editor.executeCommand(command);

				// show the dataset
				editor.showDatasetsPage(); // or: editor.openDataset(dataset);
				editor.setSelection(new StructuredSelection(addItems.toArray()));
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
		if (activePanel != null) {
			DataTable table = activePanel.getTable();
			return table.getSelectionCount() > 0;
		}
		return false;
	}
}
