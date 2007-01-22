package org.omnetpp.scave.model;

import java.util.EventListener;

public interface IModelChangeListener extends EventListener {

	void handleChange();

}

