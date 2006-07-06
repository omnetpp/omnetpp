package org.omnetpp.experimental.animation.live.model;

import org.omnetpp.experimental.animation.model.IRuntimeMessage;

public class LiveMessage implements IRuntimeMessage {
	private String name;

	private int kind;
	private int priority;
	private long length;
	private boolean error;

//	cMessage *encapmsg;
//	cPolymorphic *ctrlp;
//	void *contextptr;  

	private int senderModuleId, senderGateId;
	private int arrivalModuleId, arrivalGateId;
	private double sendingTime, arrivalTime;

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public boolean hasBitError() {
		return error;
	}

	public void setBitError(boolean error) {
		this.error = error;
	}

	public long getLength() {
		return length;
	}

	public void setLength(long length) {
		this.length = length;
	}

	public int getKind() {
		return kind;
	}

	public void setMsgkind(int msgkind) {
		this.kind = msgkind;
	}

	public int getPriority() {
		return priority;
	}

	public void setPriority(int priority) {
		this.priority = priority;
	}

	public boolean isSelfMessage() {
		return arrivalGateId==-1;
	}

	public int getSenderModuleId() {
		return senderModuleId;
	}

	public int getSenderGateId() {
		return senderGateId;
	}

	public int getArrivalModuleId() {
		return arrivalModuleId;
	}

	public int getArrivalGateId() {
		return arrivalGateId;
	}

	public double getSendingTime()  {
		return sendingTime;
	}

	public double getArrivalTime() {
		return arrivalTime;
	}

	public void setArrivalGateId(int arrivalGateId) {
		this.arrivalGateId = arrivalGateId;
	}

	public void setArrivalModuleId(int arrivalModuleId) {
		this.arrivalModuleId = arrivalModuleId;
	}

	public void setArrivalTime(double arrivalTime) {
		this.arrivalTime = arrivalTime;
	}

	public void setSenderGateId(int senderGateId) {
		this.senderGateId = senderGateId;
	}

	public void setSenderModuleId(int senderModuleId) {
		this.senderModuleId = senderModuleId;
	}

	public void setSendingTime(double sendingTime) {
		this.sendingTime = sendingTime;
	}
}
