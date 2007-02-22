package org.omnetpp.scave.actions;

import java.util.List;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.ui.ElementListSelectOrCreateDialog;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave.model.Datasets;
import org.omnetpp.scave.model.Group;
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
		setText("Add filtered data to dataset...");
		setToolTipText("Add filtered data to dataset");
	}

	@Override
	protected void doRun(final ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
		List datasets = getDatasets(editor);
		if (activePanel == null || datasets == null)
			return;

		//ElementListSelectionDialog dlg = new ElementListSelectionDialog(editor.getSite().getShell(), new LabelProvider());
		ElementListSelectOrCreateDialog dlg = new ElementListSelectOrCreateDialog(editor.getSite().getShell(), new AdapterFactoryLabelProvider(editor.getAdapterFactory()));
		dlg.setTitle("Select Target Dataset");
		dlg.setMessage("Select the dataset below, or create new one:");
		dlg.setSize(60, 15);
		dlg.setElements(datasets.toArray());
		dlg.setCallback(new ElementListSelectOrCreateDialog.ICallback() {
			public Object createNewObject() {
				//XXX use DatasetEditForm
				InputDialog dlg = new InputDialog(editor.getSite().getShell(), "New Dataset", "Enter name for new Dataset:", "", null);
				if (dlg.open()== Window.OK) {
					String datasetName = dlg.getValue();
					Analysis analysis = editor.getAnalysis();
					Datasets datasets = analysis.getDatasets();
					Dataset newDataset = ScaveModelUtil.createDataset(datasetName, DatasetType.VECTOR_LITERAL); //FIXME type!!!
					Command command = AddCommand.create(editor.getEditingDomain(), datasets,
							ScaveModelPackage.eINSTANCE.getDatasetItem(), newDataset);
					editor.executeCommand(command);
					return newDataset;
				}
				return null;
			} 
		});

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

				String filterString = editor.getBrowseDataPage().getActivePanel().getFilterParams().getFilterPattern();

				Command command = AddCommand.create(
							editor.getEditingDomain(),
							dataset,
							ScaveModelPackage.eINSTANCE.getDataset_Items(),
							ScaveModelUtil.createAdd(filterString),
							index);
				editor.executeCommand(command);
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		System.out.println("begin isApplicable");
		String filterString = editor.getBrowseDataPage().getActivePanel().getFilterParams().getFilterPattern();
		System.out.println("FilterString: >>>>"+filterString+"<<<<");
		return !filterString.equals("");
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
