/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.simulation.model;


public interface IRuntimeMessage {
    /**
     * Returns message kind.
     */
    public short getKind();

    /**
     * Returns message priority.
     */
    public short getSchedulingPriority();

    /**
     * Returns message length (bits).
     */
//XXX    public int getBitLength();

    /**
     * Returns true if bit error flag is set, false otherwise.
     */
//XXX    public boolean hasBitError();

	/**
	 * Return true if message was posted by scheduleAt().
	 */
	public boolean isSelfMessage();

	/**
	 * Returns sender module's index in the module vector or -1 if the
	 * message hasn't been sent/scheduled yet.
	 */
	public int getSenderModuleId();

	/**
	 * Returns index of gate sent through in the sender module or -1
	 * if the message hasn't been sent/scheduled yet.
	 */
	public int getSenderGateId();

	/**
	 * Returns receiver module's index in the module vector or -1 if
	 * the message hasn't been sent/scheduled yet.
	 */
	public int getArrivalModuleId();

	/**
	 * Returns index of gate the message arrived on in the sender module
	 * or -1 if the message hasn't sent/scheduled yet.
	 */
	public int getArrivalGateId();

	/**
	 * Returns time when the message was sent/scheduled or 0 if the message
	 * hasn't been sent yet.
	 */
	public double getSendingTime();

	/**
	 * Returns time when the message arrived (or will arrive if it
	 * is currently scheduled or is underway), or 0 if the message
	 * hasn't been sent/scheduled yet.
	 */
	public double getArrivalTime();
	
    /**
     * Returns a unique message identifier assigned upon message creation.
     */
	public int getId();

    /**
     * Returns an identifier which is shared among a message object and all messages
     * created by copying it (i.e. by dup() or the copy constructor).
     */
	public int getTreeId();

    /**
     * Returns the sequenceId() of the innermost encapsulated message.
     */
 	public int getEncapsulationId();

    /**
     * Returns the treeId() of the innermost encapsulated message.
     */
	public int getEncapsulationTreeId();

	// FIXME: IObject?
	public String getName();

	// FIXME: IObject?
	public String getClassName();
}
