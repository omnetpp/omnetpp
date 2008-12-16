/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.replay.model;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.simulation.model.IRuntimeModule;


public class ReplayModule implements IRuntimeModule {
	
	private int id;
	private String name;
	private int index;
	private int size = -1;
	private ReplayModule parent;
	private String typeName;
	private DisplayString displayString;
	private List<ReplayModule> submodules;
	private List<ReplayGate> gates;

	public ReplayModule() {
	}

	public ReplayModule(ReplayModule parent) {
		if (parent != null)
			parent.addSubmodule(this);
	}
	
	public ReplayModule(ReplayModule parent, int id) {
		this(parent);
		this.id = id;
	}
	
	public void addSubmodule(ReplayModule module) {
		if (module.getParentModule()!=null)
			throw new RuntimeException("submodule already has a parent");
		if (submodules==null)
			submodules = new ArrayList<ReplayModule>(2);
		submodules.add(module);
		module.parent = this;
	}

	public void removeSubmodule(ReplayModule module) {
		if (module.getParentModule()!=this)
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

	public int getIndex() {
		return index;
	}

	public void setIndex(int index) {
		this.index = index;
	}

	public boolean isVector() {
		return size!=-1;
	}
	
	public int getSize() {
		return size;
	}

	public void setSize(int size) {
		this.size = size;
	}

	public String getFullName() {
		return !isVector() ? name : name+"["+index+"]";
	}

	public String getFullPath() {
		return parent==null ? getFullName() : parent.getFullPath()+"."+getFullName();
	}

	public ReplayModule getParentModule() {
		return parent;
	}

	public List<ReplayModule> getSubmodules() {
		return submodules;
	}

	public DisplayString getDisplayString() {
		return displayString;
	}
	public void setDisplayString(DisplayString displayString) {
		this.displayString = displayString;
	}

	public int getId() {
		return id;
	}

	public void setId(int moduleId) {
		this.id = moduleId;
	}

	public ReplayModule getSubmodule(String name) {
		//FIXME use more efficient data structure to avoid linear search! e.g. map of submodule vectors etc
		if (submodules != null)
			for (ReplayModule m : submodules)
				if (name.equals(m.getName()) && m.getSize()<0)
					return m;

		return null;
	}

	public ReplayModule getSubmodule(String name, int index) {
		//FIXME use more efficient data structure to avoid linear search! e.g. map of submodule vectors etc
		if (submodules != null)
			for (ReplayModule m : submodules)
				if (name.equals(m.getName()) && m.getIndex()==index)
					return m;

		return null;
	}

	//FIXME this method to be removed -- ID is globally unique so it makes no sense
	public ReplayModule getSubmoduleByID(int id) {  
		if (submodules != null)
			for (ReplayModule submodule : submodules)
				if (submodule.getId() == id)
					return submodule;
		
		return null;
	}

	//TODO removeGate, more efficient storage, etc
	public void addGate(ReplayGate gate) {
		gates.add(gate);
	}
	
	public ReplayGate getGate(int i) {
		return gates.get(index);
	}

	public int getNumGates() {
		return gates.size();
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
		final ReplayModule other = (ReplayModule) obj;
		if (id != other.id)
			return false;
		return true;
	}
}
