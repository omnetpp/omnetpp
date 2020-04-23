package org.omnetpp.scave.pychart;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

import org.omnetpp.common.Debug;

public class PythonOutputMonitoringThread extends Thread {
    public Process process;
    public boolean monitorStdErr;

    // has to be here to be "effectively final"...
    private Integer numBytesRead = 0;

    private StringBuffer outputSoFar = new StringBuffer();
    private static final int OUTPUT_BUFFER_SIZE_MAX = 4096;

    public interface IOutputListener {
        void outputReceived(String content, boolean stdErr);
    }

    List<IOutputListener> outputListeners = new ArrayList<IOutputListener>();
    List<Runnable> deathListeners = new ArrayList<Runnable>();

    public PythonOutputMonitoringThread(Process process, boolean monitorStdErr) {
        super("Python output monitoring");
        this.process = process;
        this.monitorStdErr = monitorStdErr;
    }

    public void addDeathListener(Runnable listener) {
        deathListeners.add(listener);
    }

    public void addOutputListener(IOutputListener listener) {
        outputListeners.add(listener);
    }

    @Override
    public void run() {
        byte[] readBuffer = new byte[4096];

        try (InputStream inputStream = monitorStdErr ? process.getErrorStream() : process.getInputStream()) {
            while (numBytesRead != -1) {
                numBytesRead = inputStream.read(readBuffer);
                if (numBytesRead >= 0) {
                    String content = new String(readBuffer, 0, numBytesRead);
                    for (IOutputListener l : outputListeners)
                        l.outputReceived(content, monitorStdErr);

                    if (outputSoFar.length() < OUTPUT_BUFFER_SIZE_MAX)
                        outputSoFar.append(content);

                }
            }
        }
        catch (IOException e) {
            PyChartPlugin.logError(e);
        }

        for (Runnable l : deathListeners)
            l.run();
    }

    public String getOutputSoFar() {
        return outputSoFar.toString() + ((outputSoFar.length() > OUTPUT_BUFFER_SIZE_MAX) ? " (truncated)" : "");
    }
}