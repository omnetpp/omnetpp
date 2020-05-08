/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model.commands;

import java.util.Collection;

import org.omnetpp.scave.model.ModelObject;

public interface ICommand {
    public void execute();
    public void undo();
    public void redo();
    public String getLabel();
    public Collection<ModelObject> getAffectedObjects();
}
