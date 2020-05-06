/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.pychart;

import java.util.Map;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ShmSendBufferManager;

import py4j.ClientServer;

/**
 * Encapsulates a python3 interpreter (running in a Process); a reference to an
 * IPythonEntryPoint implementation in it (through a Py4J connection -
 * ClientServer); and some more related machinery: A thread to make blocking
 * calls running Python code asynchronously, a thread to monitor output from the
 * python3 process' stdout and stderr (each), and an SHM SendBufferManager to
 * easily transfer bulk data from the native parts to Python - with its GC Thread.
 */
public class PythonProcess {
    public static boolean debug = Debug.isChannelEnabled("pythonprocess");

    private Process process;
    private ClientServer clientServer = null;
    private boolean killedByUs = false;

    protected InterruptedFlag interruptedFlag = new InterruptedFlag();

    public PythonOutputMonitoringThread outputMonitoringThread;
    public PythonOutputMonitoringThread errorMonitoringThread;
    public PythonCallerThread pythonCallerThread;

    protected IPythonEntryPoint entryPoint = null;
    protected ShmSendBufferManager shmSendBufferManager;

    private static Map<String, String> matplotlibRcParams; // stored result of a Matplotlib rcParams() call, mainly for content assist

    public PythonProcess(Process process, ClientServer clientServer) {
        this.process = process;
        this.clientServer = clientServer;

        outputMonitoringThread = new PythonOutputMonitoringThread(this, false);
        outputMonitoringThread.start();
        errorMonitoringThread = new PythonOutputMonitoringThread(this, true);
        errorMonitoringThread.start();

        shmSendBufferManager = new ShmSendBufferManager((int)process.pid());

        // this does not have any references to anything, the Runnable
        // instances passed to it later do
        pythonCallerThread = new PythonCallerThread(this);
        pythonCallerThread.start();

        Thread collectorThread = new Thread(() -> {
            while (isAlive()) {
                try { Thread.sleep(1000); } catch (InterruptedException e) {}
                shmSendBufferManager.garbageCollect();
            }
        }, "ShmSendBufferManager garbage collector");
        collectorThread.setDaemon(true);
        collectorThread.start();

        process.onExit().thenRun(() -> {
            shmSendBufferManager.clear();
            pythonCallerThread.interrupt();
        });
    }

    public Process getProcess() {
        return process;
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
            String output = "STDOUT:\n\n" + outputMonitoringThread.getOutputSoFar() +
                    "\n\nSTDERR:\n\n" + errorMonitoringThread.getOutputSoFar();
            if (debug) {
                Debug.println("Couldn't get the entry point to the Python process... :(");
                Debug.println("Python said:\n" + output);
            }
            throw new RuntimeException("Couldn't get the entry point to the Python process.\nIts output so far:\n" + output);
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

    public ShmSendBufferManager getShmSendBufferManager() {
        return shmSendBufferManager;
    }

    public static Map<String, String> getMatplotlibRcParams() {
        return matplotlibRcParams;
    }

    public static void setMatplotlibParams(Map<String, String> rcParams) {
        matplotlibRcParams = rcParams;
    }

}