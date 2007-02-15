package org.omnetpp.eventlogtable.widgets;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.draw2d.ColorConstants;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.virtualtable.IVirtualTableItemProvider;
import org.omnetpp.eventlog.engine.BeginSendEntry;
import org.omnetpp.eventlog.engine.CancelEventEntry;
import org.omnetpp.eventlog.engine.DeleteMessageEntry;
import org.omnetpp.eventlog.engine.EndSendEntry;
import org.omnetpp.eventlog.engine.Event;
import org.omnetpp.eventlog.engine.EventEntry;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.EventLogMessage;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.MessageDependency;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.ModuleMethodBeginEntry;
import org.omnetpp.eventlog.engine.ModuleMethodEndEntry;
import org.omnetpp.eventlog.engine.PStringVector;
import org.omnetpp.eventlog.engine.SendHopEntry;

public class EventLogTableItemProvider extends LabelProvider implements IVirtualTableItemProvider {
	public enum DisplayMode {
		DESCRIPTIVE,
		RAW
	}

	protected DisplayMode displayMode = DisplayMode.DESCRIPTIVE;

	protected IResource resource;

	public DisplayMode getDisplayMode() {
		return displayMode;
	}
	
	public void setDisplayMode(DisplayMode displayMode) {
		this.displayMode = displayMode;
	}
	
	public IResource getResource() {
		return resource;
	}
	
	public void setResource(IResource resource) {
		this.resource = resource;
	}

	public void fillTableItem(TableItem item, Object element) {
		EventLogEntry eventLogEntry = (EventLogEntry)element;
		Event event = eventLogEntry.getEvent();
		IEventLog eventLog = event.getEventLog();

		try {
			IResource resource = getResource();
			IMarker[] markers = resource.findMarkers(IMarker.BOOKMARK, true, IResource.DEPTH_ZERO);
			boolean marked = false;
			for (int i = 0; i < markers.length; i++)
				if (markers[i].getAttribute("EventNumber", -1) == event.getEventNumber()) {
					marked = true;
					break;
				}
			if (marked)
				item.setBackground(ColorFactory.asColor("lightCyan"));
		}
		catch (CoreException e) {
		}

		if (eventLogEntry instanceof EventEntry) {
			MessageDependency cause = event.getCause();
			
			item.setText(0, "#" + event.getEventNumber());
			item.setForeground(0, ColorConstants.red);
			item.setText(1, event.getSimulationTime() + "s"); 
			item.setForeground(1, ColorConstants.black);

			String s;
			switch (displayMode) {
				case DESCRIPTIVE:
					s = "in " + getModuleDescription(eventLog.getModuleCreatedEntry(event.getModuleId()));
					
					BeginSendEntry beginSendEntry = cause != null ? cause.getCauseBeginSendEntry() : null;
					if (beginSendEntry != null) {
						s += " on arrival of message (" + beginSendEntry.getMessageClassName() + ") " + beginSendEntry.getMessageFullName() +
					         " from " + getModuleDescription(eventLog.getModuleCreatedEntry(beginSendEntry.getContextModuleId()));
						
						IEvent causeEvent = cause.getCauseEvent();
						if (causeEvent != null && causeEvent.getModuleId() != beginSendEntry.getContextModuleId())
							s += " called from " + getModuleDescription(eventLog.getModuleCreatedEntry(causeEvent.getModuleId()));
					}
					break;
				case RAW:
					s = getRawEntry(eventLogEntry);
					break;
				default:
					throw new RuntimeException("Unknown display mode");
			}

			item.setText(2, s);
			item.setForeground(2, ColorConstants.blue);
		}
		else {
			item.setText(0, "");
			item.setText(1, "");

			String s;

			switch (displayMode) {
				case DESCRIPTIVE:
					if (eventLogEntry instanceof EventLogMessage) {
						EventLogMessage eventLogMessage = (EventLogMessage)eventLogEntry;
						s = eventLogMessage.getText();
					}
					else {
						if (eventLogEntry instanceof ModuleMethodBeginEntry) {
							s = "Begin calling module ...";
						}
						else if (eventLogEntry instanceof ModuleMethodEndEntry) {
							s = "End calling module ...";
						}
						else if (eventLogEntry instanceof BeginSendEntry) {
							s = "Begin sending of ...";
						}
						else if (eventLogEntry instanceof SendHopEntry) {
							s = "Sending through ...";
						}
						else if (eventLogEntry instanceof EndSendEntry) {
							s = "End sending at ...";
						}
						else if (eventLogEntry instanceof CancelEventEntry) {
							s = "Cancelling event ...";
						}
						else if (eventLogEntry instanceof DeleteMessageEntry) {
							s = "Deleting message ...";
						}
						else
							s = "Not yet implemented";
					}
					break;
				case RAW:
					s = getRawEntry(eventLogEntry);
					break;
				default:
					throw new RuntimeException("Unknown display mode");
			}

			item.setForeground(2, eventLogEntry instanceof EventLogMessage ? ColorConstants.black : ColorConstants.darkGreen);

			StringBuilder indent = new StringBuilder();
			for (int i = 0; i < eventLogEntry.getLevel(); i++)
				indent.append("   ");
			item.setText(2, indent + s);
		}
	}
	
	private String getModuleDescription(ModuleCreatedEntry moduleCreatedEntry) {
		if (moduleCreatedEntry != null)
			return "module (" + moduleCreatedEntry.getModuleClassName() + ") " + moduleCreatedEntry.getFullName(); // TODO: full path
		else
			return "module <unknown>";
	}
	
	private String getRawEntry(EventLogEntry eventLogEntry) {
		StringBuilder builder = new StringBuilder();

		if (!(eventLogEntry instanceof EventLogMessage)) {
			builder.append(eventLogEntry.getDefaultAttribute());
			builder.append(" ");
		}

		PStringVector stringVector = eventLogEntry.getAttributeNames();
		for (int i = 0; i < stringVector.size(); i++)
		{
			String name = stringVector.get(i);
			builder.append(name);
			builder.append(" ");
			builder.append(eventLogEntry.getAttribute(name));
			builder.append(" ");
		}

		return builder.toString();
	}
}
