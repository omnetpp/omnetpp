package org.omnetpp.scave.python;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.scave.charting.ChartViewer;
import org.omnetpp.scave.charting.HistogramChartViewer;
import org.omnetpp.scave.charting.ScalarChartViewer;
import org.omnetpp.scave.charting.VectorChartViewer;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.pychart.IChartPropertiesProvider;
import org.omnetpp.scave.pychart.INativeChartPlotter;
import org.omnetpp.scave.pychart.IScaveResultsPickleProvider;
import org.omnetpp.scave.pychart.PythonOutputMonitoringThread.IOutputListener;
import org.omnetpp.scave.pychart.PythonProcess;
import org.omnetpp.scave.pychart.PythonProcessPool;
import org.omnetpp.scave.python.MatplotlibChartViewer.IPy4JExceptionHandler;
import org.omnetpp.scave.python.MatplotlibChartViewer.IStateChangeListener;

import py4j.Py4JException;

public class NativeChartViewer {

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

    PythonProcess proc;
    PythonProcessPool processPool;

    public NativeChartViewer(Chart chart, PythonProcessPool pool, Composite parent) {
        this.chart = chart;
        processPool = pool;

        proc = null;

        if (chart instanceof BarChart)
            chartView = new ScalarChartViewer(parent, SWT.NONE);
        else if (chart instanceof LineChart || chart instanceof ScatterChart)
            chartView = new VectorChartViewer(parent, SWT.NONE);
        else if (chart instanceof HistogramChart)
            chartView = new HistogramChartViewer(parent, SWT.NONE);
    }


    List<IOutputListener> outputListeners = new ArrayList<IOutputListener>();
    List<IStateChangeListener> stateChangeListeners = new ArrayList<IStateChangeListener>();

    IScaveResultsPickleProvider resultsProvider = null;
    IChartPropertiesProvider propertiesProvider = null;


    public void runPythonScript(File workingDir, Runnable runAfterDone, IPy4JExceptionHandler runAfterError) {

        if (chartView.isDisposed())
            return;

        String script = chart.getScript();

        if (script == null || script.isEmpty()) {
            chartView.setStatusText("No Python script given");
            return;
        }

        chartView.setStatusText("Running Python script...");

        for (Property prop : chart.getProperties())
            chartView.setProperty(prop.getName(), prop.getValue());

        if (proc != null) {
            proc.dispose();

            for (MatplotlibChartViewer.IStateChangeListener l : stateChangeListeners)
                l.pythonProcessLivenessChanged(false);
        }

        proc = processPool.getProcess();

        for (MatplotlibChartViewer.IStateChangeListener l : stateChangeListeners)
            l.pythonProcessLivenessChanged(true);

        proc.getEntryPoint().setResultsProvider(resultsProvider);
        proc.getEntryPoint().setChartPropertiesProvider(propertiesProvider);
        proc.getEntryPoint().setNativeChartPlotter(chartPlotter);


        for (IOutputListener l : outputListeners)
            proc.outputMonitoringThread.addOutputListener(l);

        chartPlotter.reset();

        if (script != null && !script.isEmpty()) {
            proc.pythonCallerThread.asyncExec(() -> {

                if (workingDir != null)
                    proc.getEntryPoint().execute("import os; os.chdir(\"\"\"" + workingDir.getAbsolutePath() + "\"\"\"); del os;");

                try {
                    proc.getEntryPoint().execute(script);
                } catch (Py4JException e) {
                    if (runAfterError != null)
                        runAfterError.handle(e);
                    return;
                }

                proc.dispose();

                for (MatplotlibChartViewer.IStateChangeListener l : stateChangeListeners)
                    l.pythonProcessLivenessChanged(false);

                if (runAfterDone != null)
                    runAfterDone.run();

                Display.getDefault().syncExec(() -> {

                    chartView.setStatusText("Rendering chart...");
                    chartView.update();

                    if (chart instanceof BarChart)
                        chartView.setDataset(chartPlotter.scalarDataset);
                    else if (chart instanceof LineChart || chart instanceof ScatterChart)
                        chartView.setDataset(chartPlotter.xyDataset);
                    else if (chart instanceof HistogramChart)
                        chartView.setDataset(chartPlotter.histogramDataset);

                    chartView.setStatusText("");
                    chartView.update();
                });
            });
        }

    }

    public void setVisible(boolean visible) {
        chartView.setVisible(visible);
    }

    public void setResultsProvider(IScaveResultsPickleProvider resultsProvider) {
        this.resultsProvider = resultsProvider;
    }

    public void setChartPropertiesProvider(IChartPropertiesProvider propertiesProvider) {
        this.propertiesProvider = propertiesProvider;
    }

    public void addOutputListener(IOutputListener listener) {
        outputListeners.add(listener);
    }

    public void addStateChangeListener(IStateChangeListener listener) {
        stateChangeListeners.add(listener);
    }

    public void killPythonProcess() {
        if (proc != null && !proc.isDisposed())
            proc.dispose();
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

    public void dispose() {
        killPythonProcess();

        if (chartView != null)
            chartView.dispose();
    }

    public ChartViewer getChartViewer() {
        return chartView;
    }

    public boolean isAlive() {
        return proc != null && !proc.isDisposed();
    }
}
