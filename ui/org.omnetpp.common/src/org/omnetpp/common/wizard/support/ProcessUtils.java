package org.omnetpp.common.wizard.support;

import java.io.IOException;

import org.omnetpp.common.util.ProcessUtils.ExecException;
import org.omnetpp.common.util.ProcessUtils.TimeoutException;

/**
 * The class made available to content templates as "ProcessUtils".
 * Delegation was introduced to keep this class as stable, public and
 * documented API. 
 * 
 * @author Andras
 */
//XXX Freemarker apparently cannot cope with naked public data members of original ProcessResult -- check!
public class ProcessUtils {
    public static class ProcessResult {
        private String standardOutput;
        private String standardError;
        private int exitValue;

        public String stdout() {return standardOutput;}
        public String stderr() {return standardError;}
        public int exitCode()  {return exitValue;}

        public ProcessResult(org.omnetpp.common.util.ProcessUtils.ProcessResult r) {
            this.standardOutput = r.standardOutput;
            this.standardError = r.standardError;
            this.exitValue = r.exitValue;
        }
    }
    
    
    public static ProcessResult exec(String command) throws IOException, TimeoutException, ExecException {
        return new ProcessResult(org.omnetpp.common.util.ProcessUtils.exec(command));
    }

    public static ProcessResult exec(String command, String[] arguments) throws IOException, TimeoutException, ExecException {
        return new ProcessResult(org.omnetpp.common.util.ProcessUtils.exec(command, arguments));
    }

    public static ProcessResult exec(String command, String[] arguments, String workingDirectory) throws IOException, TimeoutException, ExecException {
        return new ProcessResult(org.omnetpp.common.util.ProcessUtils.exec(command, arguments, workingDirectory));
    }

    public static ProcessResult exec(String command, String[] arguments, String workingDirectory, String standardInput) throws IOException, TimeoutException, ExecException {
        return new ProcessResult(org.omnetpp.common.util.ProcessUtils.exec(command, arguments, workingDirectory, standardInput, 0));
    }

    /**
     * Executes the given command with the arguments as a separate process.
     * The standard input is fed into the spawn process and the output is read until the process finishes or timeout occurs. 
     * The timeout value 0 means wait infinitely long to finish the process.
     */
    public static ProcessResult exec(String command, String[] arguments, String workingDirectory, String standardInput, double timeout) throws IOException, TimeoutException, ExecException {
        return new ProcessResult(org.omnetpp.common.util.ProcessUtils.exec(command, arguments, workingDirectory, standardInput, timeout));
    }

    /**
     * Finds the given executable in the PATH, and returns it with a full path.
     * If not found, it returns the original string.
     */
    public static String lookupExecutable(String name) {
        return org.omnetpp.common.util.ProcessUtils.lookupExecutable(name);
    }

}
