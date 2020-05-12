/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.pychart;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

import org.omnetpp.common.Debug;

public class PythonOutputMonitoringThread extends Thread {
    public PythonProcess process;
    public boolean monitorStdErr;

    // has to be here to be "effectively final"...
    private Integer numBytesRead = 0;

    private StringBuffer outputSoFar = new StringBuffer();
    private static final int OUTPUT_BUFFER_SIZE_MAX = 4096;

    public interface IOutputListener {
        void outputReceived(String content, boolean stdErr);
    }

    List<IOutputListener> outputListeners = new ArrayList<IOutputListener>();

    public PythonOutputMonitoringThread(PythonProcess process, boolean monitorStdErr) {
        super("Python output monitoring for " + (monitorStdErr ? "stderr" : "stdout") + " of PythonProcess " + process.getProcess().pid());
        this.process = process;
        this.monitorStdErr = monitorStdErr;
    }

    public void addOutputListener(IOutputListener listener) {
        outputListeners.add(listener);
    }

    @Override
    public void run() {
        byte[] readBuffer = new byte[4096];

        try (InputStream inputStream = monitorStdErr ? process.getProcess().getErrorStream() : process.getProcess().getInputStream()) {
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

        Debug.println((monitorStdErr ? "stderr" : "stdout") + " stream of PID " + process.getProcess().pid() + " was closed");
    }

    public String getOutputSoFar() {
        return outputSoFar.toString() + ((outputSoFar.length() > OUTPUT_BUFFER_SIZE_MAX) ? " (truncated)" : "");
    }
}