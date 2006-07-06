package org.omnetpp.experimental.animation.replay.model;

import org.omnetpp.experimental.animation.model.IRuntimeMessage;

public class ReplayMessage implements IRuntimeMessage {
	private String name;

	private int msgkind;
	private int priority;
	private long length;
	private boolean error;

//	cMessage *encapmsg;
//	cPolymorphic *ctrlp;
//	void *contextptr;  

	private int senderModuleId, senderGateId;
	private int arrivalModuleId, arrivalGateId;
	private double sendingTime, arrivalTime;

	/* (non-Javadoc)
	 * @see org.omnetpp.experimental.animation.replay.model.Buu#isSelfMessage()
	 */
	public boolean isSelfMessage() {
		return arrivalGateId==-1;
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.experimental.animation.replay.model.Buu#getSenderModuleId()
	 */
	public int getSenderModuleId() {
		return senderModuleId;
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.experimental.animation.replay.model.Buu#getSenderGateId()
	 */
	public int getSenderGateId() {
		return senderGateId;
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.experimental.animation.replay.model.Buu#getArrivalModuleId()
	 */
	public int getArrivalModuleId() {
		return arrivalModuleId;
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.experimental.animation.replay.model.Buu#getArrivalGateId()
	 */
	public int getArrivalGateId() {
		return arrivalGateId;
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.experimental.animation.replay.model.Buu#getSendingTime()
	 */
	public double getSendingTime()  {
		return sendingTime;
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.experimental.animation.replay.model.Buu#getArrivalTime()
	 */
	public double getArrivalTime() {
		return arrivalTime;
	}
}
