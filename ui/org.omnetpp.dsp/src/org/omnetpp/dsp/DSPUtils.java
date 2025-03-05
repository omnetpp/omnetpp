package org.omnetpp.dsp;

import java.util.ArrayList;
import java.util.List;
import java.util.StringJoiner;

import org.apache.commons.lang3.StringEscapeUtils;
import org.eclipse.cdt.debug.core.ICDTLaunchConfigurationConstants;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationType;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.debug.core.IStreamListener;
import org.eclipse.debug.core.model.IProcess;
import org.eclipse.debug.core.model.IStreamMonitor;
import org.eclipse.debug.core.model.IStreamsProxy;
import org.eclipse.lsp4e.debug.DSPPlugin;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.dsp.debug.debugmodel.SimulationDSPDebugTarget;
import org.omnetpp.launch.IOmnetppLaunchConstants;

public class DSPUtils {
    /**
     * Attach the debugger to the provided PID
     * @param pid
     */
    public static void attach(String pid) {

        try {
            var config = createAttachedConfigurationForPid(pid);
            if (config != null)
                Display.getDefault().syncExec(() -> {
                    try {
                        config.launch(ILaunchManager.DEBUG_MODE, null, false);
                    }
                    catch (CoreException e) {
                        throw new RuntimeException(e);
                    }
                });
        } catch (Exception e) {
            Activator.logError(e);
        } finally {
        }
    }

    public static class LaunchResult
    {
        public int returncode = 0;
        public String stdout = null;
        public String stderr = null;
    }

    public static LaunchResult runConfiguration(ILaunchConfiguration config, boolean removeLaunch) {
        try {
            LaunchResult launchResult = new LaunchResult();
            StringBuffer stdout = new StringBuffer();
            StringBuffer stderr = new StringBuffer();
            ILaunch launch = config.launch(ILaunchManager.RUN_MODE, null, false);
            IProcess[] processes = launch.getProcesses();
            IProcess process0 = processes[0];
            IStreamsProxy streamsProxy = process0.getStreamsProxy();
            IStreamListener stdoutListener = new IStreamListener() {
                @Override
                public void streamAppended(String text, IStreamMonitor monitor) {
                    stdout.append(text);
                }
            };
            IStreamListener stderrListener = new IStreamListener() {
                @Override
                public void streamAppended(String text, IStreamMonitor monitor) {
                    stderr.append(text);
                }
            };
            streamsProxy.getOutputStreamMonitor().addListener(stdoutListener);
            streamsProxy.getErrorStreamMonitor().addListener(stderrListener);
            while (!process0.isTerminated()) {
                try {
                    Thread.sleep(100);
                }
                catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            launchResult.returncode = process0.getExitValue();
            launchResult.stdout = stdout.toString();
            launchResult.stderr = stderr.toString();
            launch.terminate();
            if (removeLaunch)
                DebugPlugin.getDefault().getLaunchManager().removeLaunch(launch);
            return launchResult;
        }
        catch (CoreException e) {
            throw new RuntimeException(e);
        }
    }

    public static LaunchResult debugConfiguration(ILaunchConfiguration config, boolean removeLaunch) {
        try {
            LaunchResult launchResult = new LaunchResult();
            StringBuffer stdout = new StringBuffer();
            StringBuffer stderr = new StringBuffer();
            ILaunch launch = config.launch(ILaunchManager.DEBUG_MODE, null, false);
            IProcess[] processes = launch.getProcesses();
            IProcess process1 = processes[1];
            IStreamsProxy streamsProxy = process1.getStreamsProxy();
            IStreamListener stdoutListener = new IStreamListener() {
                @Override
                public void streamAppended(String text, IStreamMonitor monitor) {
                    stdout.append(text);
                }
            };
            IStreamListener stderrListener = new IStreamListener() {
                @Override
                public void streamAppended(String text, IStreamMonitor monitor) {
                    stderr.append(text);
                }
            };
            streamsProxy.getOutputStreamMonitor().addListener(stdoutListener);
            streamsProxy.getErrorStreamMonitor().addListener(stderrListener);
            while (!process1.isTerminated()) {
                try {
                    Thread.sleep(100);
                }
                catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            launchResult.returncode = ((SimulationDSPDebugTarget)launch.getDebugTarget()).getProcess().getExitValue();
            launchResult.stdout = stdout.toString();
            launchResult.stderr = stderr.toString();
            launch.terminate();
            if (removeLaunch)
                DebugPlugin.getDefault().getLaunchManager().removeLaunch(launch);
            return launchResult;
        }
        catch (CoreException e) {
            Activator.logError(e);
            throw new RuntimeException(e);
        }
    }

    private static ILaunchManager getLaunchManager() {
        return DebugPlugin.getDefault().getLaunchManager();
    }
    
    public static ILaunchConfiguration createLaunchConfiguration(String name, String program, List<String> args, String workingDirectory) {
        ILaunchConfiguration config = null;
        try {
            StringJoiner argsJoiner = new StringJoiner(" ", "", "");
            for (String s : args)
                argsJoiner.add("\"" + StringEscapeUtils.escapeJava(s) + "\"");
            ILaunchConfigurationType configType = getLaunchManager().getLaunchConfigurationType(ICDTLaunchConfigurationConstants.ID_LAUNCH_C_APP); // "org.eclipse.cdt.launch.applicationLaunchType");
            ILaunchConfigurationWorkingCopy wc = configType.newInstance(null, getLaunchManager().generateLaunchConfigurationName(name));
            wc.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, program);
            wc.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, argsJoiner.toString());
            wc.setAttribute(IOmnetppLaunchConstants.ATTR_WORKING_DIRECTORY, workingDirectory);
            wc.setAttribute(IOmnetppLaunchConstants.ATTR_DEBUGGER_START_MODE, "run");
            config = wc;
        }
        catch (CoreException e) {
            Activator.logError(e);
        }
        return config;
    }

