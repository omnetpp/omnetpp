package org.omnetpp.scave.python;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import org.omnetpp.scave.pychart.IChartPropertiesProvider;
import org.omnetpp.scave.pychart.IScaveResultsPickleProvider;
import org.omnetpp.scave.pychart.PythonProcess;
import org.omnetpp.scave.pychart.PythonProcessPool;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
import org.omnetpp.scave.pychart.PythonOutputMonitoringThread.IOutputListener;
import org.omnetpp.scave.python.MatplotlibChartViewer.IStateChangeListener;

public abstract class ChartViewerBase {

    protected PythonProcess proc = null;
    protected PythonProcessPool processPool;

    protected List<IOutputListener> outputListeners = new ArrayList<IOutputListener>();
    protected List<IStateChangeListener> stateChangeListeners = new ArrayList<IStateChangeListener>();

    private IScaveResultsPickleProvider resultsProvider = null;
    private IChartPropertiesProvider propertiesProvider = null;

    public ChartViewerBase(PythonProcessPool processPool) {
        this.processPool = processPool;
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

    public void setResultsProvider(IScaveResultsPickleProvider resultsProvider) {
        this.resultsProvider = resultsProvider;
    }

    public void setChartPropertiesProvider(IChartPropertiesProvider propertiesProvider) {
        this.propertiesProvider = propertiesProvider;
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

        proc.getEntryPoint().setResultsProvider(resultsProvider);
        proc.getEntryPoint().setChartPropertiesProvider(propertiesProvider);
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

    public abstract void setVisible(boolean visible);

}
