package org.omnetpp.scave.engineext;

import java.util.EventListener;

import org.omnetpp.scave.engine.ResultFileManager;

public interface IResultFilesChangeListener extends EventListener {
	public void resultFileManagerChanged(ResultFileManager manager);
}
