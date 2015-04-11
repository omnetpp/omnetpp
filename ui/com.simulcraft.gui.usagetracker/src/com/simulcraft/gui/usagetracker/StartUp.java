/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.gui.usagetracker;
import org.eclipse.ui.IStartup;

/**
 * Forces activation of the plugin on platform startup.
 * @author Andras
 */
public class StartUp implements IStartup {

    public void earlyStartup() {
        // nothing to do -- all work is done in Activator.start()
    }
}
