package org.omnetpp.experimental.animation.model;

import java.util.ArrayList;
import java.util.List;


public class RuntimeModule implements IRuntimeModule {
	
	private int id;
	private String name;
	private int index;
	private int size;
	private RuntimeModule parent;
	private String typeName;
	private String displayString;
	private String backgroundDisplayString;
	private List<RuntimeModule> submodules;
	private List<RuntimeGate> gates;

	public RuntimeModule() {
	}

	public RuntimeModule(RuntimeModule parent) {
		parent.addSubmodule(this);
	}
	
	public void addSubmodule(RuntimeModule module) {
		if (module.getParentModule()!=null)
			throw new RuntimeException("submodule already has a parent");
		if (submodules==null)
			submodules = new ArrayList<RuntimeModule>(2);
		submodules.add(module);
		module.parent = this;
	}

	public void removeSubmodule(RuntimeModule module) {
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
		return index!=-1;
	}
	
	public int getSize() {
		return size;
	}

	public void setSize(int size) {
		this.size = size;
	}

	public String getFullName() {
		return size<0 ? name : name+"["+index+"]";
	}

	public String getFullPath() {
		return parent==null ? getFullName() : parent.getFullPath()+"."+getFullName();
	}

	public RuntimeModule getParentModule() {
		return parent;
	}

	public List<RuntimeModule> getSubmodules() {
		return submodules;
	}

	public String getDisplayString() {
		return displayString;
	}
	public void setDisplayString(String displayString) {
		this.displayString = displayString;
	}

	public String getBackgroundDisplayString() {
		return backgroundDisplayString;
	}

	public void setBackgroundDisplayString(String backgroundDisplayString) {
		this.backgroundDisplayString = backgroundDisplayString;
	}

	public int getId() {
		return id;
	}

	public void setId(int moduleId) {
		this.id = moduleId;
	}

	public RuntimeModule getSubmodule(String name) {
		//FIXME use more efficient data structure to avoid linear search! e.g. map of submodule vectors etc
		for (RuntimeModule m : submodules)
			if (name.equals(m.getName()) && m.getSize()<0)
				return m;
		return null;
	}

	public RuntimeModule getSubmodule(String name, int index) {
		//FIXME use more efficient data structure to avoid linear search! e.g. map of submodule vectors etc
		for (RuntimeModule m : submodules)
			if (name.equals(m.getName()) && m.getIndex()==index)
				return m;
		return null;
	}

	//TODO removeGate, more efficient storage, etc
	public void addGate(RuntimeGate gate) {
		gates.add(gate);
	}
	
	public RuntimeGate getGate(int i) {
		return gates.get(index);
	}

	public int getNumGates() {
		return gates.size();
	}
}
