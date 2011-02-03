/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.eventlog;

import org.omnetpp.common.engine.BigDecimal;


public class EventLogMessage {
	private String name;
	private String className;

	private short kind;
	private short priority;
	private int bitLength;
	private boolean bitError;

	private int id;
	private int treeId;
	private int encapsulationId;
	private int encapsulationTreeId;

	private EventLogModule senderModule;
	private EventLogGate senderGate;
	private EventLogModule arrivalModule;
	private EventLogGate arrivalGate;
	private BigDecimal sendingTime, arrivalTime;

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public boolean hasBitError() {
		return bitError;
	}

	public void setBitError(boolean error) {
		this.bitError = error;
	}

	public int getBitLength() {
		return bitLength;
	}

	public void setBitLength(int length) {
		this.bitLength = length;
	}

	public short getKind() {
		return kind;
	}

	public void setKind(short kind) {
		this.kind = kind;
	}

	public short getSchedulingPriority() {
		return priority;
	}

	public void setSchedulingPriority(short priority) {
		this.priority = priority;
	}

	public boolean isSelfMessage() {
		return arrivalGate == null;
	}

	public EventLogModule getSenderModule() {
		return senderModule;
	}

	public EventLogGate getSenderGate() {
		return senderGate;
	}

	public EventLogModule getArrivalModule() {
		return arrivalModule;
	}

	public EventLogGate getArrivalGate() {
		return arrivalGate;
	}

	public BigDecimal getSendingTime()  {
		return sendingTime;
	}

	public BigDecimal getArrivalTime() {
		return arrivalTime;
	}

	public void setArrivalGate(EventLogGate arrivalGate) {
		this.arrivalGate = arrivalGate;
	}

	public void setArrivalModule(EventLogModule arrivalModule) {
		this.arrivalModule = arrivalModule;
	}

	public void setArrivalTime(BigDecimal arrivalTime) {
		this.arrivalTime = arrivalTime;
	}

	public void setSenderGate(EventLogGate senderGate) {
		this.senderGate = senderGate;
	}

	public void setSenderModule(EventLogModule senderModule) {
		this.senderModule = senderModule;
	}

	public void setSendingTime(BigDecimal sendingTime) {
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
