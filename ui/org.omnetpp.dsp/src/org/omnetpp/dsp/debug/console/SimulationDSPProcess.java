package org.omnetpp.dsp.debug.console;

import org.eclipse.core.runtime.Status;
import org.eclipse.debug.core.DebugException;
import org.eclipse.lsp4e.debug.console.DSPProcess;
import org.eclipse.lsp4e.debug.debugmodel.DSPDebugTarget;
import org.eclipse.lsp4j.debug.ProcessEventArguments;

@SuppressWarnings("restriction")
public class SimulationDSPProcess extends DSPProcess {

	protected String label;
	protected int exitValue = 0;
	
	public SimulationDSPProcess(DSPDebugTarget target) {
		super(target);
	}
	
	public SimulationDSPProcess(DSPDebugTarget dspDebugTarget, ProcessEventArguments args) {
		super(dspDebugTarget, args);
	}
	
	public void setLabel(String label) {
		this.label = label;
	}

	@Override
	public String getLabel() {
		return (label != null) ? label : super.getLabel(); 
	}	
	
	public void setExitValue(int exitValue) {
        this.exitValue = exitValue;
    }
	
	@Override
	public int getExitValue() throws DebugException {
		super.getExitValue(); // just do the checking whether the process is terminated and ignore the return code as it is (incorrectly) hardcoded 0
		return exitValue;
	}

	
}
