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
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.TimeUtils;
import org.omnetpp.common.virtualtable.IVirtualTableRowRenderer;
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
import org.omnetpp.eventlog.engine.EventLogMessageEntry;
import org.omnetpp.eventlog.engine.GateCreatedEntry;
import org.omnetpp.eventlog.engine.GateDeletedEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IMessageDependency;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.ModuleDeletedEntry;
import org.omnetpp.eventlog.engine.ModuleDisplayStringChangedEntry;
import org.omnetpp.eventlog.engine.ModuleMethodBeginEntry;
import org.omnetpp.eventlog.engine.ModuleMethodEndEntry;
import org.omnetpp.eventlog.engine.ModuleReparentedEntry;
import org.omnetpp.eventlog.engine.PStringVector;
import org.omnetpp.eventlog.engine.SendDirectEntry;
import org.omnetpp.eventlog.engine.SendHopEntry;
import org.omnetpp.eventlog.engine.SimulationBeginEntry;
import org.omnetpp.eventlog.engine.SimulationEndEntry;
import org.omnetpp.eventlogtable.widgets.EventLogTable.NameMode;

public class EventLogTableRowRenderer implements IVirtualTableRowRenderer<EventLogEntryReference> {
	private static final Color DARKBLUE = new Color(null, 0, 0, 192);

	private static final Color DARKRED = new Color(null, 127, 0, 85);
	
	private static final Color RED = new Color(null, 240, 0, 0);
	
	private static final Color BLACK = new Color(null, 0, 0, 0);
	
	private static final Color LIGHTGREY = new Color(null, 211, 211, 211);
	
	private static final Color BOOKMARK_COLOR = ColorFactory.LIGHT_CYAN;

	private static final Color EVENT_ENTRY_EVENT_NUMBER_COLOR = BLACK;

	private static final Color EVENT_LOG_ENTRY_EVENT_NUMBER_COLOR = LIGHTGREY;

	private static final Color EVENT_ENTRY_SIMULATION_TIME_COLOR = BLACK;

	private static final Color EVENT_LOG_ENTRY_SIMULATION_TIME_COLOR = LIGHTGREY;

	private static final Color CONSTANT_TEXT_COLOR = BLACK;

	private static final Color RAW_VALUE_COLOR = DARKBLUE;

	private static final Color TYPE_COLOR = DARKBLUE;

	private static final Color NAME_COLOR = DARKBLUE;

	private static final Color EVENT_LOG_MESSAGE_COLOR = DARKRED;

	private static final Color BUBBLE_ENTRY_COLOR = RED;

	private static final Color DATA_COLOR = DARKBLUE;

	private static final int HORIZONTAL_SPACING = 4;

	private static final int INDENT_SPACING = HORIZONTAL_SPACING * 4;

	private static final int VERTICAL_SPACING = 3;
	
	protected EventLogInput eventLogInput;

	protected EventLogTable eventLogTable;

	protected Font font = JFaceResources.getDefaultFont();

	protected int fontHeight;

	/**
	 * The current GC we are drawing to.
	 */
	private GC gc;
	
	/**
	 * The next x position where drawing continues.
	 */
	private int x;
	
	private IEvent contextEvent;
	
	public EventLogTableRowRenderer(EventLogTable eventLogTable) {
	    this.eventLogTable = eventLogTable;
	}

	public void setInput(Object eventLogInput) {
		this.eventLogInput = (EventLogInput)eventLogInput;
	}

	public int getRowHeight(GC gc) {
		if (fontHeight == 0) {
			Font oldFont = gc.getFont();
			gc.setFont(font);
			fontHeight = gc.getFontMetrics().getHeight();
			gc.setFont(oldFont);
		}

		return fontHeight + VERTICAL_SPACING;
	}

