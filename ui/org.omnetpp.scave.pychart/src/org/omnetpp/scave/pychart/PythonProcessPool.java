/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.pychart;

import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.Map;

import org.omnetpp.common.Debug;

import py4j.ClientServer;

/**
 * This class keeps a (configurable) number of PythonProcess instances in a pool,
 * ready to use. Each of them is a running python3 interpreter process, with a
 * Py4J connection to it. This was added to reduce the latency of repeated chart
 * script executions.
 */
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
        String locationsToPrepend = null;

        try {
            // This only worked with the internal test app, and not when used from within the IDE.
            //String pychartPluginLocation = PythonProcessPool.class.getResource("python").getPath();

            // This had a "file:" and/or "resource:" prefix, was not absolute in a built release (on Windows?),
            // and would have had to be fixed differently on macOS (because of the opp_ide.app/Contents/Eclipse directory)
            //String pychartPluginLocation = PyChartPlugin.getDefault().getBundle().getLocation();

            // This is ugly as all heck, but at least seems to work fine everywhere, everytime
            String codeSourcePath = getClass().getProtectionDomain().getCodeSource().getLocation().toURI().getPath();
            String pychartPluginLocation = new File(codeSourcePath).getAbsolutePath();

            locationsToPrepend = pychartPluginLocation + File.separator + "python"; // the plugin-local python dir
            locationsToPrepend = new File(locationsToPrepend).getCanonicalPath();

            // Part of the Python library is in <omnetpp>/python. While developing, we add this
            // directory to PYTHONPATH. In a release, the content of this directory is copied
            // into the plugin, so we don't need to add an extra PYTHONPATH entry for it.
            // In fact, adding that would be undesirable, since then the IDE would not be entirely
            // self-sufficient - it would fundamentally rely on files outside of the <root>/ide folder.
            if (Debug.inDevelopment()) {
                // OmnetppDirs.getOmnetppPythonDir() cannot be used, because the user can change it to
                // whatever they like, and that may not be compatible with the internal Python parts

                // the python directory in the source root
                String rootPythonDir = new File(pychartPluginLocation + "/../../python").getCanonicalPath();
                locationsToPrepend += File.pathSeparator + rootPythonDir;
            }
        }
        catch (URISyntaxException | IOException e) {
            // I don't think this can happen, but just in case:
            throw new RuntimeException(e);
        }

        return (oldPythonPath == null || oldPythonPath.isEmpty()) ? locationsToPrepend : locationsToPrepend + File.pathSeparator + oldPythonPath;
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
