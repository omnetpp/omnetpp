package org.omnetpp.scave.pychart;

import org.omnetpp.common.Debug;

import py4j.ClientServer;

public class PythonProcess {
    private Process process;
    private ClientServer clientServer = null;

    public PythonOutputMonitoringThread outputMonitoringThread;
    public PythonCallerThread pythonCallerThread;

    IPythonEntryPoint entryPoint = null;

    public PythonProcess(Process process, ClientServer clientServer) {
        this.process = process;
        this.clientServer = clientServer;

        outputMonitoringThread = new PythonOutputMonitoringThread(process);
        outputMonitoringThread.start();

        // this does not have any references to anything, the Runnable
        // instances passed to it later do
        pythonCallerThread = new PythonCallerThread();
        pythonCallerThread.start();
    }

    public IPythonEntryPoint getEntryPoint() {
        if (entryPoint != null)
            return entryPoint;

        Debug.println("getting entry point...");
        int tries = 0;
        boolean ok = false;
        while (!ok && tries < 5) {
            try {
                entryPoint = (IPythonEntryPoint) clientServer
                        .getPythonServerEntryPoint(new Class[] { IPythonEntryPoint.class });
                Debug.println("checking the entry point...");
                ++tries;
                ok = entryPoint.check();
                Debug.println("we have the entry point? " + ok);
            }
            catch (Exception e) {
                Debug.println("can't connect yet, trying again in a bit...");
                try {
                    Thread.sleep(500);
                }
                catch (InterruptedException e1) {
                    PyChartPlugin.logError(e1);
                }
            }
        }

        if (!ok) {
            entryPoint = null;
            Debug.println("Couldn't get the entry point to the Python process... :(");
            Debug.println("Python said:" + outputMonitoringThread.outputSoFar);
            throw new RuntimeException("Couldn't get the entry point to the Python process.\nIts output so far:\n" + outputMonitoringThread.getOutputSoFar());
        }

        return entryPoint;
    }

    public void dispose() {
        if (process != null) {
            clientServer.shutdown();
            process.destroyForcibly();
            process = null;
        }
    }

    public boolean isDisposed() {
        return process == null || !process.isAlive();
    }
}