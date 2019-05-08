package org.omnetpp.scave.python;

import java.io.File;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave.charting.ChartViewer;
import org.omnetpp.scave.charting.HistogramChartViewer;
import org.omnetpp.scave.charting.ScalarChartViewer;
import org.omnetpp.scave.charting.VectorChartViewer;
import org.omnetpp.scave.charting.properties.ChartDefaults;
import org.omnetpp.scave.charting.properties.ChartVisualProperties;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.pychart.INativeChartPlotter;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
import org.omnetpp.scave.pychart.PythonProcessPool;

public class NativeChartViewer extends ChartViewerBase {

    class ChartPlotter implements INativeChartPlotter {
        PythonScalarDataset scalarDataset = new PythonScalarDataset(null);
        PythonXYDataset xyDataset = new PythonXYDataset(null);
        PythonHistogramDataset histogramDataset = new PythonHistogramDataset(null);


        @Override
        public void plotScalars(byte[] pickledData) {
            scalarDataset.addValues(pickledData);
        }

        @Override
        public void plotVectors(byte[] pickledData) {
            xyDataset.addVectors(pickledData);
        }

        @Override
        public void plotHistograms(byte[] pickledData) {
            histogramDataset.addValues(pickledData);
        }

        @Override
        public void setChartProperty(String key, String value) {
            Display.getDefault().syncExec(() -> {
                chartView.setProperty(key, value);
            });
        }

        public void reset() {
            scalarDataset = new PythonScalarDataset(null);
            xyDataset = new PythonXYDataset(null);
            histogramDataset = new PythonHistogramDataset(null);
        }
    }

    Chart chart;
    ChartPlotter chartPlotter = new ChartPlotter();
    ChartViewer chartView;

    public NativeChartViewer(Chart chart, PythonProcessPool pool, Composite parent) {
        super(pool);
        this.chart = chart;

        switch (chart.getType()) {
        case BAR:
            chartView = new ScalarChartViewer(parent, SWT.DOUBLE_BUFFERED);
            break;
        case HISTOGRAM:
            chartView = new HistogramChartViewer(parent, SWT.DOUBLE_BUFFERED);
            break;
        case LINE:
            chartView = new VectorChartViewer(parent, SWT.DOUBLE_BUFFERED);
            break;
        case MATPLOTLIB:
        default:
            throw new RuntimeException("invalid chart type");
        }
    }

    public void runPythonScript(String script, File workingDir, Runnable runAfterDone, ExceptionHandler runAfterError) {
        if (chartView.isDisposed())
            return;

        killPythonProcess();

        if (script == null || script.isEmpty()) {
            chartView.setStatusText("No Python script given");
            return;
        }

        chartView.setStatusText("Running Python script...");

        IPropertySource2 propSource = ChartVisualProperties.createPropertySource(chart);

        for (IPropertyDescriptor desc : propSource.getPropertyDescriptors()) {
            String id = (String)desc.getId();
            Property prop = chart.lookupProperty(id);
            if (prop != null)
                chartView.setProperty(id, prop.getValue());
            else
                chartView.setProperty(id, Converter.objectToString(ChartDefaults.getDefaultPropertyValue(id)));
        }

        acquireNewProcess();
        proc.getEntryPoint().setNativeChartPlotter(chartPlotter);

        chartPlotter.reset();

        Runnable ownRunAfterDone = () -> {
            runAfterDone.run();

            Display.getDefault().syncExec(() -> {
                chartView.setStatusText("Rendering chart...");
                chartView.update();

                switch (chart.getType()) {
                case BAR:
                    chartView.setDataset(chartPlotter.scalarDataset);
                    break;
                case HISTOGRAM:
                    chartView.setDataset(chartPlotter.histogramDataset);
                    break;
                case LINE:
                    chartView.setDataset(chartPlotter.xyDataset);
                    break;
                case MATPLOTLIB:
                default:
                    throw new RuntimeException("Wrong chart type.");
                }

                chartView.setStatusText("");
                chartView.update();

                killPythonProcess();
            });
        };

        ExceptionHandler ownRunAfterError = (proc, e) -> {
            runAfterError.handle(proc, e);
            if (!proc.isKilledByUs()) {
                Display.getDefault().syncExec(() -> {
                    chartView.setStatusText("An exception occurred during Python execution.");
                    chartView.update();
                });
            }
        };

        proc.pythonCallerThread.asyncExec(() -> {
            changePythonIntoDirectory(workingDir);
            proc.getEntryPoint().execute(script);
        }, ownRunAfterDone, ownRunAfterError);
    }

    public void setVisible(boolean visible) {
        chartView.setVisible(visible);
    }

//
//    protected void updateContextMenu() {
//            add(new EditAction());
//            //add(new EditScriptAction());
//            if (chart instanceof LineChart || chart instanceof ScatterChart) {
//                add(new Separator());
//            }
//            add(new Separator());
//            add(editorContributor.getGotoChartDefinitionAction());
//            add(new Separator());
//            add(createZoomSubmenu());
//            add(new Separator());
//            add(editorContributor.getCopyChartToClipboardAction());
//            add(editorContributor.getExportToSVGAction());
//            add(new Separator());
//            add(editorContributor.getUndoRetargetAction());
//            add(editorContributor.getRedoRetargetAction());
//            add(new Separator());
//            add(editorContributor.getRefreshChartAction());
//    }
//
//    private IMenuManager createZoomSubmenu() {
//        IMenuManager zoomSubmenuManager = new MenuManager("Zoom", ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_ZOOM), null);
//        zoomSubmenuManager.add(editorContributor.getHZoomInAction());
//        zoomSubmenuManager.add(editorContributor.getHZoomOutAction());
//        zoomSubmenuManager.add(editorContributor.getVZoomInAction());
//        zoomSubmenuManager.add(editorContributor.getVZoomOutAction());
//        zoomSubmenuManager.add(new Separator());
//        zoomSubmenuManager.add(editorContributor.getZoomToFitAction());
//        return zoomSubmenuManager;
//    }


    public ChartViewer getChartViewer() {
        return chartView;
    }

    public void dispose() {
        super.dispose();

        if (chartView != null)
            chartView.dispose();
    }
}