    public static ILaunchConfiguration createAttachedConfigurationForPid(String pid) {
        ILaunchConfiguration config = null;
        try {
        	// FIXME: figure out the "program" name somehow as it must be a full path
            String dspParam = """
                {"type": "lldb-dap",
                 "request": "attach",
                 "name": "Debug",
                 "program": "opp_run_dbg",
                 "pid": """ + pid + "}";
            ILaunchConfigurationType configType = getLaunchManager().getLaunchConfigurationType(IDSPConstants.ID_DSP_HIDDEN_SIMULATION_CONFIG_TYPE);
            ILaunchConfigurationWorkingCopy wc = configType.newInstance(null, getLaunchManager().generateLaunchConfigurationName("attached"));
            wc.setAttribute(DSPPlugin.ATTR_DSP_PARAM, dspParam);
            wc.setAttribute(DSPPlugin.ATTR_DSP_CMD,  StringUtils.substituteVariables(IOmnetppLaunchConstants.OPP_DEFAULT_DSP_CMD_NAME));

            var dspargs = new ArrayList<String>();
            IOmnetppLaunchConstants.OPP_DEFAULT_DSP_CMD_ARGS.forEach(arg -> {
				try {
					dspargs.add(StringUtils.substituteVariables(arg));
				} catch (CoreException e) {
					dspargs.add(arg);  // in case of error use the original unexpanded value
				}
			});
            wc.setAttribute(DSPPlugin.ATTR_DSP_ARGS, dspargs);
            config = wc;
        }
        catch (CoreException e) {
            Activator.logError(e);
        }
        return config;
    }

    public static ILaunchConfiguration createDebugConfiguration(String name, String program, List<String> args, String workingDirectory, List<String> debuggerInitCommands) {
        ILaunchConfiguration config = null;
        try {
            StringJoiner argsJoiner = new StringJoiner(", ", "[", "]");
            for (String s : args)
                argsJoiner.add("\"" + StringEscapeUtils.escapeJava(s) + "\"");
            StringJoiner debuggerInitCommandsJoiner = new StringJoiner(", ", "[", "]");
            for (String s : debuggerInitCommands)
                debuggerInitCommandsJoiner.add("\"" + StringEscapeUtils.escapeJava(s) + "\"");
            String dspParam = """
                {"type": "lldb-dap",
                 "request": "launch",
                 "name": "Debug",
                 "initCommands": """ + debuggerInitCommandsJoiner.toString() + "," + """
                 "cwd": """ + "\"" + StringEscapeUtils.escapeJava(workingDirectory) + "\"," + """
                 "program": """ + "\"" + StringEscapeUtils.escapeJava(program) + "\"," + """
                 "args": """ + argsJoiner.toString() + "}";
            ILaunchConfigurationType configType = getLaunchManager().getLaunchConfigurationType(IDSPConstants.ID_DSP_HIDDEN_SIMULATION_CONFIG_TYPE);
            ILaunchConfigurationWorkingCopy wc = configType.newInstance(null, getLaunchManager().generateLaunchConfigurationName(name));
            wc.setAttribute(IOmnetppLaunchConstants.OPP_IGNORE_PROBLEMS_ON_LAUNCH, true);
            wc.setAttribute(DSPPlugin.ATTR_DSP_PARAM, dspParam);
            wc.setAttribute(DSPPlugin.ATTR_DSP_CMD,  StringUtils.substituteVariables(IOmnetppLaunchConstants.OPP_DEFAULT_DSP_CMD_NAME));

            var dspargs = new ArrayList<String>();
            IOmnetppLaunchConstants.OPP_DEFAULT_DSP_CMD_ARGS.forEach(arg -> {
				try {
					dspargs.add(StringUtils.substituteVariables(arg));
				} catch (CoreException e) {
					dspargs.add(arg);  // in case of error use the original unexpanded value
				}
			});
            wc.setAttribute(DSPPlugin.ATTR_DSP_ARGS, dspargs);
            config = wc;
        }
        catch (CoreException e) {
            Activator.logError(e);
        }
        return config;
    }
}
