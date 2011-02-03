/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.eventlog;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.common.displaymodel.IDisplayString;

public class EventLogModule {
	private int id;
	private String name;
	private int index;
	private int size = -1;
	private EventLogModule parent;
	private String typeName;
	private String className;
	private IDisplayString displayString;
	private List<EventLogModule> submodules;
	private List<EventLogGate> gates;

	public EventLogModule() {
	}

	public EventLogModule(EventLogModule parent) {
		if (parent != null)
			parent.addSubmodule(this);
	}

	public EventLogModule(EventLogModule parent, int id) {
		this(parent);
		this.id = id;
	}

	public void addSubmodule(EventLogModule module) {
		if (module.getParentModule() != null)
			throw new RuntimeException("submodule already has a parent");
		if (submodules == null)
			submodules = new ArrayList<EventLogModule>(2);
		submodules.add(module);
		module.parent = this;
	}

	public void removeSubmodule(EventLogModule module) {
		if (module.getParentModule() != this)
			throw new RuntimeException("not parent of this submodule");
		submodules.remove(module);
		module.parent = null;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getTypeName() {
		return typeName;
	}

	public void setTypeName(String type) {
		this.typeName = type;
	}

	public String getClassName() {
	    return className;
    }

    public void setClassName(String className) {
        this.className = className;
    }

	public int getIndex() {
		return index;
	}

	public void setIndex(int index) {
		this.index = index;
	}

	public boolean isVector() {
		return size!=-1;
	}

	public int getVectorSize() {
		return size;
	}

	public void setSize(int size) {
		this.size = size;
	}

	public String getFullName() {
		return !isVector() ? name : name+"["+index+"]";
	}

	public String getFullPath() {
		return parent == null ? getFullName() : parent.getFullPath() + "." + getFullName();
	}

	public EventLogModule getParentModule() {
		return parent;
	}

	public List<EventLogModule> getSubmodules() {
		return submodules;
	}

	public IDisplayString getDisplayString() {
		return displayString;
	}
	public void setDisplayString(IDisplayString displayString) {
		this.displayString = displayString;
	}

	public int getId() {
		return id;
	}

	public void setId(int moduleId) {
		this.id = moduleId;
	}

	public EventLogModule getSubmodule(String name) {
		// TODO: use more efficient data structure to avoid linear search! e.g. map of submodule vectors etc
		if (submodules != null)
			for (EventLogModule m : submodules)
				if (name.equals(m.getName()) && m.getVectorSize()<0)
					return m;

		return null;
	}

	public EventLogModule getSubmodule(String name, int index) {
		// TODO: use more efficient data structure to avoid linear search! e.g. map of submodule vectors etc
		if (submodules != null)
			for (EventLogModule m : submodules)
				if (name.equals(m.getName()) && m.getIndex()==index)
					return m;
		return null;
	}

	// TODO: this method to be removed -- ID is globally unique so it makes no sense
	public EventLogModule getSubmoduleByID(int id) {
		if (submodules != null)
			for (EventLogModule submodule : submodules)
				if (submodule.getId() == id)
					return submodule;
		return null;
	}

	// TODO: removeGate, more efficient storage, etc
	public void addGate(EventLogGate gate) {
        if (gates == null)
            gates = new ArrayList<EventLogGate>(2);
		gates.add(gate);
	}

	public EventLogGate getGate(int i) {
		return gates.get(index);
	}

	public int getNumGates() {
	    if (gates == null)
	        return 0;
	    else
	        return gates.size();
	}

    public EventLogGate getGateById(int id) {
        if (gates != null)
            for (EventLogGate gate : gates)
                if (gate.getId() == id)
                    return gate;
        return null;
    }

	public EventLogModule getCommonAncestorModule(EventLogModule otherModule) {
	    if (isAncestorModuleOf(otherModule))
	        return this;
	    else if (otherModule.isAncestorModuleOf(this))
	        return otherModule;
	    else
	        return getCommonAncestorModule(otherModule.getParentModule());
	}

	public boolean isAncestorModuleOf(EventLogModule otherModule) {
	    while (otherModule != null) {
	        if (otherModule.equals(this))
	            return true;
	        else
	            otherModule = otherModule.getParentModule();
	    }
	    return false;
	}

	@Override
	public String toString() {
	    return typeName + "(" + id + ")";
	}

	@Override
	public int hashCode() {
		final int PRIME = 31;
		int result = 1;
		result = PRIME * result + id;
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
		final EventLogModule other = (EventLogModule) obj;
		if (id != other.id)
			return false;
		return true;
	}
}
