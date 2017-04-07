package org.omnetpp.scave.export;

import java.io.File;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.scave.model.Chart;

public interface IChartExport {
    String getName();
    boolean isSupportedFormat(IGraphicalExportFileFormat format);
    void exportChart(Chart chart, IGraphicalExportFileFormat format, File outputFile, IProgressMonitor monitor) throws CoreException;
}
