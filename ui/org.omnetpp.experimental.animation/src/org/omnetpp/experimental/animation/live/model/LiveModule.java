package org.omnetpp.experimental.animation.live.model;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.experimental.animation.model.IRuntimeModule;


//XXX should be Java wrapper around cModule
public class LiveModule extends LiveMessage implements IRuntimeModule {
	
	private int id;
	private String name;
	private int index;
	private int size;
	private LiveModule parent;
	private String typeName;
	private DisplayString displayString;
	private List<LiveModule> submodules;
	private List<LiveGate> gates;

	public LiveModule() {
	}

	public LiveModule(LiveModule parent) {
		if (parent != null)
			parent.addSubmodule(this);
	}
	
	public LiveModule(LiveModule parent, int id) {
		this(parent);
		this.id = id;
	}
	
	public void addSubmodule(LiveModule module) {
		if (module.getParentModule()!=null)
			throw new RuntimeException("submodule already has a parent");
		if (submodules==null)
			submodules = new ArrayList<LiveModule>(2);
		submodules.add(module);
		module.parent = this;
	}

	public void removeSubmodule(LiveModule module) {
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

	public LiveModule getParentModule() {
		return parent;
	}

	public List<LiveModule> getSubmodules() {
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

	public LiveModule getSubmodule(String name) {
		//FIXME use more efficient data structure to avoid linear search! e.g. map of submodule vectors etc
		for (LiveModule m : submodules)
			if (name.equals(m.getName()) && m.getSize()<0)
				return m;
		return null;
	}

	public LiveModule getSubmodule(String name, int index) {
		//FIXME use more efficient data structure to avoid linear search! e.g. map of submodule vectors etc
		for (LiveModule m : submodules)
			if (name.equals(m.getName()) && m.getIndex()==index)
				return m;
		return null;
	}

	//TODO removeGate, more efficient storage, etc
	public void addGate(LiveGate gate) {
		gates.add(gate);
	}
	
	public LiveGate getGate(int i) {
		return gates.get(index);
	}

	public int getNumGates() {
		return gates.size();
	}
}
