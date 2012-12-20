/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.launch;

import org.eclipse.cdt.dsf.gdb.launching.GdbLaunchDelegate;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Platform;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialogWithToggle;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.LaunchPlugin;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;

/**
 * A proxy launch delegate for debugging simulations.
 */
public class SimulationDebugLaunchDelegate extends GdbLaunchDelegate {
    private static final String PREF_DONT_OFFER_CONFIG_UPGRADE = "org.omnetpp.cdt.launch.DoNotOfferUpgradingConfig";

    @Override
    public void launch(ILaunchConfiguration config, String mode, ILaunch launch, IProgressMonitor monitor) throws CoreException {

        if (!Platform.getOS().equals(Platform.OS_MACOSX)) {  //OS X contains obsolete gdb without Python support
            ILaunchConfiguration newConfig = offerUpgradingLaunchConfig(config);
            if (newConfig != null) 
                OmnetppLaunchUtils.setConfigurationInLaunch(launch, newConfig);
        }

        OmnetppLaunchUtils.updateLaunchConfigurationWithProgramAttributes(mode, launch);

        // we must use the updated configuration in 'launch' instead the original passed to us
        super.launch(launch.getLaunchConfiguration(), mode, launch, monitor);
    }

    /**
     * Convert pre-4.3 launch configs to 4.3+. This code can be removed once we are well past version 4.3.
     */
    protected ILaunchConfiguration offerUpgradingLaunchConfig(final ILaunchConfiguration config) throws CoreException {
        String gdbInitFile = config.getAttribute(IOmnetppLaunchConstants.ATTR_DEBUGGER_GDB_INIT, "");

        if (gdbInitFile.equals(".gdbinit")) {
            // offer user to replace it with the right setting
            final IPreferenceStore preferences = LaunchPlugin.getDefault().getPreferenceStore();
            String pref = preferences.getString(PREF_DONT_OFFER_CONFIG_UPGRADE);

            if (MessageDialogWithToggle.ALWAYS.equals(pref)) {
                return upgradeLaunchConfig(config);
            }
            else if (!MessageDialogWithToggle.NEVER.equals(pref)) {
                final int[] dialogResult = new int[1];
                Display.getDefault().syncExec(new Runnable() {
                    @Override
                    public void run() {
                        dialogResult[0] = MessageDialogWithToggle.openYesNoQuestion(
                                Display.getCurrent().getActiveShell(),
                                "Obsolete Launch Configuration",
                                "Launch configuration '" + config.getName() + "' contains '.gdbinit' as " +
                                "GDB initialization file. To be able to utilize pretty printers, this should be " +
                                "changed to '" + IOmnetppLaunchConstants.OPP_GDB_INIT_FILE + "'.\n\n" +
                                "Upgrade launch configuration?",
                                "Always do that in the future", false,
                                preferences, PREF_DONT_OFFER_CONFIG_UPGRADE).getReturnCode();
                    }
                });

                if (dialogResult[0] == IDialogConstants.YES_ID)
                    return upgradeLaunchConfig(config);
            }
        }
        return null;
    }

    protected ILaunchConfigurationWorkingCopy upgradeLaunchConfig(ILaunchConfiguration config) throws CoreException {
        ILaunchConfigurationWorkingCopy workingCopy = config.getWorkingCopy();
        workingCopy.setAttribute(IOmnetppLaunchConstants.ATTR_DEBUGGER_GDB_INIT, IOmnetppLaunchConstants.OPP_GDB_INIT_FILE);
        workingCopy.doSave();
        return workingCopy;
    }
}
