package org.omnetpp.scave.jobs;

import java.io.File;
import java.util.List;
import java.util.concurrent.Callable;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.export.IChartExport;
import org.omnetpp.scave.export.IGraphicalExportFileFormat;
import org.omnetpp.scave.model.Chart;

public class ExportChartsJob extends Job {

    List<Chart> charts;
    IChartExport exporter;
    ResultFileManager manager;
    IGraphicalExportFileFormat format;
    List<File> files;

    public ExportChartsJob(List<Chart> charts, IChartExport exporter, ResultFileManager manager, IGraphicalExportFileFormat format, List<File> outputFiles) {
        super("Chart Export");
        Assert.isNotNull(charts);
        Assert.isNotNull(exporter);
        Assert.isNotNull(manager);
        Assert.isNotNull(format);
        Assert.isNotNull(outputFiles);
        Assert.isTrue(!charts.isEmpty() && charts.size() == outputFiles.size());
        this.charts = charts;
        this.manager = manager;
        this.exporter = exporter;
        this.format = format;
        this.files = outputFiles;
    }

    @Override
    protected IStatus run(final IProgressMonitor monitor) {
        try {
            monitor.beginTask("Exporting charts.", charts.size());

            ResultFileManager.callWithReadLock(manager,
                new Callable<Object>() {
                    public Object call() throws CoreException {
                        for (int i = 0; i < charts.size(); ++i) {
                            if (monitor.isCanceled())
                                return Status.CANCEL_STATUS;
                            Chart chart = charts.get(i);
                            File file = files.get(i);
                            SubProgressMonitor subMonitor = new SubProgressMonitor(monitor, 1);
                            exporter.exportChart(chart, format, file, subMonitor);
                            // XXX targetFolder.refreshLocal(0, monitor); // because we're creating the file behind Eclipse's back
                        }
                        return null;
                    }
            });

            return Status.OK_STATUS;
        } catch (RuntimeException e) {
            return ScavePlugin.getErrorStatus(0, "Failed to export charts.", e);
        }
        finally {
            monitor.done();
        }
    }
}
