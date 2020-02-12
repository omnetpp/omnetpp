package org.omnetpp.scave.python;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.image.ImageUtils;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.pychart.ActionDescription;
import org.omnetpp.scave.pychart.IMatplotlibWidget;
import org.omnetpp.scave.pychart.IPlotWidgetProvider;
import org.omnetpp.scave.pychart.IMatplotlibFigureCanvas;
import org.omnetpp.scave.pychart.MatplotlibWidget;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
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
        void pythonProcessLivenessChanged(boolean alive);
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

        ExceptionHandler ownRunAfterError = (proc, e ) -> {
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


    protected static String askFileName(String[] names, String[] extensions, int filterIndex, String fileName, Shell shell) {

        FileDialog dialog = new FileDialog(shell, SWT.SAVE);
        dialog.setFilterNames(names);
        dialog.setFilterExtensions(extensions);
        dialog.setFilterIndex(filterIndex);
        dialog.setFileName(fileName);

        String filename = dialog.open();

        if (filename != null) {
            File file = new File(filename);
            if (file.exists()) {
                MessageBox messageBox = new MessageBox(shell,
                        SWT.YES | SWT.NO | SWT.APPLICATION_MODAL | SWT.ICON_WARNING);
                messageBox.setText("File already exists");
                messageBox.setMessage("The file " + filename
                        + " already exists and will be overwritten. Do you want to continue the operation?");
                if (messageBox.open() == SWT.NO)
                    filename = null;
            }
        }

        return filename;
    }

    public void export(String name) {
        IMatplotlibFigureCanvas canvas = plotWidget.getCanvas();

        if (canvas != null) {
            HashMap<String, ArrayList<String>> types = canvas.getSupportedFiletypes();

            String defaultType = canvas.getDefaultFiletype();
            int filterIndex = 0;

            String[] names = new String[types.keySet().size()];
            Object[] ks = types.keySet().toArray();
            for (int i = 0; i < ks.length; ++i)
                names[i] = (String) ks[i];

            String[] extensions = new String[names.length];

            for (int i = 0; i < names.length; ++i) {
                String filter = "";

                ArrayList<String> exts = types.get(names[i]);
                for (String e : exts) {
                    if (e.equals(defaultType))
                        filterIndex = i;
                    filter += ("*." + e + ";");
                }

                extensions[i] = filter.substring(0, filter.length() - 1);
                names[i] += " (" + extensions[i].replace(";", ", ") + ")";
            }

            String filename = askFileName(names, extensions, filterIndex, name, plotWidget.getShell());

            if (filename != null) {
                String error = canvas.exportFigure(filename);
                if (!error.isEmpty()) {
                    MessageBox messageBox = new MessageBox(plotWidget.getShell(), SWT.OK | SWT.APPLICATION_MODAL | SWT.ICON_ERROR);
                    messageBox.setText("Error exporting image");
                    messageBox.setMessage("Error: " + error);
                    messageBox.open();
                }
            }
        }
    }

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

    public void dispose() {
        super.dispose();

        if (plotWidget != null)
            plotWidget.dispose();
    }
}
