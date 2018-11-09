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
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.image.ImageUtils;
import org.omnetpp.scave.pychart.ActionDescription;
import org.omnetpp.scave.pychart.IChartPropertiesProvider;
import org.omnetpp.scave.pychart.IPlotWidget;
import org.omnetpp.scave.pychart.IPlotWidgetProvider;
import org.omnetpp.scave.pychart.IPyFigureCanvas;
import org.omnetpp.scave.pychart.IScaveResultsPickleProvider;
import org.omnetpp.scave.pychart.PlotWidget;
import org.omnetpp.scave.pychart.PythonProcess;
import org.omnetpp.scave.pychart.PythonProcessPool;
import org.omnetpp.scave.pychart.PythonOutputMonitoringThread.IOutputListener;

import py4j.Py4JException;

public class MatplotlibChartViewer {

    public interface IPy4JExceptionHandler {
        void handle(Py4JException e);
    }

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

    private PythonProcess proc = null;

    int figureNumber = -1;
    private PlotWidget plotWidget;

    private PythonProcessPool processPool;

    String lastActiveAction = "";

    List<IOutputListener> outputListeners = new ArrayList<IOutputListener>();
    List<IStateChangeListener> stateChangeListeners = new ArrayList<IStateChangeListener>();

    IScaveResultsPickleProvider resultsProvider = null;
    IChartPropertiesProvider propertiesProvider = null;


    public MatplotlibChartViewer(PythonProcessPool processPool, Composite parent) {
        this.processPool = processPool;

        proc = null;
        plotWidget = new PlotWidget(parent, SWT.DOUBLE_BUFFERED, proc, null);
    }

    public void runPythonScript(String script, File workingDir, Runnable runAfterDone, IPy4JExceptionHandler runAfterError) {
        if (proc != null)
            proc.dispose();

        proc = processPool.getProcess();

        for (IStateChangeListener l : stateChangeListeners) {
            l.pythonProcessLivenessChanged(true);
            l.activeActionChanged("");
            lastActiveAction = "";
        }

        proc.getEntryPoint().setPlotWidgetProvider(widgetProvider);
        proc.getEntryPoint().setResultsProvider(resultsProvider);
        proc.getEntryPoint().setChartPropertiesProvider(propertiesProvider);

        for (IOutputListener l : outputListeners)
            proc.outputMonitoringThread.addOutputListener(l);

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

                if (runAfterDone != null)
                    runAfterDone.run();
            });
        }
    }

    public boolean isAlive() {
        return proc != null && !proc.isDisposed();
    }

    public void setVisible(boolean visible) {
        plotWidget.setVisible(visible);
    }

    public PlotWidget getPlotWidget() {
        return plotWidget;
    }

    public void addOutputListener(IOutputListener listener) {
        outputListeners.add(listener);
    }

    public void addStateChangeListener(IStateChangeListener listener) {
        stateChangeListeners.add(listener);
    }

    public void setResultsProvider(IScaveResultsPickleProvider resultsProvider) {
        this.resultsProvider = resultsProvider;
    }

    public void setChartPropertiesProvider(IChartPropertiesProvider propertiesProvider) {
        this.propertiesProvider = propertiesProvider;
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

    public void copyToClipboard() {
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

    public void killPythonProcess() {
        if (proc != null && !proc.isDisposed())
            proc.dispose();
    }

    public void dispose() {
        killPythonProcess();

        if (plotWidget != null)
            plotWidget.dispose();
    }
}
