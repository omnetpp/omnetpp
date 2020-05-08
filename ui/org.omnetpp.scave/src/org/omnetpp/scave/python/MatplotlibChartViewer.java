/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/
package org.omnetpp.scave.python;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MessageBox;
import org.omnetpp.common.image.ImageUtils;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.pychart.ActionDescription;
import org.omnetpp.scave.pychart.IMatplotlibFigureCanvas;
import org.omnetpp.scave.pychart.IMatplotlibWidget;
import org.omnetpp.scave.pychart.IPlotWidgetProvider;
import org.omnetpp.scave.pychart.MatplotlibWidget;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
import org.omnetpp.scave.pychart.PythonProcess;
import org.omnetpp.scave.pychart.PythonProcessPool;

/**
 * Displays a chart in a Matplotlib plot.
 */
public class MatplotlibChartViewer extends ChartViewerBase {

    private int figureNumber = -1;
    private MatplotlibWidget plotWidget;
    private String lastActiveAction = "";

    public interface IStateChangeListener {
        void activeActionChanged(String action);
        void pythonProcessLivenessChanged(PythonProcess proc);
    }

    IPlotWidgetProvider widgetProvider = new IPlotWidgetProvider() {

        @Override
        public void updateActiveAction(int figureNumber, String action) {
            for (IStateChangeListener l : stateChangeListeners)
                l.activeActionChanged(action);
        }

        @Override
        public void setPlotActions(int figureNumber, List<ActionDescription> actions) {
            // we ignore these at the moment, we should assert that the hard-coded actions we support are there
        }

        @Override
        public void setWarning(String warning) {
            if (plotWidget != null)
                plotWidget.setWarning(warning);
        }

        @Override
        public IMatplotlibWidget getWidget(int figureNumber, IMatplotlibFigureCanvas canvas) {
            if (MatplotlibChartViewer.this.figureNumber >= 0 && figureNumber != MatplotlibChartViewer.this.figureNumber)
                throw new RuntimeException("Only one figure per chart is allowed - figure number 1.");

            Display.getDefault().syncExec(() -> {
                MatplotlibChartViewer.this.figureNumber = figureNumber;
                plotWidget.rebindToNewProcess(proc, canvas);
                plotWidget.figureNumber = figureNumber;
            });

            return plotWidget;
        }
    };

    public MatplotlibChartViewer(Composite parent, Chart chart, PythonProcessPool processPool, ResultFileManager rfm) {
        super(chart, processPool, rfm);
        plotWidget = new MatplotlibWidget(parent, SWT.DOUBLE_BUFFERED, proc, null);
    }

    public void runPythonScript(String script, File workingDir, Runnable runAfterDone, ExceptionHandler runAfterError) {
        if (plotWidget.isDisposed())
            return;

        final List<Object> limits = new ArrayList<Object>();
        try {
            if (getPythonProcess() != null && getPythonProcess().isAlive() && plotWidget.getCanvas() != null)
                limits.addAll(plotWidget.getCanvas().getAxisLimits());
        }
        catch (Exception e) {
            // ignore, since this is mostly a convenience feature
        }

        killPythonProcess();
        plotWidget.setWarning(null);

        if (script == null || script.isEmpty()) {
            plotWidget.setMessage("No Python script given");
            return;
        }
        try {
            acquireNewProcess();
            proc.getEntryPoint().setPlotWidgetProvider(widgetProvider);
        }
        catch (RuntimeException e) {
            MessageBox mb = new MessageBox(Display.getCurrent().getActiveShell(), SWT.ICON_ERROR);
            mb.setMessage(e.getMessage());
            mb.open();
            return;
        }

        ExceptionHandler ownRunAfterError = (proc, e) -> {
            runAfterError.handle(proc, e);
            plotWidget.setWarning("An exception occurred during Python execution.");
        };

        Runnable ownRunAfterDone = () -> {
            runAfterDone.run();
            try {
                if (getPythonProcess() != null && getPythonProcess().isAlive() && plotWidget.getCanvas() != null && limits != null)
                    plotWidget.getCanvas().setAxisLimits(limits);
            } catch (Exception e) {
                // ignore, since this is mostly a convenience feature
            }
            try {
                if (getPythonProcess() != null && getPythonProcess().isAlive() && plotWidget.getCanvas() != null && !StringUtils.isBlank(lastActiveAction)) {
                    plotWidget.getCanvas().performAction(lastActiveAction);
                }
            } catch (Exception e) {
                // ignore, since this is mostly a convenience feature
            }

        };

        proc.pythonCallerThread.asyncExec(() -> {
            changePythonIntoDirectory(workingDir);
            proc.getEntryPoint().execute(script);
        }, ownRunAfterDone, ownRunAfterError);
    }

    @Override
    public Control getWidget() {
        return plotWidget;
    }

    public void setVisible(boolean visible) {
        plotWidget.setVisible(visible);
    }

    public MatplotlibWidget getPlotWidget() {
        return plotWidget;
    }

    public void interact() {
        if (!lastActiveAction.isEmpty())
            plotWidget.getCanvas().performAction(lastActiveAction);
        lastActiveAction = "";
    }

    public void zoom() {
        if (!"zoom".equals(lastActiveAction))
            plotWidget.getCanvas().performAction("zoom");
        lastActiveAction = "zoom";
    }

    public void pan() {
        if (!"pan".equals(lastActiveAction))
            plotWidget.getCanvas().performAction("pan");
        lastActiveAction = "pan";
    }

    public void home() {
        plotWidget.getCanvas().performAction("home");
    }

    public void back() {
        plotWidget.getCanvas().performAction("back");
    }

    public void forward() {
        plotWidget.getCanvas().performAction("forward");
    }

    public boolean isSaveImagePossible() {
        return plotWidget.getCanvas() != null;
    }

    @Override
    public void saveImage(String filename) {
        IMatplotlibFigureCanvas canvas = plotWidget.getCanvas();
        if (canvas != null)
            canvas.exportFigure(filename);
    }

    public Map<String, ArrayList<String>> getSupportedFiletypes() {
        return plotWidget.getCanvas().getSupportedFiletypes();
    }

    public String getDefaultFiletype() {
        return plotWidget.getCanvas().getDefaultFiletype();
    }

    @Override
    public void copyImageToClipboard() {
        Clipboard cp = java.awt.Toolkit.getDefaultToolkit().getSystemClipboard();
        ClipboardOwner owner = new java.awt.datatransfer.ClipboardOwner() {
            public void lostOwnership(Clipboard clipboard, Transferable contents) {
            }
        };

        class ImageTransferable implements Transferable {
            public java.awt.Image image;

            public ImageTransferable(java.awt.Image image) {
                this.image = image;
            }

            public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException, IOException {
                if (flavor == DataFlavor.imageFlavor)
                    return image;
                else
                    throw new UnsupportedFlavorException(flavor);
            }

            public DataFlavor[] getTransferDataFlavors() {
                return new DataFlavor[] {DataFlavor.imageFlavor};
            }

            public boolean isDataFlavorSupported(DataFlavor flavor) {
                return flavor == DataFlavor.imageFlavor;
            }
        };

        Image image = plotWidget.getImage();
        cp.setContents(new ImageTransferable(ImageUtils.convertToAWT(image)), owner);
    }

    @Override
    public void dispose() {
        super.dispose();

        if (plotWidget != null)
            plotWidget.dispose();
    }
}
