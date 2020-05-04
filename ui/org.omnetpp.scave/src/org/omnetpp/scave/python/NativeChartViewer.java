package org.omnetpp.scave.python;

import java.io.File;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MessageBox;
import org.omnetpp.common.Debug;
import org.omnetpp.scave.charting.BarPlot;
import org.omnetpp.scave.charting.HistogramPlot;
import org.omnetpp.scave.charting.LinePlot;
import org.omnetpp.scave.charting.PlotBase;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.pychart.INativeChartPlotter;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
import org.omnetpp.scave.pychart.PythonProcessPool;

/**
 * Displays a chart in a native plot widget.
 */
public class NativeChartViewer extends ChartViewerBase {

    public static boolean debug = Debug.isChannelEnabled("nativechartviewer");

    private PlotBase plot;
    private ChartPlotter chartPlotter = new ChartPlotter();
    Map<String, String> pendingPropertyChanges = new HashMap<>();

    class ChartPlotter implements INativeChartPlotter {
        GroupsSeriesDataset scalarDataset = new GroupsSeriesDataset(null);
        XYDataset xyDataset = new XYDataset(null);
        HistogramDataset histogramDataset = new HistogramDataset(null);


        @Override
        public void plotScalars(byte[] pickledData, Map<String, String> props) {
            List<String> seriesKeys = scalarDataset.addValues(pickledData);

            pendingPropertyChanges.putAll(props);
            for (String seriesKey : seriesKeys)
                for (String propKey : props.keySet())
                    pendingPropertyChanges.put(propKey + "/" + seriesKey, props.get(propKey));
        }

        @Override
        public void plotVectors(byte[] pickledData, Map<String, String> props) {
            List<String> lineKeys = xyDataset.addVectors(pickledData);

            for (String lineKey : lineKeys)
                for (String propKey : props.keySet())
                    pendingPropertyChanges.put(propKey + "/" + lineKey, props.get(propKey));
        }

        @Override
        public void plotHistograms(byte[] pickledData, Map<String, String> props) {
            List<String> histKeys = histogramDataset.addValues(pickledData);

            for (String histKey : histKeys)
                for (String propKey : props.keySet())
                    pendingPropertyChanges.put(propKey + "/" + histKey, props.get(propKey));
        }

        @Override
        public boolean isEmpty() {
            return scalarDataset.getSeriesCount() == 0
                    && scalarDataset.getGroupCount() == 0
                    && xyDataset.getSeriesCount() == 0
                    && histogramDataset.getSeriesCount() == 0;
        }

        protected void doSetProperty(String key, String value) {
            final RuntimeException exc[] = new RuntimeException[] { null };
            Display.getDefault().syncExec(() -> {
                try {
                    plot.setProperty(key, value);
                }
                catch (RuntimeException e) {
                    exc[0] = e;
                }
            });
            if (exc[0] != null)
                throw exc[0];
        }

        protected void doSetProperties(Map<String, String> props) {
            final RuntimeException exc[] = new RuntimeException[] { null };
            Display.getDefault().syncExec(() -> {
                try {
                    for (String k : props.keySet())
                        plot.setProperty(k, props.get(k));
                }
                catch (RuntimeException e) {
                    exc[0] = e;
                }
            });
            if (exc[0] != null)
                throw exc[0];
        }

        public void applyPendingPropertyChanges() {
            try {
                doSetProperties(pendingPropertyChanges);
            }
            finally {
                pendingPropertyChanges.clear();
            }
        }

        @Override
        public void setProperty(String key, String value) {
            pendingPropertyChanges.put(key, value);
        }

        @Override
        public void setProperties(Map<String, String> properties) {
            pendingPropertyChanges.putAll(properties);
        }

        @Override
        public void setGroupTitles(List<String> titles) {
             scalarDataset.setGroupTitles(titles);
        }

        @Override
        public void setWarning(String warning) {
            Display.getDefault().syncExec(() -> {
                plot.setWarningText(warning);
            });
        }

