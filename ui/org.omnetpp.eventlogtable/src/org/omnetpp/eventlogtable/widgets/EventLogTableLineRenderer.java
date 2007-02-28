package org.omnetpp.eventlogtable.widgets;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.virtualtable.IVirtualTableLineRenderer;
import org.omnetpp.eventlog.engine.BeginSendEntry;
import org.omnetpp.eventlog.engine.BubbleEntry;
import org.omnetpp.eventlog.engine.CancelEventEntry;
import org.omnetpp.eventlog.engine.ConnectionCreatedEntry;
import org.omnetpp.eventlog.engine.ConnectionDeletedEntry;
import org.omnetpp.eventlog.engine.ConnectionDisplayStringChangedEntry;
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
import org.omnetpp.eventlog.engine.ModuleDeletedEntry;
import org.omnetpp.eventlog.engine.ModuleDisplayStringChangedEntry;
import org.omnetpp.eventlog.engine.ModuleMethodBeginEntry;
import org.omnetpp.eventlog.engine.ModuleMethodEndEntry;
import org.omnetpp.eventlog.engine.ModuleReparentedEntry;
import org.omnetpp.eventlog.engine.PStringVector;
import org.omnetpp.eventlog.engine.SendDirectEntry;
import org.omnetpp.eventlog.engine.SendHopEntry;

public class EventLogTableLineRenderer implements IVirtualTableLineRenderer<EventLogEntry> {
	public enum DisplayMode {
		DESCRIPTIVE,
		RAW
	}

	private static final Color BOOKMARK_COLOR = ColorFactory.asColor("lightCyan");

	private static final Color EVENT_ENTRY_EVENT_NUMBER_COLOR = ColorFactory.asColor("red");

	private static final Color EVENT_LOG_ENTRY_EVENT_NUMBER_COLOR = ColorFactory.asColor("lightPink");

	private static final Color EVENT_ENTRY_SIMULATION_TIME_COLOR = ColorFactory.asColor("black");

	private static final Color EVENT_LOG_ENTRY_SIMULATION_TIME_COLOR = ColorFactory.asColor("lightGrey");

	private static final Color CONSTANT_TEXT_COLOR = ColorFactory.asColor("black");

	private static final Color RAW_VALUE_COLOR = ColorFactory.asColor("darkGreen");

	private static final Color TYPE_COLOR = ColorFactory.asColor("blue");

	private static final Color NAME_COLOR = ColorFactory.asColor("blue");

	private static final Color EVENT_LOG_MESSAGE_COLOR = ColorFactory.asColor("darkRed");

	private static final Color BUBBLE_ENTRY_COLOR = ColorFactory.asColor("red");

	private static final Color DATA_COLOR = ColorFactory.asColor("darkGreen");

	private static final int HORIZONTAL_SPACING = 4;

	private static final int VERTICAL_SPACING = 1;

	protected DisplayMode displayMode = DisplayMode.DESCRIPTIVE;

	protected IResource resource;

	protected Font font = JFaceResources.getTextFont();

	protected int fontHeight;

	/**
	 * The current GC we are drawing to.
	 */
	protected GC gc;
	
	/**
	 * The next x position where drawing continues.
	 */
	protected int x;

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

	public int getLineHeight(GC gc) {
		if (fontHeight == 0) {
			Font oldFont = gc.getFont();
			gc.setFont(font);
			fontHeight = gc.getFontMetrics().getHeight();
			gc.setFont(oldFont);
		}

		return fontHeight + VERTICAL_SPACING;
	}

	public void drawCell(GC gc, EventLogEntry element, int index) {
		Assert.isTrue(element != null);
		
		this.x = HORIZONTAL_SPACING;
		this.gc = gc;
		gc.setAntialias(SWT.OFF);

		EventLogEntry eventLogEntry = (EventLogEntry)element;
		Event event = eventLogEntry.getEvent();
		IEventLog eventLog = event.getEventLog();
		boolean isEventLogEntry = eventLogEntry instanceof EventEntry;

		try {
			if (resource != null) {
				IMarker[] markers = resource.findMarkers(IMarker.BOOKMARK, true, IResource.DEPTH_ZERO);
				boolean marked = false;
				for (int i = 0; i < markers.length; i++)
					if (markers[i].getAttribute("EventNumber", -1) == event.getEventNumber()) {
						marked = true;
						break;
					}
				
				if (marked && 
					(gc.getBackground().equals(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_BACKGROUND)) ||
					 gc.getBackground().equals(BOOKMARK_COLOR))) {
					gc.setBackground(BOOKMARK_COLOR);
					gc.fillRectangle(gc.getClipping());
				}
			}
		}
		catch (CoreException e) {
		}
		
