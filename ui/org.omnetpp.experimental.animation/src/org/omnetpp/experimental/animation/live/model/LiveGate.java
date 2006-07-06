package org.omnetpp.experimental.animation.live.model;

import org.omnetpp.experimental.animation.model.IRuntimeGate;

//XXX should be Java wrapper around cGate
public class LiveGate extends LiveMessage implements IRuntimeGate {
	private int id;
	private String name;
	private int index;
	private int size;
	private LiveModule ownerModule;

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
		return ownerModule==null ? getFullName() : ownerModule.getFullPath()+"."+getFullName();
	}

	public LiveModule getOwnerModule() {
		return ownerModule;
	}
	
}
