/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.launch;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.debug.ui.AbstractLaunchConfigurationTabGroup;
import org.eclipse.debug.ui.CommonTab;
import org.eclipse.debug.ui.EnvironmentTab;
import org.eclipse.debug.ui.ILaunchConfigurationDialog;
import org.eclipse.debug.ui.ILaunchConfigurationTab;
import org.omnetpp.launch.tabs.OmnetppMainTab;

/**
 * Defines the tab group for OMNeT simulation specific launches
 *
 * @author rhornig
 */
public class SimulationLaunchConfigurationTabGroup extends AbstractLaunchConfigurationTabGroup {

    public SimulationLaunchConfigurationTabGroup() {
    }

    public void createTabs(ILaunchConfigurationDialog dialog, String mode) {
        List<ILaunchConfigurationTab> tabList = new ArrayList<ILaunchConfigurationTab>();
        tabList.add(new OmnetppMainTab());
        tabList.add(new EnvironmentTab());
        tabList.add(new CommonTab());
        setTabs(tabList.toArray(new ILaunchConfigurationTab[tabList.size()]));
    }

}
