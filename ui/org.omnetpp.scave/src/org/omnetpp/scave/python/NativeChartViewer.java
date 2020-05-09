/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.python;

import java.io.File;

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
import org.omnetpp.scave.editors.MemoizationCache;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
import org.omnetpp.scave.pychart.PythonProcessPool;

/**
 * Displays a chart in a native plot widget.
 */
public class NativeChartViewer extends ChartViewerBase {

    public static boolean debug = Debug.isChannelEnabled("nativechartviewer");

    private PlotBase plot;
    private NativeChartPlotter chartPlotter;

    public NativeChartViewer(Composite parent, Chart chart, PythonProcessPool pool, ResultFileManager rfm, MemoizationCache memoizationCache) {
        super(chart, pool, rfm, memoizationCache);

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

        chartPlotter = new NativeChartPlotter(plot);
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
                case BAR: plot.setDataset(chartPlotter.getScalarDataset()); break;
                case HISTOGRAM: plot.setDataset(chartPlotter.getHistogramDataset()); break;
                case LINE: plot.setDataset(chartPlotter.getXyDataset()); break;
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
