package org.omnetpp.scave.pychart;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.InterruptedFlag;

import py4j.ClientServer;

public class PythonProcess {
    public static boolean debug = Debug.isChannelEnabled("pythonprocess");

    private Process process;
    private ClientServer clientServer = null;
    boolean killedByUs = false;

    protected InterruptedFlag interruptedFlag = new InterruptedFlag();

    public PythonOutputMonitoringThread outputMonitoringThread;
    public PythonOutputMonitoringThread errorMonitoringThread;
    public PythonCallerThread pythonCallerThread;

    IPythonEntryPoint entryPoint = null;

    public PythonProcess(Process process, ClientServer clientServer) {
        this.process = process;
        this.clientServer = clientServer;

        outputMonitoringThread = new PythonOutputMonitoringThread(process, false);
        outputMonitoringThread.start();
        errorMonitoringThread = new PythonOutputMonitoringThread(process, true);
        errorMonitoringThread.start();

        // this does not have any references to anything, the Runnable
        // instances passed to it later do
        pythonCallerThread = new PythonCallerThread(this);
        pythonCallerThread.start();
    }

    public IPythonEntryPoint getEntryPoint() {
        if (entryPoint != null)
            return entryPoint;

        if (debug)
            Debug.println("getting entry point...");
        int tries = 0;
        boolean ok = false;
        while (!ok && tries < 5) {
            try {
                entryPoint = (IPythonEntryPoint) clientServer
                        .getPythonServerEntryPoint(new Class[] { IPythonEntryPoint.class });
                if (debug)
                    Debug.println("checking the entry point...");
                ++tries;
                ok = entryPoint.check();
                if (debug)
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
            if (debug) {
                Debug.println("Couldn't get the entry point to the Python process... :(");
                Debug.println("Python said:" + outputMonitoringThread.outputSoFar + errorMonitoringThread.outputSoFar);
            }
            throw new RuntimeException("Couldn't get the entry point to the Python process.\nIts output so far:\n" + outputMonitoringThread.getOutputSoFar() + errorMonitoringThread.getOutputSoFar());
        }

        return entryPoint;
    }

    public void kill() {
        if (process != null) {
            interruptedFlag.setFlag(true);
            clientServer.shutdown();
            process.destroyForcibly();
            killedByUs = true;
        }
    }

    public boolean isAlive() {
        return process != null && process.isAlive();
    }

    public boolean isKilledByUs() {
        return killedByUs;
    }

    @Override
    public String toString() {
        return "Python process: " + process + " isAlive: " + isAlive() + " killed by us: " + killedByUs + " exitCode: " + (process.isAlive() ? "none" : Integer.toString(process.exitValue()));
    }

    public InterruptedFlag getInterruptedFlag() {
        return interruptedFlag;
    }

}