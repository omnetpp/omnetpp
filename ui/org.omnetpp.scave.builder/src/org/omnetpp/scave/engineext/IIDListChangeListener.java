package org.omnetpp.scave.engineext;

import java.util.EventListener;

import org.omnetpp.scave.engine.IDList;

public interface IIDListChangeListener extends EventListener {
	public void idlistChanged(IDList idlist);
}
