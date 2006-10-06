package org.omnetpp.experimental.seqchart.widgets;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.eventlog.engine.BeginSendEntry;
import org.omnetpp.eventlog.engine.Event;
import org.omnetpp.eventlog.engine.EventEntry;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.EventLogMessage;
import org.omnetpp.eventlog.engine.MessageDependency;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;

public class EventLogVirtualTableItemProvider extends LabelProvider implements IVirtualTableItemProvider {
	public void fillTableItem(TableItem item, Object element) {
		EventLogEntry eventLogEntry = (EventLogEntry)element;
		Event event = eventLogEntry.getEvent();
		EventEntry eventEntry = eventLogEntry instanceof EventEntry ? (EventEntry)eventLogEntry : null;
		ModuleCreatedEntry moduleCreatedEntry = event.getEventLog().getModuleCreatedEntry(event.getModuleId());
		MessageDependency cause = event.getCause();
		BeginSendEntry beginSendEntry = cause != null ? cause.getCauseBeginSendEntry() : null;

		if (eventEntry != null) {
			item.setForeground(ColorConstants.blue);
			item.setText(0, "#" + event.getEventNumber());
			item.setText(1, event.getSimulationTime() + "s");
			String s = "module (" + moduleCreatedEntry.getModuleClassName() + ") " + moduleCreatedEntry.getFullName(); // TODO: full path

			if (beginSendEntry == null)
				item.setText(2, s);
			else
				item.setText(2, s + ", on arrival of message (" + beginSendEntry.getMessageClassName() + ") " + beginSendEntry.getMessageFullName());
		}
		else {
			EventLogMessage eventLogMessage = (EventLogMessage)eventLogEntry;
			item.setText(0, "");
			item.setText(1, "");
			item.setText(2, eventLogMessage.getText());
		}
	}
}