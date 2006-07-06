package org.omnetpp.experimental.animation.model;

import java.util.List;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.experimental.animation.replay.model.ReplayModule;

public interface IRuntimeModule {
	
	public int getId();

	public String getName();

	public int getIndex();

	public int getSize();

	public boolean isVector();
	
	public String getFullName();

	public String getFullPath();

	public String getTypeName();

	public DisplayString getDisplayString();

	public IRuntimeModule getParentModule();
	
	public List<? extends IRuntimeModule> getSubmodules();
	
    public IRuntimeModule getSubmodule(String name);

    public  IRuntimeModule getSubmodule(String name, int index);
    
    public int getNumGates();
    
    public IRuntimeGate getGate(int i); 
}
