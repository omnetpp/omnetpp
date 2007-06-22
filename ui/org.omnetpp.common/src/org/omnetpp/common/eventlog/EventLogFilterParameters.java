package org.omnetpp.common.eventlog;

import java.io.Serializable;
import java.math.BigDecimal;
import java.util.ArrayList;

import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;

public class EventLogFilterParameters implements Serializable {
	private static final long serialVersionUID = 1L;

	private transient EventLogInput eventLogInput;
	
	public boolean enableEventNumberFilter;

	public boolean enableSimulationTimeFilter;

	public boolean enableModuleFilter;

	public boolean enableMessageFilter;

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

	public String moduleFilterPattern;

	public String[] moduleTypes;

	public int[] moduleIds;

	public String messageFilterPattern;

	public int[] messageIds;

	public int[] messageTreeIds;

	public int[] messageEncapsulationIds;

	public int[] messageEncapsulationTreeIds;

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

	public ArrayList<ModuleTreeItem> getSelectedModules() {
		ArrayList<ModuleTreeItem> selectedModules = new ArrayList<ModuleTreeItem>();
		
		for (int id : moduleIds)
			selectedModules.add(eventLogInput.getModuleTreeRoot().findDescendantModule(id));

		return selectedModules;
	}
}
