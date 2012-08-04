/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.editors;

import org.eclipse.jface.action.IToolBarManager;
import org.omnetpp.animation.eventlog.editors.EventLogAnimationContributor;

/**
 * Contributes context menu and toolbar items to the platform.
 *
 * @author andras
 */
public class SimulationContributor extends EventLogAnimationContributor {
    @Override
    public void contributeToToolBar(IToolBarManager toolBarManager) {
        // do not contribute to global toolbars
    }
}
