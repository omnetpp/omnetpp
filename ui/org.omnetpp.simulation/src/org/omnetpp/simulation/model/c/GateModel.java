package org.omnetpp.simulation.model.c;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.displaymodel.IDisplayString;

public class GateModel {
    private int id;
    private String name;
	private int index;
	private int size;
	private ModuleModel ownerModule;
	private IDisplayString displayString;
	private ConnectionModel incomingConnection;
    private ConnectionModel outgoingConnection;

	public GateModel(ModuleModel ownerModule, int id) {
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

	public ModuleModel getOwnerModule() {
		return ownerModule;
	}

    public int getOwnerModuleId() {
        return ownerModule.getId();
    }

    public ConnectionModel getIncomingConnection() {
        return incomingConnection;
    }

    public void setIncomingConnection(ConnectionModel incomingConnection) {
        Assert.isTrue(this.incomingConnection == null);
        this.incomingConnection = incomingConnection;
    }

    public ConnectionModel getOutgoingConnection() {
        return outgoingConnection;
    }

    public void setOutgoingConnection(ConnectionModel outgoingConnection) {
        Assert.isTrue(this.outgoingConnection == null);
        this.outgoingConnection = outgoingConnection;
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
        GateModel other = (GateModel) obj;
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
