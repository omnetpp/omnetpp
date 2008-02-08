package org.omnetpp.launch;

import org.eclipse.debug.ui.AbstractLaunchConfigurationTabGroup;
import org.eclipse.debug.ui.CommonTab;
import org.eclipse.debug.ui.EnvironmentTab;
import org.eclipse.debug.ui.ILaunchConfigurationDialog;
import org.eclipse.debug.ui.ILaunchConfigurationTab;

import org.omnetpp.launch.tabs.OmnetppMainTab;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class SimulationLaunchConfigurationTabGroup extends AbstractLaunchConfigurationTabGroup {

    public SimulationLaunchConfigurationTabGroup() {
    }

    public void createTabs(ILaunchConfigurationDialog dialog, String mode) {
        ILaunchConfigurationTab[] tabs = new ILaunchConfigurationTab[] {
                new OmnetppMainTab(),
                new EnvironmentTab(),
                new CommonTab()
            };
            setTabs(tabs);
    }

}
