package org.omnetpp.experimental.animation.replay.modelchange;

public interface IRuntimeModelChange {
	public void execute();
	public void undo();
}
