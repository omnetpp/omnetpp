package org.omnetpp.scave.export;

import java.io.File;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.JobChangeAdapter;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.canvas.ZoomableCachingCanvas;
import org.omnetpp.scave.charting.ChartViewerFactory;
import org.omnetpp.scave.charting.IChartView;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;

public class NativeChartExport implements IChartExport {
    protected ResultFileManager manager;

    public NativeChartExport(ResultFileManager manager) {
        this.manager = manager;
    }

    public String getName() {
        return "Native chart renderer";
    }

    public boolean isSupportedFormat(IGraphicalExportFileFormat format) {
        return format instanceof SvgFormat;
    }

    public void exportChart(final Chart chart, final IGraphicalExportFileFormat format, final File outputFile, final IProgressMonitor monitor) throws CoreException {
        try {
            if (monitor != null)
                monitor.beginTask("Exporting chart " + chart.getName(), 2);
            Display.getDefault().syncExec(new Runnable() {
                private IChartView chartView;

                public void run() {
                    chartView = ChartFactory.createChart(Display.getCurrent().getActiveShell(), chart, manager, new JobChangeAdapter() {
                        @Override
                        public void done(IJobChangeEvent event) {
                            if (monitor != null)
                                monitor.worked(1);
                            if (event.getResult().isOK()) {
                                Display.getDefault().asyncExec(new Runnable() {
                                    public void run() {
                                        SvgFormat svgFormat = (SvgFormat)format;
                                        ZoomableCachingCanvas canvas = chartView.getCanvas();
                                        Point p = Display.getCurrent().getDPI();
                                        canvas.setSize((int)svgFormat.getWidth() * p.x, (int)svgFormat.getHeight() * p.y);
                                        canvas.exportToSVG(outputFile.getAbsolutePath());
                                    }
                                });
                            }
                        }
                    });
                }
            });
        } finally {
            if (monitor != null)
                monitor.done();
        }
    }
}
