/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.simulation.model;

import org.omnetpp.common.displaymodel.IDisplayString;

public interface IRuntimeModule {

	public int getId();

	public String getName();

	public int getIndex();

	public int getVectorSize();

	public boolean isVector();

	public String getFullName();

	public String getFullPath();

	public String getTypeName();

	public IDisplayString getDisplayString();

	public IRuntimeModule getParentModule();

	//public List<? extends IRuntimeModule> getSubmodules();

    public IRuntimeModule getSubmodule(String name);

    public IRuntimeModule getSubmodule(String name, int index);

    public IRuntimeGate getGate(int i);
}
