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
