package org.omnetpp.dsp.debug.debugmodel;

import java.util.List;
import java.util.Map;
import java.util.function.Supplier;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.debug.core.DebugException;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.model.IProcess;
import org.eclipse.lsp4e.debug.debugmodel.DSPDebugTarget;
import org.eclipse.lsp4e.debug.debugmodel.TransportStreams;
import org.eclipse.lsp4j.debug.ExitedEventArguments;
import org.eclipse.lsp4j.debug.ProcessEventArguments;
import org.eclipse.lsp4j.debug.TerminatedEventArguments;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.dsp.Activator;
import org.omnetpp.dsp.debug.console.SimulationDSPProcess;

@SuppressWarnings("restriction")
public class SimulationDSPDebugTarget extends DSPDebugTarget {
	public SimulationDSPDebugTarget(ILaunch launch, Supplier<TransportStreams> streamsSupplier,
			Map<String, Object> dspParameters) {
		super(launch, streamsSupplier, dspParameters);
	}
	
	@Override
	public void initialize(IProgressMonitor monitor) throws CoreException {
		super.initialize(monitor);
		// refine the name of the target node in launch view
		@SuppressWarnings("unchecked")
		var dspPars = (Map<String, Object>)ReflectionUtils.getFieldValue(this, "dspParameters");
		var shortName = new Path((String)dspPars.get("program")).lastSegment();
		var cmdLineOptions = StringUtils.join((List<String>)dspPars.get("args"), " "); 
		ReflectionUtils.setFieldValue(this, "targetName", shortName + " " + cmdLineOptions);
	}

	@Override
	public void process(ProcessEventArguments args) {
		var process = new SimulationDSPProcess(this, args);
		if (args == null) // this is just a dummy process to attach the lldb-dap process console
			process.setLabel("LLDB Debugger");  // use a different name for the LLDB console pseudo process
		else {
			var pid = process.getAttribute(IProcess.ATTR_PROCESS_ID);
			process.setLabel("#" + pid + ": " + args.getName());
		}
		ReflectionUtils.setFieldValue(this, "process", process);
		// by default the lldb-dap process is added to the launch
		getLaunch().addProcess(process);
	}
	
	
	@Override
	public SimulationDSPProcess getProcess() {
		// TODO Auto-generated method stub
		return (SimulationDSPProcess)super.getProcess();
	}
	
    @Override
    public void exited(ExitedEventArguments args) {
    	getProcess().setExitValue(args.getExitCode());
    }
    
    @Override
    public void terminated(TerminatedEventArguments body) {
    	super.terminated(body);
    	try {
    		// terminate the whole launch on the debug target exit
    		// NOTE: this is not a good behavior if multiple targets exists (rarely happens)
    		// TODO make the exit behavior configurable
    		// TODO possibly add option to even remove the launch after termination 
			getLaunch().terminate();
		} catch (DebugException e) {
			Activator.logError(e);
		}
    }
}
