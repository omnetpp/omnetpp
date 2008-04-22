package org.omnetpp.common.eventlog;

import java.io.Serializable;
import java.math.BigDecimal;

import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.IntVector;
import org.omnetpp.eventlog.engine.LongVector;
import org.omnetpp.eventlog.engine.StringVector;

public class EventLogFilterParameters implements Serializable {
	private static final long serialVersionUID = 1L;

	private transient EventLogInput eventLogInput;
    
    public boolean enableCollectionLimits;
	
	public boolean enableEventNumberFilter;

	public boolean enableSimulationTimeFilter;

	public boolean enableModuleFilter;

	public boolean enableModuleExpressionFilter;

	public boolean enableModuleClassNameFilter;

	public boolean enableModuleNameFilter;

	public boolean enableModuleIdFilter;

    public boolean enableMessageFilter;

	public boolean enableMessageExpressionFilter;

	public boolean enableMessageClassNameFilter;

	public boolean enableMessageNameFilter;

	public boolean enableMessageIdFilter;

	public boolean enableMessageTreeIdFilter;

	public boolean enableMessageEncapsulationIdFilter;

	public boolean enableMessageEncapsulationTreeIdFilter;

	public boolean enableTraceFilter;

	public int lowerEventNumberLimit = -1;

	public int upperEventNumberLimit = -1;
	
	public BigDecimal lowerSimulationTimeLimit;
	
	public BigDecimal upperSimulationTimeLimit;

	public int tracedEventNumber = -1;

	public boolean traceCauses = true;

	public boolean traceConsequences = true;

	public boolean traceMessageReuses = false;

	public boolean traceSelfMessages = true;

	public int causeEventNumberDelta = 1000;

	public int consequenceEventNumberDelta = 1000;

	public BigDecimal causeSimulationTimeDelta;

	public BigDecimal consequenceSimulationTimeDelta;

	public String moduleFilterExpression;

	public String[] moduleClassNames;

	public int[] moduleIds;
	
	public int[] moduleNameIds;
	
	public String[] messageClassNames;

	public String[] messageNames;

	public String messageFilterExpression;

	public int[] messageIds;

	public int[] messageTreeIds;

	public int[] messageEncapsulationIds;

	public int[] messageEncapsulationTreeIds;

	public boolean collectMessageReuses = false;
    
    public int maximumNumberOfMessageDependencies = 10;
	
	public int maximumDepthOfMessageDependencies = 30;

	public EventLogFilterParameters(EventLogInput eventLogInput) {
		this.eventLogInput = eventLogInput;
	}
	
	public void setEventLogInput(EventLogInput eventLogInput) {
		this.eventLogInput = eventLogInput;
	}

	public int getFirstEventNumber() {
		IEventLog eventLog = eventLogInput.getEventLog();

		int lowerTraceEventNumberLimit1 = -1;
		if (enableTraceFilter && tracedEventNumber != -1 && causeEventNumberDelta != -1)
			lowerTraceEventNumberLimit1 = tracedEventNumber - causeEventNumberDelta;

		int lowerTraceEventNumberLimit2 = -1;
		if (enableTraceFilter && tracedEventNumber != -1 && causeSimulationTimeDelta != null) {
			IEvent event = eventLog.getEventForEventNumber(tracedEventNumber);
			event = eventLog.getLastEventNotAfterSimulationTime(new org.omnetpp.common.engine.BigDecimal(event.getSimulationTime().doubleValue() - causeSimulationTimeDelta.doubleValue()));

			if (event != null)
				lowerTraceEventNumberLimit2 = event.getEventNumber();
		}

		int lowerEventNumberForSimulationTimeLimit = -1;
		if (enableSimulationTimeFilter && lowerSimulationTimeLimit != null) {
			IEvent event = eventLog.getLastEventNotAfterSimulationTime(org.omnetpp.common.engine.BigDecimal.parse(lowerSimulationTimeLimit.toPlainString()));

			if (event != null)
				lowerEventNumberForSimulationTimeLimit = event.getEventNumber();
		}

		int lowerEventNumberLimit = -1;
		if (enableEventNumberFilter && this.lowerEventNumberLimit != -1)
			lowerEventNumberLimit = this.lowerEventNumberLimit;

		return Math.max(Math.max(lowerEventNumberLimit, lowerEventNumberForSimulationTimeLimit),
						Math.max(lowerTraceEventNumberLimit1, lowerTraceEventNumberLimit2));
	}