        public void reset() {
            if (xyDataset != null)
                xyDataset.dispose();

            scalarDataset = new GroupsSeriesDataset(null);
            xyDataset = new XYDataset(null);
            histogramDataset = new HistogramDataset(null);
        }

        public void dispose() {
            if (xyDataset != null)
                xyDataset.dispose();
        }

        @Override
        public Set<String> getSupportedPropertyKeys() {
            Set<String> result = new HashSet<String>();
            result.addAll(Arrays.asList(plot.getPropertyNames()));
            return result;
        }
    }

    public NativeChartViewer(Composite parent, Chart chart, PythonProcessPool pool, ResultFileManager rfm) {
        super(chart, pool, rfm);

        switch (chart.getType()) {
        case BAR:
            plot = new BarPlot(parent, SWT.DOUBLE_BUFFERED);
            break;
        case HISTOGRAM:
            plot = new HistogramPlot(parent, SWT.DOUBLE_BUFFERED);
            break;
        case LINE:
            plot = new LinePlot(parent, SWT.DOUBLE_BUFFERED);
            break;
        case MATPLOTLIB:
        default:
            throw new RuntimeException("invalid chart type");
        }
    }

    public void runPythonScript(String script, File workingDir, Runnable runAfterDone, ExceptionHandler runAfterError) {
        if (plot.isDisposed())
            return;

        final double zx = chartPlotter.isEmpty() || !plot.getHorizontalBar().isVisible() ? Double.NaN : plot.getZoomX();
        final double zy = chartPlotter.isEmpty() || !plot.getVerticalBar().isVisible()? Double.NaN : plot.getZoomY();
        final long vt = plot.getViewportTop();
        final long vl = plot.getViewportLeft();

        killPythonProcess();
        plot.setWarningText(null);

        if (script == null || script.isEmpty()) {
            plot.setStatusText("No Python script given");
            return;
        }

        plot.setStatusText("Running Python script...");
        plot.clear();
        chartPlotter.applyPendingPropertyChanges();


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

                plot.setStatusText("Rendering chart...");
                chartPlotter.applyPendingPropertyChanges();
                plot.update();
                if(debug)
                    Debug.println("status text updated");

                switch (chart.getType()) {
                case BAR: plot.setDataset(chartPlotter.scalarDataset); break;
                case HISTOGRAM: plot.setDataset(chartPlotter.histogramDataset); break;
                case LINE: plot.setDataset(chartPlotter.xyDataset); break;
                case MATPLOTLIB: // fallthrough
                default: throw new RuntimeException("Wrong chart type.");
                }

                plot.setStatusText("");
                chartPlotter.applyPendingPropertyChanges();
                plot.update();

                if (Double.isFinite(zx)) {
                    plot.setZoomX(zx);
                    plot.scrollHorizontalTo(vl);
                }
                if (Double.isFinite(zy)) {
                    plot.setZoomY(zy);
                    plot.scrollVerticalTo(vt);
                }

                killPythonProcess();
            });
        };

        ExceptionHandler ownRunAfterError = (proc, e) -> {
            runAfterError.handle(proc, e);
            if (!proc.isKilledByUs()) {
                Display.getDefault().syncExec(() -> {
                    plot.setStatusText(null);
                    plot.setWarningText("An exception occurred during Python execution.");
                    plot.update();
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
        return plot;
    }

    public void setVisible(boolean visible) {
        plot.setVisible(visible);
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
//            add(editorContributor.getSaveImageAction());
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


    public PlotBase getPlot() {
        return plot;
    }

    @Override
    public void copyImageToClipboard() {
        plot.copyImageToClipboard();
    }

    @Override
    public void saveImage(String fileName) {
        plot.saveImage(fileName);
    }

    @Override
    public void dispose() {
        super.dispose();

        chartPlotter.dispose();
        if (plot != null)
            plot.dispose();
    }
}
