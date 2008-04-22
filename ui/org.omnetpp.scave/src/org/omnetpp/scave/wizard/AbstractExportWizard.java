package org.omnetpp.scave.wizard;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.IExportWizard;
import org.eclipse.ui.IWorkbench;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScaveExport;
import org.omnetpp.scave.jobs.ExportJob;
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
public abstract class AbstractExportWizard extends Wizard implements IExportWizard {

	protected IDList selectedScalars = IDList.EMPTY;
	protected IDList selectedVectors = IDList.EMPTY;
	protected IDList selectedHistograms = IDList.EMPTY;
	protected Dataset selectedDataset;
	protected DatasetItem selectedDatasetItem;
	protected ResultFileManager manager;
	protected ExportWizardPage page;
	
	
	protected AbstractExportWizard() {
		setWindowTitle("Export");
	}
	
	/**
	 * Sets the IDs of data to be exported and the ResultFileManager that owns them.
	 */
	public void init(IWorkbench workbench, IStructuredSelection selection) {
		// XXX use the selection of the active Scave editor instead passed parameter
		
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
			ScaveEditor editor = ScaveEditor.getActiveScaveEditor(workbench);
			if (editor != null && selected instanceof Dataset || selected instanceof DatasetItem) {
				selectedDataset = ScaveModelUtil.findEnclosingOrSelf((EObject)selected, Dataset.class);
				selectedDatasetItem = (selected instanceof DatasetItem) ? (DatasetItem)selected : null;
				manager = editor.getResultFileManager();
				selectedScalars = DatasetManager.getIDListFromDataset(manager, selectedDataset, selectedDatasetItem, ResultType.SCALAR_LITERAL);
				selectedVectors = DatasetManager.getIDListFromDataset(manager, selectedDataset, selectedDatasetItem, ResultType.VECTOR_LITERAL);
				selectedHistograms = DatasetManager.getIDListFromDataset(manager, selectedDataset, selectedDatasetItem, ResultType.HISTOGRAM_LITERAL);
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
				ScaveExport exporter = createExporter();
				exporter.setPrecision(page.getPrecision());
				exporter.setBaseFileName(page.getFileName());
				ExportJob job = new ExportJob(exporter, 
										selectedScalars, selectedVectors, selectedHistograms,
										selectedDataset, selectedDatasetItem,
										page.getGroupBy(), manager);
				job.schedule();
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
	
	protected void saveDialogSettings() {
		if (page != null)
			page.saveDialogSettings();
	}
}
