package org.omnetpp.scave.pychart;

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

        System.out.println("getting entry point...");
        int tries = 0;
        boolean ok = false;
        while (!ok && tries < 5) {
            try {
                entryPoint = (IPythonEntryPoint) clientServer
                        .getPythonServerEntryPoint(new Class[] { IPythonEntryPoint.class });
                System.out.println("checking the entry point...");
                ++tries;
                ok = entryPoint.check();
                System.out.println("we have the entry point? " + ok);
            }
            catch (Exception e) {
                System.out.println("can't connect yet, trying again in a bit...");
                try {
                    Thread.sleep(500);
                }
                catch (InterruptedException e1) {
                    // TODO Auto-generated catch block
                    e1.printStackTrace();
                }
            }
        }

        if (!ok) {
            entryPoint = null;
            System.out.println("Couldn't get the entry point to the Python process... :(");
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