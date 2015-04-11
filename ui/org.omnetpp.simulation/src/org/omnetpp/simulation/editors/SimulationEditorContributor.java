/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.editors;

import org.eclipse.jface.action.IAction;
import org.eclipse.ui.part.EditorActionBarContributor;

/**
 * Contributes context menu and toolbar items to the platform.
 *
 * @author andras
 */
public class SimulationEditorContributor extends EditorActionBarContributor /*XXX rather: EventLogAnimationContributor*/ {
    // Note: the following fields are filled in from the respective action class's code
    // when Eclipse instantiates them. Unfortunately there seems to be no API to obtain
    // action instances by their ID.
    public static IAction callFinishAction;
    public static IAction expressRunAction;
    public static IAction fastRunAction;
    public static IAction linkWithSimulationAction;
    public static IAction processInfoAction;
    public static IAction rebuildNetworkAction;
    public static IAction refreshAction;
    public static IAction runAction;
    public static IAction runUntilAction;
    public static IAction runUntilDialog;
    public static IAction setupIniConfigAction;
    public static IAction setupNetworkAction;
    public static IAction stepAction;
    public static IAction stopAction;

}
