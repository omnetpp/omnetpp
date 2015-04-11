/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

/**
 * Interface defining the application's command IDs.
 * Key bindings can be defined for specific commands.
 * To associate an action with a command, use IAction.setActionDefinitionId(commandId).
 *
 * @see org.eclipse.jface.action.IAction#setActionDefinitionId(String)
 */
public interface ICommandIds {

    public static final String CMD_OPEN_RESULT_FILE = "org.omnetpp.scave.openScalarFile";
    public static final String CMD_EXTEND_DATASET = "org.omnetpp.scave.extendDataset";
    public static final String CMD_DELETE_REST = "org.omnetpp.scave.deleteRest";
    //public static final String CMD_CREATE_BARCHART = "org.omnetpp.scave.openScalarFile";

}
