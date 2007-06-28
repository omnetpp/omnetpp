package org.omnetpp.launch;

import org.eclipse.debug.ui.AbstractLaunchConfigurationTabGroup;
import org.eclipse.debug.ui.CommonTab;
import org.eclipse.debug.ui.EnvironmentTab;
import org.eclipse.debug.ui.ILaunchConfigurationDialog;
import org.eclipse.debug.ui.ILaunchConfigurationTab;

import org.omnetpp.launch.tabs.OmnetppMainTab;

public class StandaloneOmnetppLaunchConfigurationTabGroup extends AbstractLaunchConfigurationTabGroup {

    public StandaloneOmnetppLaunchConfigurationTabGroup() {
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
