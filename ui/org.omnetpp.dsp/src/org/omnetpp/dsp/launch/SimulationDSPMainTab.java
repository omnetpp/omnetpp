package 	org.omnetpp.dsp.launch;

import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.lsp4e.debug.launcher.DSPMainTab;
import org.omnetpp.launch.IOmnetppLaunchConstants;

public class SimulationDSPMainTab extends DSPMainTab {
	public SimulationDSPMainTab() {
		super(true); // enable custom settings check-box on the tab
	}
	
	@Override
	public void setDefaults(ILaunchConfigurationWorkingCopy configuration) {
		super.setDefaults(configuration);		
		configuration.setAttribute(IOmnetppLaunchConstants.ATTR_DSP_CMD, IOmnetppLaunchConstants.OPP_DEFAULT_DSP_CMD_NAME);
		configuration.setAttribute(IOmnetppLaunchConstants.ATTR_DSP_ARGS, IOmnetppLaunchConstants.OPP_DEFAULT_DSP_CMD_ARGS);
	}
}
