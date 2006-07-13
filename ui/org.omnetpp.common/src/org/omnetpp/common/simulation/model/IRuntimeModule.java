package org.omnetpp.common.simulation.model;

import org.omnetpp.common.displaymodel.IDisplayString;

public interface IRuntimeModule {
	
	public int getId();

	public String getName();

	public int getIndex();

	public int getSize();

	public boolean isVector();
	
	public String getFullName();

	public String getFullPath();

	public String getTypeName();

	public IDisplayString getDisplayString();

	public IRuntimeModule getParentModule();
	
	//public List<? extends IRuntimeModule> getSubmodules();
	
    public IRuntimeModule getSubmodule(String name);

    public IRuntimeModule getSubmodule(String name, int index);
    
    public int getNumGates();
    
    public IRuntimeGate getGate(int i); 
}
