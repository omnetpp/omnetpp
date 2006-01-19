package org.omnetpp.scave.model;

import java.util.EventListener;

public abstract class ModelChangeListener implements EventListener {

	public abstract void handleChange();

}

