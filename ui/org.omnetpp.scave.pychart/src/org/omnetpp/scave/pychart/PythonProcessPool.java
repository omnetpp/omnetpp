package org.omnetpp.scave.pychart;

import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.Map;

import org.omnetpp.common.Debug;
import org.omnetpp.common.OmnetppDirs;

import py4j.ClientServer;

public class PythonProcessPool {
    private boolean shouldSetOmnetppMplBackend = true;
    private ArrayList<PythonProcess> availableProcesses = new ArrayList<PythonProcess>();
    private String errorMessage = null;
    private boolean threadExit = false;
    private Thread launcherThread;

    public PythonProcessPool() {
        this(1);
    }

    public PythonProcessPool(int N) {

        launcherThread = new Thread(() -> {
            while (!threadExit) {
                if (availableProcesses.size() < N) {
                    try {
                        availableProcesses.add(createProcess());
                    }
                    catch (IOException e) {
                        errorMessage = e.getMessage();
                        threadExit = true;
                    }
                    if (PythonProcess.debug)
                        Debug.println("We have " + availableProcesses.size() + " processes.");
                }
                try {
                    Thread.sleep(1000);
                }
                catch (InterruptedException e) {
                    // need to create a process right away - most likely
                }
            }
        });

        launcherThread.setDaemon(true);
        launcherThread.start();
    }

    public void setShouldSetOmnetppMplBackend(boolean value) {
        this.shouldSetOmnetppMplBackend = value;
    }

    private PythonProcess createProcess() throws IOException {
        if (PythonProcess.debug)
            Debug.println("connecting...");

        ClientServer clientServer = new ClientServer.ClientServerBuilder().javaPort(0).pythonPort(0).readTimeout(1000).build();

        int javaPort = clientServer.getJavaServer().getListeningPort();
        if (PythonProcess.debug)
            Debug.println("listening port in Java: " + javaPort);

        ProcessBuilder pb = new ProcessBuilder()
                .command("python3", "-u", "-m", "omnetpp.internal.PythonEntryPoint", Integer.toString(javaPort))
                .redirectError(ProcessBuilder.Redirect.PIPE).redirectOutput(ProcessBuilder.Redirect.PIPE)
                .redirectInput(ProcessBuilder.Redirect.PIPE);

        Map<String, String> env = pb.environment();
        env.put("WITHIN_OMNETPP_IDE", "yes");
        env.put("PYTHONPATH", extendPythonPath(env.get("PYTHONPATH")));
        if (shouldSetOmnetppMplBackend)
            env.put("MPLBACKEND", "module://omnetpp.internal.backend_SWTAgg");

        if (PythonProcess.debug)
            Debug.println("starting python process... with path " + env.get("PYTHONPATH"));

        Process process = pb.start();

        return new PythonProcess(process, clientServer);
    }

    private String extendPythonPath(String oldPythonPath) {
        String pychartPluginLocation  = PyChartPlugin.getDefault().getBundle().getLocation();
        String locationsToPrepend = OmnetppDirs.getOmnetppPythonDir() + File.pathSeparator // the <omnetpp_root>/python dir
                + pychartPluginLocation + File.separator + "python"; // the plugin-local python dir

        return oldPythonPath == null ? locationsToPrepend : locationsToPrepend + File.pathSeparator + oldPythonPath;
    }

    public void dispose() {
        threadExit = true;
        launcherThread.interrupt();
        for (PythonProcess pr : availableProcesses)
            pr.kill();
        availableProcesses.clear();
    }

    public PythonProcess getProcess() {

        while (availableProcesses.isEmpty()) {
            launcherThread.interrupt();
            // TODO: make sure to wait for a process to become available

            if (errorMessage != null)
                throw new RuntimeException(errorMessage);
        }

        PythonProcess pr = availableProcesses.remove(0);
        // waking up the launcher to replenish the supply of processes
        launcherThread.interrupt();

        return pr;
    }
}
