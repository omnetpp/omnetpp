package org.omnetpp.launch;

import org.eclipse.core.runtime.Assert;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunchConfigurationType;
import org.eclipse.debug.internal.ui.DebugUIPlugin;
import org.eclipse.debug.ui.DebugUITools;
import org.eclipse.debug.ui.ILaunchShortcut;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.IEditorPart;

// TODO this should be more do some searching for already existing configuration
// and invoke them directly if only a single one found (read more in: We Have Lift-off: The Launching Framework in Eclipse)
// use the following in plugin.xml
//<extension
//point="org.eclipse.debug.ui.launchShortcuts">
//<shortcut
//class="org.omnetpp.launch.SimulationShortcut"
//icon="icons/full/ctool16/omnetsim.gif"
//id="org.omnetpp.launch.shortcut"
//label="OMNeT++ Simulation..."
//modes="run, debug">
//<contextualLaunch>
//  <enablement>
//    <with variable="selection">
//      <count value="1"/>
//      <iterate>
//        <test property="org.eclipse.debug.ui.matchesPattern" value="*.ini"/>
//      </iterate>
//    </with>
//  </enablement>
//	 </contextualLaunch>
//</shortcut>   
//
//</extension>

public class SimulationShortcut implements ILaunchShortcut {

	public void launch(ISelection selection, String mode) {
		// select the simulation type
		ILaunchConfigurationType simLaunchConfigurationType = DebugPlugin.getDefault().getLaunchManager().getLaunchConfigurationType("org.omnetpp.launch.simulationLaunchConfigurationType");
		Assert.isNotNull(simLaunchConfigurationType, "simulationLaunchConfigurationType not registered");
		IStructuredSelection ssel = new StructuredSelection(simLaunchConfigurationType);
		DebugUITools.openLaunchConfigurationDialog(DebugUIPlugin.getShell(), ssel, mode);
	}

	public void launch(IEditorPart editor, String mode) {
		launch((ISelection)null, mode);
	}
}
