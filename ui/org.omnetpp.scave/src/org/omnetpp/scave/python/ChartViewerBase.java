package org.omnetpp.scave.python;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.widgets.Control;
import org.omnetpp.scave.editors.PropertiesProvider;
import org.omnetpp.scave.editors.ResultsProvider;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
import org.omnetpp.scave.pychart.PythonOutputMonitoringThread.IOutputListener;
import org.omnetpp.scave.pychart.PythonProcess;
import org.omnetpp.scave.pychart.PythonProcessPool;
import org.omnetpp.scave.python.MatplotlibChartViewer.IStateChangeListener;

public abstract class ChartViewerBase {

    protected Chart chart;
    protected ResultFileManager rfm;

    protected PythonProcess proc = null;
    protected PythonProcessPool processPool;

    protected List<IOutputListener> outputListeners = new ArrayList<IOutputListener>();
    protected List<IStateChangeListener> stateChangeListeners = new ArrayList<IStateChangeListener>();

    public ChartViewerBase(PythonProcessPool processPool, Chart chart, ResultFileManager rfm) {
        this.processPool = processPool;
        this.chart = chart;
        this.rfm = rfm;
    }

    public void killPythonProcess() {
        if (proc != null && proc.isAlive())
            proc.kill();
    }

    public boolean isAlive() {
        return proc != null && proc.isAlive();
    }

    public void addOutputListener(IOutputListener listener) {
        outputListeners.add(listener);
    }

    public void addStateChangeListener(IStateChangeListener listener) {
        stateChangeListeners.add(listener);
    }

    protected void acquireNewProcess() {
        killPythonProcess();

        proc = processPool.getProcess();

        for (MatplotlibChartViewer.IStateChangeListener l : stateChangeListeners)
            l.pythonProcessLivenessChanged(true);

        proc.outputMonitoringThread.addDeathListener(() -> {
            for (MatplotlibChartViewer.IStateChangeListener l : stateChangeListeners)
                l.pythonProcessLivenessChanged(false);
        });

        for (IOutputListener l : outputListeners) {
            proc.outputMonitoringThread.addOutputListener(l);
            proc.errorMonitoringThread.addOutputListener(l);
        }

        proc.getEntryPoint().setResultsProvider(new ResultsProvider(rfm, proc.getInterruptedFlag()));
        proc.getEntryPoint().setChartPropertiesProvider(new PropertiesProvider(chart));
    }

    protected void changePythonIntoDirectory(File workingDir) {
        proc.getEntryPoint().execute("import os; os.chdir(\"\"\"" + workingDir.getAbsolutePath() + "\"\"\"); del os;");
        proc.getEntryPoint().execute("import site; site.addsitedir(\"\"\"" + workingDir.getAbsolutePath() + "\"\"\"); del site;");
    }

    public PythonProcess getPythonProcess() {
        return proc;
    }

    public void dispose() {
        killPythonProcess();
    }

    public abstract void runPythonScript(String script, File workingDir, Runnable runAfterDone, ExceptionHandler runAfterError);

    public abstract Control getWidget();

    public abstract void setVisible(boolean visible);


}
