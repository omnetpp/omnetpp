package org.omnetpp.experimental.animation.model;

public interface IRuntimeGate {
	public int getId();

	public String getName();

	public int getIndex();

	public int getSize();

	public boolean isVector();
	
	public String getFullName();

	public IRuntimeModule getOwnerModule();
	
}
