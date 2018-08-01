package org.omnetpp.scave.pychart;

import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.Map;

import py4j.ClientServer;

public class PythonProcessPool {

    ArrayList<PythonProcess> availableProcesses = new ArrayList<PythonProcess>();

    String errorMessage = null;

    boolean threadExit = false;
    Thread launcherThread;

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
                    System.out.println("We have " + availableProcesses.size() + " processes.");
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

    private PythonProcess createProcess() throws IOException {
        System.out.println("connecting...");
        ClientServer clientServer = new ClientServer.ClientServerBuilder().javaPort(0).pythonPort(0).readTimeout(1000)
                .build();

        int javaPort = clientServer.getJavaServer().getListeningPort();
        System.out.println("listening port in Java: " + javaPort);

        // This only worked with the internal test app, and not when used from within the IDE.
        //String location = PythonProcessPool.class.getResource("python").getPath();

        String location = "NULL";
        try {
            location = new File(PythonProcessPool.class.getProtectionDomain().getCodeSource().getLocation().toURI().getPath() + "src/org/omnetpp/scave/pychart/python").getAbsolutePath();
        }
        catch (URISyntaxException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        String location2 = location.replaceAll("/src/org/omnetpp/scave/pychart/python$", "/org/omnetpp/scave/pychart/python");

        String pythonPath = location + File.pathSeparator + location2;

        ProcessBuilder pb = new ProcessBuilder()
                .command("python3", "-m", "PythonEntryPoint", Integer.toString(javaPort))
                // .directory(new File(location))
                .redirectErrorStream(true).redirectOutput(ProcessBuilder.Redirect.PIPE)
                .redirectInput(ProcessBuilder.Redirect.PIPE);

        Map<String, String> env = pb.environment();
        if (env.containsKey("PYTHONPATH"))
            env.put("PYTHONPATH", pythonPath + File.pathSeparator + env.get("PYTHONPATH"));
        else
            env.put("PYTHONPATH", pythonPath);

        System.out.println("starting python process... with path " + env.get("PYTHONPATH"));

        Process process = pb.start();

        return new PythonProcess(process, clientServer);
    }

    public void dispose() {
        threadExit = true;
        launcherThread.interrupt();
        for (PythonProcess pr : availableProcesses)
            pr.dispose();
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