	public int getLastEventNumber() {
		IEventLog eventLog = eventLogInput.getEventLog();

		int upperTraceEventNumberLimit1 = Integer.MAX_VALUE;
		if (enableTraceFilter && tracedEventNumber != -1 && consequenceEventNumberDelta != -1)
			upperTraceEventNumberLimit1 = tracedEventNumber + consequenceEventNumberDelta;

		int upperTraceEventNumberLimit2 = Integer.MAX_VALUE;
		if (enableTraceFilter && tracedEventNumber != -1 && consequenceSimulationTimeDelta != null) {
			IEvent event = eventLog.getEventForEventNumber(tracedEventNumber);
			event = eventLog.getFirstEventNotBeforeSimulationTime(new org.omnetpp.common.engine.BigDecimal(event.getSimulationTime().doubleValue() + consequenceSimulationTimeDelta.doubleValue()));

			if (event != null)
				upperTraceEventNumberLimit2 = event.getEventNumber();
		}

		int upperEventNumberForSimulationTimeLimit = Integer.MAX_VALUE;
		if (enableSimulationTimeFilter && upperSimulationTimeLimit != null) {
			IEvent event = eventLog.getFirstEventNotBeforeSimulationTime(org.omnetpp.common.engine.BigDecimal.parse(upperSimulationTimeLimit.toPlainString()));

			if (event != null)
				upperEventNumberForSimulationTimeLimit = event.getEventNumber();
		}

		int upperEventNumberLimit = Integer.MAX_VALUE;
		if (enableEventNumberFilter && this.upperEventNumberLimit != -1)
			upperEventNumberLimit = this.upperEventNumberLimit;

		int limit = Math.min(Math.min(upperEventNumberLimit, upperEventNumberForSimulationTimeLimit),
				             Math.min(upperTraceEventNumberLimit1, upperTraceEventNumberLimit2));
		
		if (limit == Integer.MAX_VALUE)
			return -1;
		else
			return limit;
	}

	public IntVector getModuleIds() {
		IntVector vector = new IntVector();
		
		if (enableModuleNameFilter)
    		for (int id : moduleNameIds)
    			vector.add(id);

		if (enableModuleIdFilter)
    		for (int id : moduleIds)
    			vector.add(id);

		return vector;
	}

	public StringVector getModuleClassNames() {
		return createStringVector(moduleClassNames);
	}

	public StringVector getMessageClassNames() {
		return createStringVector(messageClassNames);
	}

	public StringVector getMessageNames() {
		return createStringVector(messageNames);
	}

	public LongVector getMessageIds() {
		if (enableMessageIdFilter)
			return createLongVector(messageIds);
		else
			return new LongVector();
	}

	public LongVector getMessageTreeIds() {
		if (enableMessageTreeIdFilter)
			return createLongVector(messageTreeIds);
		else
			return new LongVector();
	}

	public LongVector getMessageEncapsulationIds() {
		if (enableMessageEncapsulationIdFilter)
			return createLongVector(messageEncapsulationIds);
		else
			return new LongVector();
	}

	public LongVector getMessageEcapsulationTreeIds() {
		if (enableMessageEncapsulationTreeIdFilter)
			return createLongVector(messageEncapsulationTreeIds);
		else
			return new LongVector();
	}

	public IntVector createIntVector(int[] ids) {
		IntVector vector = new IntVector();
		
		for (int id : ids)
			vector.add(id);

		return vector;
	}

	public LongVector createLongVector(int[] ids) {
		LongVector vector = new LongVector();
		
		for (int id : ids)
			vector.add(id);

		return vector;
	}

	public StringVector createStringVector(String[] names) {
		StringVector vector = new StringVector();
		
		for (String name : names)
			vector.add(name);

		return vector;
	}
}
