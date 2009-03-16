/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.replay.model;

import java.math.BigDecimal;

import org.omnetpp.common.simulation.model.IRuntimeMessage;

public class ReplayMessage implements IRuntimeMessage {
	private String name;
	private String className;

	private int kind;
	private int priority;
	private int length;
	private boolean error;

	private int id;
	private int treeId;
	private int encapsulationId;
	private int encapsulationTreeId;
	
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

	public int getLength() {
		return length;
	}

	public void setLength(int length) {
		this.length = length;
	}

	public short getKind() {
		return kind;
	}

	public void setKind(int kind) {
		this.kind = kind;
	}

	public short getSchedulingPriority() {
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

	public BigDecimal getSendingTime()  {
		return sendingTime;
	}

	public BigDecimal getArrivalTime() {
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

	public int getEncapsulationId() {
		return encapsulationId;
	}

	public void setEncapsulationId(int encapsulationId) {
		this.encapsulationId = encapsulationId;
	}

	public int getEncapsulationTreeId() {
		return encapsulationTreeId;
	}

	public void setEncapsulationTreeId(int encapsulationTreeId) {
		this.encapsulationTreeId = encapsulationTreeId;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public int getTreeId() {
		return treeId;
	}

	public void setTreeId(int treeId) {
		this.treeId = treeId;
	}

	public String getClassName() {
		return className;
	}

	public void setClassName(String className) {
		this.className = className;
	}

}
