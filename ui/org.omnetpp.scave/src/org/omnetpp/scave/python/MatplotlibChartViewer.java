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

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.image.ImageUtils;
import org.omnetpp.scave.pychart.ActionDescription;
import org.omnetpp.scave.pychart.IPlotWidget;
import org.omnetpp.scave.pychart.IPlotWidgetProvider;
import org.omnetpp.scave.pychart.IPyFigureCanvas;
import org.omnetpp.scave.pychart.PlotWidget;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
import org.omnetpp.scave.pychart.PythonProcessPool;

public class MatplotlibChartViewer extends ChartViewerBase {

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
        public IPlotWidget getWidget(int figureNumber, IPyFigureCanvas canvas) {
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

    int figureNumber = -1;
    private PlotWidget plotWidget;
    String lastActiveAction = "";

    public MatplotlibChartViewer(PythonProcessPool processPool, Composite parent) {
        super(processPool);
        plotWidget = new PlotWidget(parent, SWT.DOUBLE_BUFFERED, proc, null);
    }

    public void runPythonScript(String script, File workingDir, Runnable runAfterDone, ExceptionHandler runAfterError) {
        if (plotWidget.isDisposed())
            return;

        killPythonProcess();

        if (script == null || script.isEmpty()) {
            plotWidget.setMessage("No Python script given");
            return;
        }

        acquireNewProcess();
        proc.getEntryPoint().setPlotWidgetProvider(widgetProvider);

        lastActiveAction = "";
        for (IStateChangeListener l : stateChangeListeners)
            l.activeActionChanged("");

        ExceptionHandler ownRunAfterError = (proc, e ) -> {
            runAfterError.handle(proc, e);
            plotWidget.setMessage("An exception occurred during Python execution.");
        };

        proc.pythonCallerThread.asyncExec(() -> {
            changePythonIntoDirectory(workingDir);
            proc.getEntryPoint().execute(script);
        }, runAfterDone, ownRunAfterError);
    }

    @Override
    public Control getWidget() {
        return plotWidget;
    }

    public void setVisible(boolean visible) {
        plotWidget.setVisible(visible);
    }

    public PlotWidget getPlotWidget() {
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
        IPyFigureCanvas canvas = plotWidget.getCanvas();

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
