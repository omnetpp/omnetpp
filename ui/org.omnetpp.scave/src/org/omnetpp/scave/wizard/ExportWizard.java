package org.omnetpp.scave.wizard;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IExportWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarFields;
import org.omnetpp.scave.engine.ScaveExport;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Common base class for data export wizards.
 *
 * @author tomi
 */
public abstract class ExportWizard extends Wizard implements IExportWizard {

	protected IDList selectedScalars = IDList.EMPTY;
	protected IDList selectedVectors = IDList.EMPTY;
	protected IDList selectedHistograms = IDList.EMPTY;
	protected ResultFileManager manager;
	protected ExportWizardPage page;
	
	/**
	 * Sets the IDs of data to be exported and the ResultFileManager that owns them.
	 */
	public void init(IWorkbench workbench, IStructuredSelection selection) {
		// selection is IDListSelection
		if (selection instanceof IDListSelection) {
			IDListSelection idlistSelection = (IDListSelection)selection;
			selectedScalars = IDList.fromArray(idlistSelection.getScalarIDs());
			selectedVectors = IDList.fromArray(idlistSelection.getVectorIDs());
			selectedHistograms = IDList.fromArray(idlistSelection.getHistogramIDs());
			manager = idlistSelection.getResultFileManager();
		}
		// selection is Dataset or DatasetItem
		else if (selection.size() == 1 &&
				(selection.getFirstElement() instanceof Dataset || selection.getFirstElement() instanceof DatasetItem)) {
			// find resultfilemanager
			Object selected = selection.getFirstElement();
			if (workbench.getActiveWorkbenchWindow() != null) {
				IWorkbenchPage page = workbench.getActiveWorkbenchWindow().getActivePage();
				if (page != null) {
					IEditorPart part = page.getActiveEditor();
					if (part instanceof ScaveEditor)
						manager = ((ScaveEditor)part).getResultFileManager();
				}
			}
			if (manager != null) {
				Dataset dataset = ScaveModelUtil.findEnclosingOrSelf((EObject)selected, Dataset.class);
				DatasetItem item = selected instanceof DatasetItem ? (DatasetItem)selected : null;
				if (dataset != null) {
					selectedScalars = DatasetManager.getIDListFromDataset(manager, dataset, item, ResultType.SCALAR_LITERAL);
					selectedVectors = DatasetManager.getIDListFromDataset(manager, dataset, item, ResultType.VECTOR_LITERAL);
					selectedHistograms = DatasetManager.getIDListFromDataset(manager, dataset, item, ResultType.HISTOGRAM_LITERAL);
				}
			}
		}
	}
	
	@Override
	public boolean canFinish() {
		return page != null && manager != null && super.canFinish();
	}

	@Override
	public boolean performFinish() {
		if (page != null && manager != null) {
			try {
				// export the data
				String fileName = page.getFileName();
				ScaveExport exporter = createExporter();
				exporter.setPrecision(page.getPrecision());
				fileName = exporter.makeFileName(fileName);
				exporter.open(fileName);
				exportVectors(exporter);
				exportScalars(exporter);
				exporter.close();
				// save the control values before the dialog gets closed
				saveDialogSettings();
				return true;
			}
			catch (Exception e) {
				MessageDialog.openError(getShell(), "Error", "Error occured during export: " + e.toString());
			}
		}
		return false;
	}
	
	protected abstract ScaveExport createExporter();
	
	protected void exportScalars(ScaveExport exporter) {
		if (selectedScalars.size() > 0) {
			ScalarFields groupedBy = page.getGroupBy();
			exporter.saveScalars("scalars", "", selectedScalars, groupedBy, manager);
		}
	}

	protected void exportVectors(ScaveExport exporter) {
		for (int i = 0; i < selectedVectors.size(); ++i) {
			long id = selectedVectors.get(i);
			VectorResult vector = manager.getVector(id);
			XYArray data = DatasetManager.getDataOfVector(manager, id);
			exporter.saveVector(vector.getName(), "", data);
		}
	}

	protected void exportHistograms(ScaveExport exporter) {
		// TODO
	}
	
	protected void saveDialogSettings() {
		if (page != null)
			page.saveDialogSettings();
	}
}