		switch (index) {
			case 0:
				drawText("#" + event.getEventNumber(), isEventLogEntry ? EVENT_ENTRY_EVENT_NUMBER_COLOR : EVENT_LOG_ENTRY_EVENT_NUMBER_COLOR, true);
				break;
			case 1:
				drawText(event.getSimulationTime() + "s", isEventLogEntry ? EVENT_ENTRY_SIMULATION_TIME_COLOR : EVENT_LOG_ENTRY_SIMULATION_TIME_COLOR, true); 
				break;
			case 2:
				StringBuilder indent = new StringBuilder();
				for (int i = 0; i < eventLogEntry.getLevel(); i++)
					indent.append("   ");
				drawText(indent.toString());

				Image image = getEventLogEntryImage(eventLogEntry);
				gc.drawImage(image, x, 0);
				x += image.getBounds().width + HORIZONTAL_SPACING;
				
				switch (displayMode) {
					case DESCRIPTIVE:
						if (eventLogEntry instanceof EventEntry) {
							MessageDependency cause = event.getCause();
				
							drawText("in ", CONSTANT_TEXT_COLOR);
							drawModuleDescription(eventLog.getModuleCreatedEntry(event.getModuleId()));
							
							BeginSendEntry beginSendEntry = cause != null ? cause.getCauseBeginSendEntry() : null;
							if (beginSendEntry != null) {
								drawText(" on arrival of message ", CONSTANT_TEXT_COLOR);
								drawMessageDescription(beginSendEntry);
								drawText(" from ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(eventLog.getModuleCreatedEntry(beginSendEntry.getContextModuleId()));
								
								IEvent causeEvent = cause.getCauseEvent();
								if (causeEvent != null && causeEvent.getModuleId() != beginSendEntry.getContextModuleId()) {
									drawText(" called from ", CONSTANT_TEXT_COLOR);
									drawModuleDescription(eventLog.getModuleCreatedEntry(causeEvent.getModuleId()));
								}
							}
						}
						else {
							if (eventLogEntry instanceof EventLogMessage) {
								EventLogMessage eventLogMessage = (EventLogMessage)eventLogEntry;
								drawText(eventLogMessage.getText(), EVENT_LOG_MESSAGE_COLOR);
							}
							else if (eventLogEntry instanceof BubbleEntry) {
								BubbleEntry bubbleEntry = (BubbleEntry)eventLogEntry;
								drawText("Bubble in ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(eventLog.getModuleCreatedEntry(bubbleEntry.getContextModuleId()));
								drawText(" : ", CONSTANT_TEXT_COLOR);
								drawText(bubbleEntry.getText(), BUBBLE_ENTRY_COLOR);
							}
							else if (eventLogEntry instanceof ModuleMethodBeginEntry) {
								ModuleMethodBeginEntry moduleMethodBeginEntry = (ModuleMethodBeginEntry)eventLogEntry;
								drawText("Begin calling ", CONSTANT_TEXT_COLOR);
								drawText(moduleMethodBeginEntry.getMethod(), DATA_COLOR);
								drawText(" in ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(eventLog.getModuleCreatedEntry(moduleMethodBeginEntry.getToModuleId()));
								drawText(" from ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(eventLog.getModuleCreatedEntry(moduleMethodBeginEntry.getFromModuleId()));
							}
							else if (eventLogEntry instanceof ModuleMethodEndEntry) {
								drawText("End calling module", CONSTANT_TEXT_COLOR);
							}
							else if (eventLogEntry instanceof ModuleCreatedEntry) {
								// TODO:
								drawText("Module created...", CONSTANT_TEXT_COLOR);
							}
							else if (eventLogEntry instanceof ModuleDeletedEntry) {
								// TODO:
								drawText("Module deleted...", CONSTANT_TEXT_COLOR);
							}
							else if (eventLogEntry instanceof ModuleReparentedEntry) {
								// TODO:
								drawText("Module reparented...", CONSTANT_TEXT_COLOR);
							}
							else if (eventLogEntry instanceof ConnectionCreatedEntry) {
								// TODO:
								drawText("Connection created...", CONSTANT_TEXT_COLOR);
							}
							else if (eventLogEntry instanceof ConnectionDeletedEntry) {
								// TODO:
								drawText("Connection deleted...", CONSTANT_TEXT_COLOR);
							}
							else if (eventLogEntry instanceof ConnectionDisplayStringChangedEntry) {
								// TODO:
								drawText("Connection display string changed...", CONSTANT_TEXT_COLOR);
							}
							else if (eventLogEntry instanceof ModuleDisplayStringChangedEntry) {
								// TODO:
								drawText("Module display string changed...", CONSTANT_TEXT_COLOR);
							}
							else if (eventLogEntry instanceof CancelEventEntry) {
								// TODO:
								drawText("Cancelling event caused by message ...", CONSTANT_TEXT_COLOR);
							}
							else if (eventLogEntry instanceof BeginSendEntry) {
								// TODO: add message stuff
								BeginSendEntry beginSendEntry = (BeginSendEntry)eventLogEntry;
								drawText("Begin sending of ", CONSTANT_TEXT_COLOR);
								drawMessageDescription(beginSendEntry);
							}
							else if (eventLogEntry instanceof EndSendEntry) {
								EndSendEntry endSendEntry = (EndSendEntry)eventLogEntry;
								drawText("End sending at ", CONSTANT_TEXT_COLOR);
								drawText(endSendEntry.getArrivalTime() + "s", DATA_COLOR);
							}
							else if (eventLogEntry instanceof SendHopEntry) {
								// TODO: add senderGateId
								SendHopEntry sendHopEntry = (SendHopEntry)eventLogEntry;
								drawText("Sending from ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(eventLog.getModuleCreatedEntry(sendHopEntry.getSenderModuleId()));
								drawText(" with transmission delay ", CONSTANT_TEXT_COLOR);
								drawText(sendHopEntry.getTransmissionDelay() + "s", DATA_COLOR);
								drawText(" and propagation delay ", CONSTANT_TEXT_COLOR);
								drawText(sendHopEntry.getPropagationDelay() + "s", DATA_COLOR);
							}
							else if (eventLogEntry instanceof SendDirectEntry) {
								// TODO: add destGateId
								SendDirectEntry sendDirectEntry = (SendDirectEntry)eventLogEntry;
								drawText("Sending direct message from ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(eventLog.getModuleCreatedEntry(sendDirectEntry.getSenderModuleId()));
								drawText(" to ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(eventLog.getModuleCreatedEntry(sendDirectEntry.getDestModuleId()));
								drawText(" with transmission delay ", CONSTANT_TEXT_COLOR);
								drawText(sendDirectEntry.getTransmissionDelay() + "s", DATA_COLOR);
								drawText(" and propagation delay ", CONSTANT_TEXT_COLOR);
								drawText(sendDirectEntry.getPropagationDelay() + "s", DATA_COLOR);
							}
							else if (eventLogEntry instanceof DeleteMessageEntry) {
								// TODO:
								drawText("Deleting message ...", CONSTANT_TEXT_COLOR);
							}
							else
								throw new RuntimeException("Unknown event log entry: " + eventLogEntry.getClassName());
						}
						break;
					case RAW:
						drawRawEntry(eventLogEntry);
						break;
					default:
						throw new RuntimeException("Unknown display mode");
				}
		}
	}

	private void drawModuleDescription(ModuleCreatedEntry moduleCreatedEntry) {
		if (moduleCreatedEntry != null) {
			drawText("module ", CONSTANT_TEXT_COLOR);
			drawText("(" + moduleCreatedEntry.getModuleClassName() + ") ", TYPE_COLOR);
			drawText(moduleCreatedEntry.getFullName(), NAME_COLOR, true); // TODO: full path
		}
		else
			drawText("module <unknown>", CONSTANT_TEXT_COLOR);
	}
	
	private void drawMessageDescription(BeginSendEntry beginSendEntry) {
		drawText("(" + beginSendEntry.getMessageClassName() + ") ", TYPE_COLOR);
		drawText(beginSendEntry.getMessageFullName(), NAME_COLOR, true);
	}
	
	private void drawRawEntry(EventLogEntry eventLogEntry) {

		if (!(eventLogEntry instanceof EventLogMessage)) {
			drawText(eventLogEntry.getDefaultAttribute() + " ", CONSTANT_TEXT_COLOR, true);
		}

		PStringVector stringVector = eventLogEntry.getAttributeNames();
		for (int i = 0; i < stringVector.size(); i++)
		{
			String name = stringVector.get(i);
			drawText(name + " ", CONSTANT_TEXT_COLOR);
			drawText(eventLogEntry.getAttribute(name) + " ", RAW_VALUE_COLOR);
		}
	}
	private void drawText(String text) {
		drawText(text, null, false);
	}
	
	private void drawText(String text, Color color) {
		drawText(text, color, false);
	}
	
	private void drawText(String text, Color color, boolean bold) {
		Font oldFont = gc.getFont();
		FontData fontData = font.getFontData()[0];
		Font newFont = new Font(oldFont.getDevice(), fontData.getName(), fontData.getHeight(), bold ? SWT.BOLD : SWT.NORMAL);
		gc.setFont(newFont);

		if (color != null)
			gc.setForeground(color);

		gc.drawText(text, x, 0);
		x += gc.textExtent(text).x;
		newFont.dispose();
		gc.setFont(oldFont);
	}
	
	private Image getEventLogEntryImage(EventLogEntry eventLogEntry) {
		String className = eventLogEntry.getClassName();
		return ImageFactory.getImage(ImageFactory.EVENTLOG_IMAGE_DIR + className.substring(0, className.length() - 5));
	}

}
