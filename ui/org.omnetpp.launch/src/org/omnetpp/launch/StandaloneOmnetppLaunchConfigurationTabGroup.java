package org.omnetpp.launch;

import org.eclipse.debug.ui.AbstractLaunchConfigurationTabGroup;
import org.eclipse.debug.ui.CommonTab;
import org.eclipse.debug.ui.ILaunchConfigurationDialog;
import org.eclipse.debug.ui.ILaunchConfigurationTab;

public class StandaloneOmnetppLaunchConfigurationTabGroup extends AbstractLaunchConfigurationTabGroup {

    public StandaloneOmnetppLaunchConfigurationTabGroup() {
        // TODO Auto-generated constructor stub
    }

    public void createTabs(ILaunchConfigurationDialog dialog, String mode) {
        ILaunchConfigurationTab[] tabs = new ILaunchConfigurationTab[] {
                new CommonTab()
            };
            setTabs(tabs);
    }

}
