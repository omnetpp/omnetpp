package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.Datasets;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class DatasetSelectionDialog extends ElementListSelectOrCreateDialog {

	public DatasetSelectionDialog(final ScaveEditor editor) {
		super(editor.getSite().getShell(), new DatasetLabelProvider(editor));

		// setup dialog defaults
		setTitle("Select Target Dataset");
		setMessage("Select the dataset below, or create new one:");
		setSize(60, 15);

		// set initial contents
		Analysis analysis = editor.getAnalysis();
		Assert.isTrue(analysis.getDatasets()!=null); //XXX user must not be able to delete the "Datasets" element!
		List<Dataset> datasets = analysis.getDatasets().getDatasets();
		if (datasets == null)
			datasets = new ArrayList<Dataset>();
		setElements(datasets.toArray());


		// configure the "New" button to prompt for new dataset name
		setCallback(new ElementListSelectOrCreateDialog.ICallback() {
			public Object createNewObject() {
				InputDialog dlg = new InputDialog(editor.getSite().getShell(), "New Dataset", "Enter name for new Dataset:", "", null);
				if (dlg.open()== Window.OK) {
					String datasetName = dlg.getValue();
					Analysis analysis = editor.getAnalysis();
					Datasets datasets = analysis.getDatasets();
					Dataset newDataset = ScaveModelUtil.createDataset(datasetName);
					Command command = AddCommand.create(editor.getEditingDomain(), datasets,
							ScaveModelPackage.eINSTANCE.getDatasetItem(), newDataset);
					editor.executeCommand(command);
					return newDataset;
				}
				return null;
			}
		});
	}
	
	@Override
	protected IDialogSettings getDialogBoundsSettings() {
	    return UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
	}

	static class DatasetLabelProvider extends LabelProvider {
		private ILabelProvider imageProvider;
		
		public DatasetLabelProvider(ScaveEditor editor) {
			imageProvider = new AdapterFactoryLabelProvider(editor.getAdapterFactory());
		}
		
		public Image getImage(Object element) {
			if (element instanceof Dataset) {
				return imageProvider.getImage(element);
			}
			return null;
		}

		public String getText(Object element) {
			if (element instanceof Dataset) {
				Dataset dataset = (Dataset)element;
				return dataset.getName();
			}
			return super.getText(element);
		}
	}
}
