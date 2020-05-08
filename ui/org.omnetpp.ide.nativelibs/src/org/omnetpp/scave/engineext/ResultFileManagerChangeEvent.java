/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.engineext;

import org.omnetpp.scave.engine.ResultFileManager;

public class ResultFileManagerChangeEvent {
    private ResultFileManager manager;

    public ResultFileManagerChangeEvent(ResultFileManager manager) {
        this.manager = manager;
    }

    public ResultFileManager getResultFileManager() {
        return manager;
    }
}
