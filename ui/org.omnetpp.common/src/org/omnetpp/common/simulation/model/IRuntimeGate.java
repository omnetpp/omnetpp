/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.simulation.model;

public interface IRuntimeGate {
	public int getId();

	public String getName();

	public int getIndex();

	public int getVectorSize();

	public boolean isVector();
	
	public String getFullName();

	public IRuntimeModule getOwnerModule();
	
}
