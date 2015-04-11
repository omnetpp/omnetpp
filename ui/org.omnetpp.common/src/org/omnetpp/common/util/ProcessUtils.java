/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.util.concurrent.CancellationException;

import org.eclipse.core.runtime.IProgressMonitor;

/**
 * Helpful utility class to spawn a child process and provide a string as its standard input and
 * retrieve the standard output and errors written by the child process.
 *
 * @author levy
 */
public class ProcessUtils {
    public static class ProcessResult {
        public String standardOutput;
        public String standardError;
        public int exitValue;

        public ProcessResult(String standardOutput, String standardError, int exitValue) {
            this.standardOutput = standardOutput;
            this.standardError = standardError;
            this.exitValue = exitValue;
        }
    }

    public static class TimeoutException
        extends RuntimeException
    {
        public TimeoutException(String string) {
            super(string);
        }

        private static final long serialVersionUID = 1L;
    }

    public static class ExecException
        extends RuntimeException
    {
        public ProcessResult result;

        public ExecException(ProcessResult result, String string) {
            super(string);
            this.result = result;
        }

        private static final long serialVersionUID = 1L;
    }

    public static ProcessResult exec(String command, IProgressMonitor monitor) throws IOException, TimeoutException, ExecException, CancellationException {
        return exec(command, null, monitor);
    }

    public static ProcessResult exec(String command, String[] arguments, IProgressMonitor monitor) throws IOException, TimeoutException, ExecException, CancellationException {
        return exec(command, arguments, ".", monitor);
    }

    public static ProcessResult exec(String command, String[] arguments, String workingDirectory, IProgressMonitor monitor) throws IOException, TimeoutException, ExecException, CancellationException {
        return exec(command, arguments, workingDirectory, null, monitor);
    }

    public static ProcessResult exec(String command, String[] arguments, String workingDirectory, String standardInput, IProgressMonitor monitor) throws IOException, TimeoutException, ExecException, CancellationException {
        return exec(command, arguments, workingDirectory, standardInput, 0, monitor);
    }

    /**
     * Executes the given command with the arguments as a separate process.
     * The standard input is fed into the spawn process and the output is read until the process finishes or timeout occurs.
     * The timeout value 0 means wait infinitely long to finish the process.
     */
    public static ProcessResult exec(String command, String[] arguments, String workingDirectory, String standardInput, double timeout, IProgressMonitor monitor) throws IOException, TimeoutException, ExecException, CancellationException {
        String[] cmd = new String[arguments == null ? 1 : arguments.length + 1];
        cmd[0] = command;
        if (arguments != null)
            System.arraycopy(arguments, 0, cmd, 1, arguments.length);

        Process process = Runtime.getRuntime().exec(cmd, null, new File(workingDirectory));

        if (standardInput != null) {
            OutputStream outputStream = process.getOutputStream();
            outputStream.write(standardInput.getBytes());
            outputStream.close();
        }

        DataInputStream inputStream = new DataInputStream(process.getInputStream());
        DataInputStream errorStream = new DataInputStream(process.getErrorStream());
        ByteArrayOutputStream standardOutput = new ByteArrayOutputStream();
        ByteArrayOutputStream standardError = new ByteArrayOutputStream();

        long begin = System.currentTimeMillis();

        while (timeout == 0 || System.currentTimeMillis() - begin < timeout * 1000) {
            try {
                Thread.sleep(10);
            }
            catch (InterruptedException e) {
                // void, ignore
            }

            // read standard input
            byte[] data = new byte[inputStream.available()];
            inputStream.readFully(data);
            standardOutput.write(data);

            // read standard error
            data = new byte[errorStream.available()];
            errorStream.readFully(data);
            standardError.write(data);

            // check progress monitor
            if (monitor != null && monitor.isCanceled()) {
                process.destroy();
                throw new CancellationException();
            }

            int exitValue;

            try {
                exitValue = process.exitValue(); // throws exception if process is still alive
            }
            catch (Exception e) {
                continue; // still working
            }

            // process terminated
            ProcessResult result = new ProcessResult(standardOutput.toString(), standardError.toString(), exitValue);

            if (exitValue == 0)
                return result;
            else
                throw new ExecException(result, "Process exec error for " + command + " (exit code " + exitValue + ") : " + standardError.toString());
        }

        // timeout
        process.destroy();

        throw new TimeoutException("Process exec timeout elapsed for: " + command);
    }

    public static String lookupExecutable(String name) {
        if (name != null && !name.equals("")) {
            String systemPath = System.getenv("PATH");

            if (systemPath != null) {
                String separator = System.getProperty("file.separator");
                String[] directories = systemPath.split(System.getProperty("path.separator"));

                for (String directory : directories) {
                    String path = directory + separator + name;
                    if (new File(path).exists())
                        return path;

                    path = directory + separator + name + ".exe";
                    if (new File(path).exists())
                        return path;
                }
            }
        }

        return name;
    }
}
