package org.omnetpp.common.wizard.support;

import java.io.IOException;

import org.omnetpp.common.util.ProcessUtils.ExecException;
import org.omnetpp.common.util.ProcessUtils.ProcessResult;
import org.omnetpp.common.util.ProcessUtils.TimeoutException;

/**
 * The class made available to content templates as "ProcessUtils".
 * Delegation was introduced to keep this class as stable, public and
 * documented API. 
 * 
 * @author Andras
 */
public class ProcessUtils {
    public static ProcessResult exec(String command) throws IOException, TimeoutException, ExecException {
        return org.omnetpp.common.util.ProcessUtils.exec(command);
    }

    public static ProcessResult exec(String command, String[] arguments) throws IOException, TimeoutException, ExecException {
        return org.omnetpp.common.util.ProcessUtils.exec(command, arguments);
    }

    public static ProcessResult exec(String command, String[] arguments, String workingDirectory) throws IOException, TimeoutException, ExecException {
        return org.omnetpp.common.util.ProcessUtils.exec(command, arguments, workingDirectory);
    }

    public static ProcessResult exec(String command, String[] arguments, String workingDirectory, String standardInput) throws IOException, TimeoutException, ExecException {
        return org.omnetpp.common.util.ProcessUtils.exec(command, arguments, workingDirectory, standardInput, 0);
    }

    /**
     * Executes the given command with the arguments as a separate process.
     * The standard input is fed into the spawn process and the output is read until the process finishes or timeout occurs. 
     * The timeout value 0 means wait infinitely long to finish the process.
     */
    public static ProcessResult exec(String command, String[] arguments, String workingDirectory, String standardInput, double timeout) throws IOException, TimeoutException, ExecException {
        return org.omnetpp.common.util.ProcessUtils.exec(command, arguments, workingDirectory, standardInput, timeout);
    }

    public static String lookupExecutable(String name) {
        return org.omnetpp.common.util.ProcessUtils.lookupExecutable(name);
    }

}
