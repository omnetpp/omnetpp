package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.Datasets;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class DatasetSelectionDialog extends ElementListSelectOrCreateDialog {

	public DatasetSelectionDialog(final ScaveEditor editor) {
		super(editor.getSite().getShell(), new AdapterFactoryLabelProvider(editor.getAdapterFactory()));
		//XXX problem with this label provider: string begins with "dataset" (kills filter-by-typing)
		//XXX make own labelprovider, but it should provide images too

		// setup dialog defaults
		setTitle("Select Target Dataset");
		setMessage("Select the dataset below, or create new one:");
		setSize(60, 15);

		// set initial contents
		Analysis analysis = editor.getAnalysis();
		Assert.isTrue(analysis.getDatasets()!=null); //XXX user must not be able to delete the "Datasets" element!
		List datasets = analysis.getDatasets().getDatasets();
		if (datasets == null)
			datasets = new ArrayList();
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

	protected static String fallback(String string, String defaultString) {
		return (string!=null && !string.equals("")) ? string : defaultString;
	}
}