	public void drawCell(GC gc, EventLogEntryReference eventLogEntryReference, int index) {
		Assert.isTrue(eventLogEntryReference != null);
		
		this.x = HORIZONTAL_SPACING;
		this.gc = gc;
		gc.setAntialias(SWT.OFF);

		EventLogEntry eventLogEntry = eventLogEntryReference.getEventLogEntry(eventLogInput);
		contextEvent = eventLogEntry.getEvent();
		int eventNumber = contextEvent.getEventNumber();
		BigDecimal simulationTime = contextEvent.getSimulationTime();
		boolean isEventLogEntry = eventLogEntry instanceof EventEntry;

		try {
			if (eventLogInput.getFile() != null) {
				IMarker[] markers = eventLogInput.getFile().findMarkers(IMarker.BOOKMARK, true, IResource.DEPTH_ZERO);
				boolean marked = false;
				for (int i = 0; i < markers.length; i++)
					if (markers[i].getAttribute("EventNumber", -1) == eventNumber) {
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
			throw new RuntimeException(e);
		}
		
		switch (index) {
			case 0:
				drawText("#" + eventNumber, isEventLogEntry ? EVENT_ENTRY_EVENT_NUMBER_COLOR : EVENT_LOG_ENTRY_EVENT_NUMBER_COLOR, false);
				break;
			case 1:
				drawText(simulationTime + "s", isEventLogEntry ? EVENT_ENTRY_SIMULATION_TIME_COLOR : EVENT_LOG_ENTRY_SIMULATION_TIME_COLOR, false); 
				break;
			case 2:
				x += eventLogEntry.getLevel() * INDENT_SPACING;
				
				if (!(eventLogEntry instanceof EventEntry))
					x += INDENT_SPACING;

				if (eventLogEntry instanceof SendHopEntry || eventLogEntry instanceof SendDirectEntry)
					x += INDENT_SPACING;

				Image image = getEventLogEntryImage(eventLogEntry);
				gc.drawImage(image, x, 0);
				x += image.getBounds().width + HORIZONTAL_SPACING;
				
				switch (eventLogTable.getDisplayMode()) {
					case DESCRIPTIVE:
						if (eventLogEntry instanceof EventEntry) {
							IMessageDependency cause = contextEvent.getCause();
				
							drawText("Event in ", CONSTANT_TEXT_COLOR);
							drawModuleDescription(contextEvent.getModuleId(), EventLogTable.NameMode.FULL_PATH);
							
							BeginSendEntry beginSendEntry = cause != null ? cause.getBeginSendEntry() : null;
							if (beginSendEntry != null) {
								drawText(" on arrival of ", CONSTANT_TEXT_COLOR);
								
								if (contextEvent.isSelfMessageProcessingEvent())
									drawText("self ", CONSTANT_TEXT_COLOR);

								drawMessageDescription(beginSendEntry);

								if (!contextEvent.isSelfMessageProcessingEvent()) {
									drawText(" from ", CONSTANT_TEXT_COLOR);
									drawModuleDescription(beginSendEntry.getContextModuleId());
								}
								
								IEvent causeEvent = cause.getCauseEvent();
								if (causeEvent != null && causeEvent.getModuleId() != beginSendEntry.getContextModuleId()) {
									drawText(" called from ", CONSTANT_TEXT_COLOR);
									drawModuleDescription(causeEvent.getModuleId());
								}
							}
						}
						else {
							if (eventLogEntry instanceof EventLogMessageEntry) {
								EventLogMessageEntry eventLogMessageEntry = (EventLogMessageEntry)eventLogEntry;
								drawText(eventLogMessageEntry.getText(), EVENT_LOG_MESSAGE_COLOR);
							}
							else if (eventLogEntry instanceof BubbleEntry) {
								BubbleEntry bubbleEntry = (BubbleEntry)eventLogEntry;
								drawText("Bubble", CONSTANT_TEXT_COLOR);

								if (contextEvent.getModuleId() != bubbleEntry.getContextModuleId()) {
									drawText(" in ", CONSTANT_TEXT_COLOR);
									drawModuleDescription(bubbleEntry.getContextModuleId());
								}

								drawText(": ", CONSTANT_TEXT_COLOR);
								drawText(bubbleEntry.getText(), BUBBLE_ENTRY_COLOR, true);
							}
							else if (eventLogEntry instanceof ModuleMethodBeginEntry) {
								ModuleMethodBeginEntry moduleMethodBeginEntry = (ModuleMethodBeginEntry)eventLogEntry;
								drawText("Begin calling ", CONSTANT_TEXT_COLOR);
								drawText(moduleMethodBeginEntry.getMethod(), DATA_COLOR);
								drawText(" in ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(moduleMethodBeginEntry.getToModuleId());

								if (contextEvent.getModuleId() != moduleMethodBeginEntry.getContextModuleId()) {
									drawText(" from ", CONSTANT_TEXT_COLOR);
									drawModuleDescription(moduleMethodBeginEntry.getFromModuleId());
								}
							}
							else if (eventLogEntry instanceof ModuleMethodEndEntry) {
								drawText("End calling module", CONSTANT_TEXT_COLOR);
							}
							else if (eventLogEntry instanceof ModuleCreatedEntry) {
								ModuleCreatedEntry moduleCreatedEntry = (ModuleCreatedEntry)eventLogEntry;
								drawText("Creating ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(moduleCreatedEntry);
								drawText(" under ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(moduleCreatedEntry.getParentModuleId());
							}
							else if (eventLogEntry instanceof ModuleDeletedEntry) {
								ModuleDeletedEntry moduleDeletedEntry = (ModuleDeletedEntry)eventLogEntry;
								drawText("Deleting ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(moduleDeletedEntry.getModuleId());
							}
							else if (eventLogEntry instanceof ModuleReparentedEntry) {
								ModuleReparentedEntry moduleReparentedEntry = (ModuleReparentedEntry)eventLogEntry;
								drawText("Reparenting ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(moduleReparentedEntry.getModuleId());
								drawText(" under ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(moduleReparentedEntry.getNewParentModuleId());
							}
							else if (eventLogEntry instanceof GateCreatedEntry) {
							    GateCreatedEntry gateCreatedEntry = (GateCreatedEntry)eventLogEntry;
                                drawText("Creating ", CONSTANT_TEXT_COLOR);
							    drawGateDescription(gateCreatedEntry);
                                drawText(" in ", CONSTANT_TEXT_COLOR);
                                drawModuleDescription(gateCreatedEntry.getModuleId());
							}
                            else if (eventLogEntry instanceof GateDeletedEntry) {
                                GateDeletedEntry gateDeletedEntry = (GateDeletedEntry)eventLogEntry;
                                drawText("Deleting ", CONSTANT_TEXT_COLOR);
                                drawGateDescription(gateDeletedEntry.getModuleId(), gateDeletedEntry.getGateId());
                                drawText(" in ", CONSTANT_TEXT_COLOR);
                                drawModuleDescription(gateDeletedEntry.getModuleId());
                            }
							else if (eventLogEntry instanceof ConnectionCreatedEntry) {
								ConnectionCreatedEntry connectionCreatedEntry = (ConnectionCreatedEntry)eventLogEntry;
								drawText("Creating ", CONSTANT_TEXT_COLOR);
								drawConnectionDescription(connectionCreatedEntry.getSourceModuleId(), connectionCreatedEntry.getSourceGateId(),
									connectionCreatedEntry.getDestModuleId(), connectionCreatedEntry.getDestGateId());
							}
							else if (eventLogEntry instanceof ConnectionDeletedEntry) {
								ConnectionDeletedEntry connectionDeletedEntry = (ConnectionDeletedEntry)eventLogEntry;
								drawText("Deleting ", CONSTANT_TEXT_COLOR);
								drawConnectionDescription(connectionDeletedEntry.getSourceModuleId(), connectionDeletedEntry.getSourceGateId());
							}
							else if (eventLogEntry instanceof ConnectionDisplayStringChangedEntry) {
								ConnectionDisplayStringChangedEntry connectionDisplayStringChangedEntry = (ConnectionDisplayStringChangedEntry)eventLogEntry;
								drawText("Display string changed for ", CONSTANT_TEXT_COLOR);
                                drawConnectionDescription(connectionDisplayStringChangedEntry.getSourceModuleId(), connectionDisplayStringChangedEntry.getSourceGateId());
                                drawText(" to ", CONSTANT_TEXT_COLOR);
								drawText(connectionDisplayStringChangedEntry.getDisplayString(), DATA_COLOR);
							}
							else if (eventLogEntry instanceof ModuleDisplayStringChangedEntry) {
								ModuleDisplayStringChangedEntry moduleDisplayStringChangedEntry = (ModuleDisplayStringChangedEntry)eventLogEntry;
								drawText("Display string changed", CONSTANT_TEXT_COLOR);

								if (contextEvent.getModuleId() != moduleDisplayStringChangedEntry.getModuleId())	{
									drawText(" for ", CONSTANT_TEXT_COLOR);
									drawModuleDescription(moduleDisplayStringChangedEntry.getModuleId());
								}

								drawText(" to ", CONSTANT_TEXT_COLOR);							
								drawText(moduleDisplayStringChangedEntry.getDisplayString(), DATA_COLOR);
							}
							else if (eventLogEntry instanceof CancelEventEntry) {
								CancelEventEntry cancelEventEntry = (CancelEventEntry)eventLogEntry;
								drawText("Cancelling self ", CONSTANT_TEXT_COLOR);
								BeginSendEntry beginSendEntry = findBeginSendEntry(cancelEventEntry.getPreviousEventNumber(), cancelEventEntry.getMessageId());

								if (beginSendEntry == null)
								    beginSendEntry = findBeginSendEntry(contextEvent.getEventEntry().getCauseEventNumber(), cancelEventEntry.getMessageId());

								drawMessageDescription(beginSendEntry);
							}
							else if (eventLogEntry instanceof BeginSendEntry) {
								BeginSendEntry beginSendEntry = (BeginSendEntry)eventLogEntry;
								boolean isSelfMessage = contextEvent.isSelfMessage(beginSendEntry);

								if (isSelfMessage)
								    drawText("Scheduling ", CONSTANT_TEXT_COLOR);
								else
								    drawText("Sending ", CONSTANT_TEXT_COLOR);

								drawMessageDescription(beginSendEntry);

                                if (isSelfMessage)
                                    drawText(" for ", CONSTANT_TEXT_COLOR);
                                else
                                    drawText(" arriving at ", CONSTANT_TEXT_COLOR);
                                
                                drawMessageArrivalTime(contextEvent.getEndSendEntry(beginSendEntry));
								drawText(" kind = ", CONSTANT_TEXT_COLOR);
								drawText(String.valueOf(beginSendEntry.getMessageKind()), DATA_COLOR);
								drawText(" length = ", CONSTANT_TEXT_COLOR);
								drawText(String.valueOf(beginSendEntry.getMessageLength()), DATA_COLOR);
							}
							else if (eventLogEntry instanceof EndSendEntry) {
								EndSendEntry endSendEntry = (EndSendEntry)eventLogEntry;
								drawText("Arrival at ", CONSTANT_TEXT_COLOR);
                                drawMessageArrivalTime(endSendEntry);
							}
							else if (eventLogEntry instanceof SendHopEntry) {
								SendHopEntry sendHopEntry = (SendHopEntry)eventLogEntry;
								drawText("Sending through ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(sendHopEntry.getSenderModuleId());
								drawText(" ", CONSTANT_TEXT_COLOR);
								drawGateDescription(sendHopEntry.getSenderModuleId(), sendHopEntry.getSenderGateId());
								
								if (sendHopEntry.getTransmissionDelay().doubleValue() != 0) {
    								drawText(" with transmission delay ", CONSTANT_TEXT_COLOR);
    								drawText(sendHopEntry.getTransmissionDelay() + "s", DATA_COLOR);
								}

								if (sendHopEntry.getPropagationDelay().doubleValue() != 0) {
    								drawText(" and propagation delay ", CONSTANT_TEXT_COLOR);
    								drawText(sendHopEntry.getPropagationDelay() + "s", DATA_COLOR);
								}
							}
							else if (eventLogEntry instanceof SendDirectEntry) {
								SendDirectEntry sendDirectEntry = (SendDirectEntry)eventLogEntry;
								drawText("Sending direct message ", CONSTANT_TEXT_COLOR);

								if (contextEvent.getModuleId() != sendDirectEntry.getContextModuleId())	{
									drawText("from ", CONSTANT_TEXT_COLOR);
									drawModuleDescription(sendDirectEntry.getSenderModuleId());
								}

								drawText(" to ", CONSTANT_TEXT_COLOR);
								drawModuleDescription(sendDirectEntry.getDestModuleId());
                                drawText(" ", CONSTANT_TEXT_COLOR);
								drawGateDescription(sendDirectEntry.getDestModuleId(), sendDirectEntry.getDestGateId());
								drawText(" with transmission delay ", CONSTANT_TEXT_COLOR);
								drawText(sendDirectEntry.getTransmissionDelay() + "s", DATA_COLOR);
								drawText(" and propagation delay ", CONSTANT_TEXT_COLOR);
								drawText(sendDirectEntry.getPropagationDelay() + "s", DATA_COLOR);
							}
							else if (eventLogEntry instanceof DeleteMessageEntry) {
								DeleteMessageEntry deleteMessageEntry = (DeleteMessageEntry)eventLogEntry;
								drawText("Deleting ", CONSTANT_TEXT_COLOR);
                                BeginSendEntry beginSendEntry = findBeginSendEntry(deleteMessageEntry.getPreviousEventNumber(), deleteMessageEntry.getMessageId());

                                if (beginSendEntry == null)
                                    beginSendEntry = findBeginSendEntry(contextEvent.getEventEntry().getCauseEventNumber(), deleteMessageEntry.getMessageId());

								drawMessageDescription(beginSendEntry);
							}
                            else if (eventLogEntry instanceof SimulationBeginEntry) {
                                SimulationBeginEntry simulationBeginEntry = (SimulationBeginEntry)eventLogEntry;
                                drawText("Simulation started with runId ", CONSTANT_TEXT_COLOR);
                                drawText(simulationBeginEntry.getRunId(), DATA_COLOR);
                            }
                            else if (eventLogEntry instanceof SimulationEndEntry) {
                                drawText("Simulation finished", CONSTANT_TEXT_COLOR);
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

	public String getTooltipText(EventLogEntryReference eventLogEntryReference, SizeConstraint outSizeConstraint) {
		EventLogEntry eventLogEntry = eventLogEntryReference.getEventLogEntry(eventLogInput);
		
		if (eventLogEntry instanceof BeginSendEntry) {
			BeginSendEntry beginSendEntry = (BeginSendEntry)eventLogEntry;
			String detail = beginSendEntry.getDetail();

			if (detail != null) {
				int longestLineLength = 0;
				for (String line : detail.split("\n"))
					longestLineLength = Math.max(longestLineLength, line.length());
				// TODO: correct solution would be to get pre font width (monospace, 8) and consider margins too
				outSizeConstraint.minimumWidth = longestLineLength * 8;
				return "<pre>" + detail + "</pre>";
			}
		}

		return null;
	}

	private BeginSendEntry findBeginSendEntry(int previousEventNumber, int messageId) {
		if (previousEventNumber != -1) {
			IEvent event = eventLogInput.getEventLog().getEventForEventNumber(previousEventNumber);
	
			if (event != null) {
				int index = event.findBeginSendEntryIndex(messageId);
				
				if (index != -1)
					return (BeginSendEntry)event.getEventLogEntry(index);
			}
		}
		
		return null;
	}

	private void drawModuleDescription(int moduleId) {
		drawModuleDescription(eventLogInput.getEventLog().getModuleCreatedEntry(moduleId));
	}

    private void drawModuleDescription(int moduleId, NameMode nameMode) {
        drawModuleDescription(eventLogInput.getEventLog().getModuleCreatedEntry(moduleId), nameMode);
    }

    private void drawModuleDescription(ModuleCreatedEntry moduleCreatedEntry) {
        drawModuleDescription(moduleCreatedEntry, eventLogTable.getNameMode());
    }

    private void drawModuleDescription(ModuleCreatedEntry moduleCreatedEntry, NameMode nameMode) {
		drawText("module ", CONSTANT_TEXT_COLOR);

		if (moduleCreatedEntry != null) {
		    String typeName = null;

		    switch (eventLogTable.getTypeMode()) {
		        case NED:
		            typeName = moduleCreatedEntry.getNedTypeName();
		            break;
                case CPP:
                    typeName = moduleCreatedEntry.getModuleClassName();
                    break;
		    }
			
		    drawText("(" + typeName + ") ", TYPE_COLOR);
			
			switch (nameMode) {
                case SMART_NAME:
                    if (contextEvent.getModuleId() == moduleCreatedEntry.getModuleId())
                        drawModuleFullName(moduleCreatedEntry);
                    else
                        drawModuleFullPath(moduleCreatedEntry);
                    break;
			    case FULL_NAME:
			        drawModuleFullName(moduleCreatedEntry);
			        break;
			    case FULL_PATH:
			        drawModuleFullPath(moduleCreatedEntry);
			        break;
			}
		}
		else
			drawText("<unknown>", NAME_COLOR, true);
	}

    private void drawModuleFullPath(ModuleCreatedEntry moduleCreatedEntry) {
        drawText(eventLogInput.getEventLogTableFacade().EventLogEntry_getModuleFullPath(moduleCreatedEntry.getCPtr()), NAME_COLOR, true);
    }

    private void drawModuleFullName(ModuleCreatedEntry moduleCreatedEntry) {
        drawText(moduleCreatedEntry.getFullName(), NAME_COLOR, true);
    }

	private void drawGateDescription(int moduleId, int gateId) {
        drawGateDescription(eventLogInput.getEventLog().getGateCreatedEntry(moduleId, gateId));
	}
	
	private void drawGateDescription(GateCreatedEntry gateCreatedEntry) {
        drawText("gate ", CONSTANT_TEXT_COLOR);

        if (gateCreatedEntry != null) {
            int index = gateCreatedEntry.getIndex();
            drawText(gateCreatedEntry.getName() + (index != -1 ? "[" + index + "]" : ""), NAME_COLOR, true);
        }
        else
            drawText("<unknown>", NAME_COLOR, true);
	}

    private void drawConnectionDescription(int sourceModuleId, int sourceGateId) {
        drawConnectionDescription(sourceModuleId, sourceGateId, -1, -1);
    }
	
	private void drawConnectionDescription(int sourceModuleId, int sourceGateId, int destModuleId, int destGateId) {
        drawText("connection from ", CONSTANT_TEXT_COLOR);
        
        ModuleCreatedEntry sourceModuleCreatedEntry = eventLogInput.getEventLog().getModuleCreatedEntry(sourceModuleId);

        if (sourceModuleCreatedEntry != null) {
            drawModuleDescription(sourceModuleCreatedEntry);
            drawText(" ", CONSTANT_TEXT_COLOR);
            drawGateDescription(sourceModuleId, sourceGateId);
        }

        ModuleCreatedEntry destModuleCreatedEntry = eventLogInput.getEventLog().getModuleCreatedEntry(destModuleId);

        if (destModuleCreatedEntry != null) {
            drawText(" to ", CONSTANT_TEXT_COLOR);
            drawModuleDescription(destModuleCreatedEntry);
            drawText(" ", CONSTANT_TEXT_COLOR);
            drawGateDescription(destModuleId, destGateId);
        }
	}

	private void drawMessageDescription(BeginSendEntry beginSendEntry) {
        drawText("message ", CONSTANT_TEXT_COLOR);

        if (beginSendEntry != null) {
			drawText("(" + beginSendEntry.getMessageClassName() + ") ", TYPE_COLOR);
			drawText(beginSendEntry.getMessageFullName(), NAME_COLOR, true);
		}
		else
			drawText("<unknown>", NAME_COLOR, true);
	}
	
	private void drawMessageArrivalTime(EndSendEntry endSendEntry) {
        BigDecimal arrivalTime = endSendEntry.getArrivalTime();
        BigDecimal simulationTime = endSendEntry.getEvent().getSimulationTime();
        drawText(arrivalTime + "s", DATA_COLOR);
        drawText(", now + ", CONSTANT_TEXT_COLOR);
        drawText(TimeUtils.secondsToTimeString(arrivalTime.toBigDecimal().subtract(simulationTime.toBigDecimal())), DATA_COLOR);
	}

	private void drawRawEntry(EventLogEntry eventLogEntry) {

		if (!(eventLogEntry instanceof EventLogMessageEntry)) {
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
	
	private void drawText(String text, Color color) {
		drawText(text, color, false);
	}
	
	private void drawText(String text, Color color, boolean bold) {
	    text = text.replaceAll("\n", "\\\\n");
		Font oldFont = gc.getFont();
		FontData fontData = font.getFontData()[0];
		Font newFont = new Font(oldFont.getDevice(), fontData.getName(), fontData.getHeight(), bold ? SWT.BOLD : SWT.NORMAL);
		gc.setFont(newFont);

		if (color != null && !gc.getForeground().equals(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT)))
			gc.setForeground(color);

		gc.drawText(text, x, VERTICAL_SPACING/2);
		x += gc.textExtent(text).x + 1; // +1 due to avoid cropping antialias
		newFont.dispose();
		gc.setFont(oldFont);
	}
	
	private Image getEventLogEntryImage(EventLogEntry eventLogEntry) {
		String className = eventLogEntry.getClassName();
		Event event = eventLogEntry.getEvent();

		if (eventLogEntry instanceof EventEntry && event.isSelfMessageProcessingEvent())
			return ImageFactory.getImage(ImageFactory.EVENLOG_IMAGE_SELF_EVENT);
		else if (eventLogEntry instanceof BeginSendEntry) {
			int index = eventLogEntry.getEntryIndex();
			int count = event.getNumEventLogEntries();
			
			if (count > index + 1) {
                EventLogEntry nextEventLogEntry = event.getEventLogEntry(index + 1);
    
    			if (nextEventLogEntry instanceof EndSendEntry)
    				return ImageFactory.getImage(ImageFactory.EVENLOG_IMAGE_SCHEDULE_AT);
			}
		}

		return ImageFactory.getImage(ImageFactory.EVENTLOG_IMAGE_DIR + className.substring(0, className.length() - 5));
	}
}
