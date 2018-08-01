package org.omnetpp.scave.pychart;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

public class PythonOutputMonitoringThread extends Thread {
    public Process process;

    // has to be here to be "effectively final"...
    Integer numBytesRead = 0;

    String outputSoFar = "";

    public interface IOutputListener {
        void outputReceived(String content);
    }

    List<IOutputListener> outputListeners = new ArrayList<IOutputListener>();

    public void addOutputListener(IOutputListener listener) {
        outputListeners.add(listener);
    }

    public PythonOutputMonitoringThread(Process process) {
        super("Python output monitoring");
        this.process = process;
    }

    @Override
    public void run() {
        byte[] readBuffer = new byte[4096];
        InputStream inputStream = process.getInputStream();

        try {
            while (numBytesRead != -1) {
                numBytesRead = inputStream.read(readBuffer);
                if (numBytesRead >= 0) {
                    String content = new String(readBuffer, 0, numBytesRead);
                    for (IOutputListener l : outputListeners)
                        l.outputReceived(content);
                    outputSoFar += content;
                }
            }
        }
        catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        // TODO: notify PythonProcess of Process death?
    }

    public String getOutputSoFar() {
        return outputSoFar;
    }
}