/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.eventlog;

import org.omnetpp.common.displaymodel.IDisplayString;

public class EventLogGate {
    private int id;
    private String name;
	private int index;
	private int size;
	private EventLogModule ownerModule;
	private IDisplayString displayString;

	public EventLogGate(EventLogModule ownerModule, int id) {
	    this.ownerModule = ownerModule;
	    this.id = id;
    }

    public int getId() {
		return id;
	}

	public void setId(int gateId) {
		this.id = gateId;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public int getIndex() {
		return index;
	}

	public void setIndex(int index) {
		this.index = index;
	}

	public boolean isVector() {
		return index!=-1;
	}

	public int getVectorSize() {
		return size;
	}

	public void setSize(int size) {
		this.size = size;
	}

	public String getFullName() {
		return size<0 ? name : name+"["+index+"]";
	}

	public String getFullPath() {
		return ownerModule==null ? getFullName() : ownerModule.getFullPath()+"."+getFullName();
	}

	public EventLogModule getOwnerModule() {
		return ownerModule;
	}

    public int getOwnerModuleId() {
        return ownerModule.getId();
    }

    public IDisplayString getDisplayString() {
        return displayString;
    }

    public void setDisplayString(IDisplayString displayString) {
        this.displayString = displayString;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + id;
        result = prime * result + ownerModule.getId();
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        EventLogGate other = (EventLogGate) obj;
        if (id != other.id)
            return false;
        if (ownerModule.getId() != other.ownerModule.getId())
            return false;
        return true;
    }

    @Override
    public String toString() {
        return ownerModule.toString() + " : " + id;
    }
}
