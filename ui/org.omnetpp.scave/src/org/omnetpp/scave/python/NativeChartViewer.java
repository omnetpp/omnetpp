package org.omnetpp.scave.python;

import java.io.File;
import java.util.Map;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MessageBox;
import org.omnetpp.common.Debug;
import org.omnetpp.scave.charting.BarPlotViewer;
import org.omnetpp.scave.charting.HistogramPlotViewer;
import org.omnetpp.scave.charting.LinePlotViewer;
import org.omnetpp.scave.charting.PlotViewerBase;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.pychart.INativeChartPlotter;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
import org.omnetpp.scave.pychart.PythonProcessPool;

public class NativeChartViewer extends ChartViewerBase {

    public static boolean debug = Debug.isChannelEnabled("nativechartviewer");

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
        public void setProperty(String key, String value) {
            if(debug)
                Debug.println("setProperty syncExec begin: " + key + " : " + value);
            Display.getDefault().syncExec(() -> {
                plotViewer.setProperty(key, value);
            });
            if(debug)
                Debug.println("setProperty syncExec end");
        }

        @Override
        public void setProperties(Map<String, String> properties) {
            if(debug)
                Debug.println("setProperties syncExec begin");
            Display.getDefault().syncExec(() -> {
                for (String k : properties.keySet())
                    plotViewer.setProperty(k, properties.get(k));
            });
            if(debug)
                Debug.println("setProperties syncExec end");
        }

        @Override
        public void setWarning(String warning) {
            Display.getDefault().syncExec(() -> {
                plotViewer.setWarningText(warning); // TODO separate text for this
            });
        }

        public void reset() {

            if (xyDataset != null)
                xyDataset.dispose();

            scalarDataset = new PythonScalarDataset(null);
            xyDataset = new PythonXYDataset(null);
            histogramDataset = new PythonHistogramDataset(null);
        }

        public void dispose() {
            if (xyDataset != null)
                xyDataset.dispose();
        }
    }

    private ChartPlotter chartPlotter = new ChartPlotter();
    private PlotViewerBase plotViewer;

    public NativeChartViewer(Composite parent, Chart chart, PythonProcessPool pool, ResultFileManager rfm) {
        super(chart, pool, rfm);

        switch (chart.getType()) {
        case BAR:
            plotViewer = new BarPlotViewer(parent, SWT.DOUBLE_BUFFERED);
            break;
        case HISTOGRAM:
            plotViewer = new HistogramPlotViewer(parent, SWT.DOUBLE_BUFFERED);
            break;
        case LINE:
            plotViewer = new LinePlotViewer(parent, SWT.DOUBLE_BUFFERED);
            break;
        case MATPLOTLIB:
        default:
            throw new RuntimeException("invalid chart type");
        }
    }

    public void runPythonScript(String script, File workingDir, Runnable runAfterDone, ExceptionHandler runAfterError) {
        if (plotViewer.isDisposed())
            return;

        // TODO: do not store these if the dataset is empty!
        final double zx = plotViewer.getZoomX();
        final double zy = plotViewer.getZoomY();
        final long vt = plotViewer.getViewportTop();
        final long vl = plotViewer.getViewportLeft();

        killPythonProcess();
        plotViewer.setWarningText(null);

        if (script == null || script.isEmpty()) {
            plotViewer.setStatusText("No Python script given");
            return;
        }

        plotViewer.setStatusText("Running Python script...");

        plotViewer.clear();

        try {
            acquireNewProcess();
            proc.getEntryPoint().setNativeChartPlotter(chartPlotter);
        }
        catch (RuntimeException e) {
            MessageBox mb = new MessageBox(Display.getCurrent().getActiveShell(), SWT.ICON_ERROR);
            mb.setMessage(e.getMessage());
            mb.open();
            return;
        }

        chartPlotter.reset();

        Runnable ownRunAfterDone = () -> {
            runAfterDone.run();

            Display.getDefault().syncExec(() -> {

                if(debug)
                    Debug.println("data received, starting drawing");

                plotViewer.setStatusText("Rendering chart...");
                plotViewer.update();
                if(debug)
                    Debug.println("status text updated");

                switch (chart.getType()) {
                case BAR: plotViewer.setDataset(chartPlotter.scalarDataset); break;
                case HISTOGRAM: plotViewer.setDataset(chartPlotter.histogramDataset); break;
                case LINE: plotViewer.setDataset(chartPlotter.xyDataset); break;
                case MATPLOTLIB: // fallthrough
                default: throw new RuntimeException("Wrong chart type.");
                }

                plotViewer.setStatusText("");
                plotViewer.update();

                plotViewer.setZoomX(zx);
                plotViewer.setZoomY(zy);
                plotViewer.scrollVerticalTo(vt);
                plotViewer.scrollHorizontalTo(vl);

                killPythonProcess();
            });
        };

        ExceptionHandler ownRunAfterError = (proc, e) -> {
            runAfterError.handle(proc, e);
            if (!proc.isKilledByUs()) {
                Display.getDefault().syncExec(() -> {
                    plotViewer.setStatusText(null);
                    plotViewer.setWarningText("An exception occurred during Python execution.");
                    plotViewer.update();
                });
            }
        };

        proc.pythonCallerThread.asyncExec(() -> {
            changePythonIntoDirectory(workingDir);
            proc.getEntryPoint().execute(script);
        }, ownRunAfterDone, ownRunAfterError);
    }

    @Override
    public Control getWidget() {
        return plotViewer;
    }

    public void setVisible(boolean visible) {
        plotViewer.setVisible(visible);
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


    public PlotViewerBase getPlotViewer() {
        return plotViewer;
    }

    public void dispose() {
        super.dispose();

        chartPlotter.dispose();
        if (plotViewer != null)
            plotViewer.dispose();
    }
}
