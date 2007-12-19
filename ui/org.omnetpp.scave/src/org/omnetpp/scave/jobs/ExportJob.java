package org.omnetpp.scave.jobs;

import java.io.File;

import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.omnetpp.common.util.Pair;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItemFields;
import org.omnetpp.scave.engine.ScaveExport;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model2.DatasetManager;

/**
 * Job for exporting scalar/vector/histogram data in the background.
 *
 * @author tomi
 */
public class ExportJob extends WorkspaceJob
{
	private String fileName;
	private ScaveExport exporter;
	private ResultFileManager manager;
	private IDList scalars, vectors, histograms;
	private Dataset dataset;
	private DatasetItem datasetItem;
	private ResultItemFields scalarsGroupBy;
	
	public ExportJob(String fileName, ScaveExport exporter,
			IDList scalars, IDList vectors, IDList histograms,
			Dataset dataset, DatasetItem datasetItem,
			ResultItemFields scalarsGroupBy, ResultFileManager manager) {
		super("Data Export");
		this.fileName = fileName;
		this.exporter = exporter;
		this.scalars = scalars;
		this.vectors = vectors;
		this.histograms = histograms;
		this.dataset = dataset;
		this.datasetItem = datasetItem;
		this.scalarsGroupBy = scalarsGroupBy;
		this.manager = manager;
	}

	@Override
	public IStatus runInWorkspace(IProgressMonitor monitor)
			throws CoreException {
		
		if (fileName == null || exporter == null || manager == null)
			return Status.OK_STATUS;
		
		IStatus status = Status.CANCEL_STATUS;

		try {
			monitor.beginTask("Exporting", calculateTotalWork());
			exporter.open(fileName); // TODO: file exists: overwite/append
			
			status = exportVectors(exporter, monitor);
			if (status.getSeverity() != IStatus.OK)
				return status;

			status = exportScalars(exporter, monitor);
			if (status.getSeverity() != IStatus.OK)
				return status;
			
			status = exportHistograms(exporter, monitor);
			if (status.getSeverity() != IStatus.OK)
				return status;
			
			return Status.OK_STATUS;
		}
		catch (Exception e) {
			IStatus error = new Status(IStatus.ERROR, ScavePlugin.PLUGIN_ID, "Error occured during export", e);
			ScavePlugin.getDefault().getLog().log(error);
			return error;
		}
		finally {
			exporter.close();
			monitor.done();
			if (status.getSeverity() != IStatus.OK) {
				try {
					File file = new File(fileName); 
					if (file.exists())
						file.delete();
				} catch (Exception e) {
					ScavePlugin.logError("Cannot delete export file: "+fileName, e);
				}
			}
		}
	}
	
	protected int calculateTotalWork() {
		int work = 0;
		if (scalars != null && scalars.size() > 0)
			++work;
		if (vectors != null)
			work += 2 * vectors.size();
		if (histograms != null && histograms.size() > 0)
			++work;
		return work;
	}
	
	protected IStatus exportScalars(ScaveExport exporter, IProgressMonitor monitor) {
		if (scalars != null && scalars.size() > 0) {
			if (monitor.isCanceled())
				return Status.CANCEL_STATUS;
			exporter.saveScalars("scalars", "", scalars, scalarsGroupBy, manager);
			monitor.worked(1);
		}
		return Status.OK_STATUS;
	}

	protected IStatus exportVectors(ScaveExport exporter, IProgressMonitor monitor) {
		XYArray[] data = null;
		IProgressMonitor subMonitor = new SubProgressMonitor(monitor, vectors.size());
		if (dataset != null) {
			Pair<IDList, XYArray[]> pair =
				DatasetManager.readAndComputeVectorData(dataset, datasetItem, manager, subMonitor);
			vectors = pair.first;
			data = pair.second;
		}
		else {
			data = DatasetManager.getDataOfVectors(manager, vectors, subMonitor);
		}
		
		if (monitor.isCanceled())
			return Status.CANCEL_STATUS;

		for (int i = 0; i < vectors.size(); ++i) {
			if (monitor.isCanceled())
				return Status.CANCEL_STATUS;
			long id = vectors.get(i);
			VectorResult vector = manager.getVector(id);
			exporter.saveVector(vector.getName(), "", id, false, data[i], manager);
			monitor.worked(1);
		}

		return Status.OK_STATUS;
	}

	protected IStatus exportHistograms(ScaveExport exporter, IProgressMonitor monitor) {
		if (histograms != null && histograms.size() > 0) {
			if (monitor.isCanceled())
				return Status.CANCEL_STATUS;
			// TODO: do export
			monitor.worked(1);
		}
		return Status.OK_STATUS;
	}
}
