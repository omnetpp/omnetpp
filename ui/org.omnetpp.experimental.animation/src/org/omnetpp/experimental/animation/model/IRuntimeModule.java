package org.omnetpp.experimental.animation.model;

import java.util.List;

public interface IRuntimeModule {
	
	public int getId();

	public String getName();

	public int getIndex();

	public int getSize();

	public boolean isVector();
	
	public String getFullName();

	public String getFullPath();

	public String getTypeName();

	public String getDisplayString(); //XXX

	public String getBackgroundDisplayString(); //XXX

	public RuntimeModule getParentModule();
	
	public List<RuntimeModule> getSubmodules();
	
    public IRuntimeModule getSubmodule(String name);

    public  IRuntimeModule getSubmodule(String name, int index);
    
    public int getNumGates();
    
    public IRuntimeGate getGate(int i); 
}
