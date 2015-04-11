/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.eventlogtable.widgets;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.jface.viewers.StyledString.Styler;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyleRange;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.graphics.TextStyle;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.TimeUtils;
import org.omnetpp.common.virtualtable.IVirtualTableRowRenderer;
import org.omnetpp.eventlog.engine.BeginSendEntry;
import org.omnetpp.eventlog.engine.BubbleEntry;
import org.omnetpp.eventlog.engine.CancelEventEntry;
import org.omnetpp.eventlog.engine.CloneMessageEntry;
import org.omnetpp.eventlog.engine.ConnectionCreatedEntry;
import org.omnetpp.eventlog.engine.ConnectionDeletedEntry;
import org.omnetpp.eventlog.engine.ConnectionDisplayStringChangedEntry;
import org.omnetpp.eventlog.engine.CreateMessageEntry;
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
import org.omnetpp.eventlog.engine.KeyframeEntry;
import org.omnetpp.eventlog.engine.MessageEntry;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.ModuleDeletedEntry;
import org.omnetpp.eventlog.engine.ModuleDisplayStringChangedEntry;
import org.omnetpp.eventlog.engine.ModuleMethodBeginEntry;
import org.omnetpp.eventlog.engine.ModuleMethodEndEntry;
import org.omnetpp.eventlog.engine.PStringVector;
import org.omnetpp.eventlog.engine.SendDirectEntry;
import org.omnetpp.eventlog.engine.SendHopEntry;
import org.omnetpp.eventlog.engine.SimulationBeginEntry;
import org.omnetpp.eventlog.engine.SimulationEndEntry;
import org.omnetpp.eventlogtable.widgets.EventLogTable.NameMode;

public class EventLogTableRowRenderer implements IVirtualTableRowRenderer<EventLogEntryReference> {
    private static final Font FONT;
    private static final Font BOLD_FONT;
    private static final Color DARKBLUE = new Color(null, 0, 0, 192);
    private static final Color DARKRED = new Color(null, 127, 0, 85);
    private static final Color RED = new Color(null, 240, 0, 0);
    private static final Color BLACK = new Color(null, 0, 0, 0);
    private static final Color LIGHTGREY = new Color(null, 211, 211, 211);
    private static final Color BOOKMARK_COLOR = ColorFactory.LIGHT_CYAN;
    private static final Styler EVENT_ENTRY_EVENT_NUMBER_STYLE = createStyle(BLACK);
    private static final Styler EVENT_LOG_ENTRY_EVENT_NUMBER_STYLE = createStyle(LIGHTGREY);
    private static final Styler EVENT_ENTRY_SIMULATION_TIME_STYLE = createStyle(BLACK);
    private static final Styler EVENT_LOG_ENTRY_SIMULATION_TIME_STYLE = createStyle(LIGHTGREY);
    private static final Styler CONSTANT_TEXT_STYLE = createStyle(BLACK);
    private static final Styler BOLD_CONSTANT_TEXT_STYLE = createStyle(BLACK, true);
    private static final Styler RAW_VALUE_STYLE = createStyle(DARKBLUE);
    private static final Styler TYPE_STYLE = createStyle(DARKBLUE);
    private static final Styler NAME_STYLE = createStyle(DARKBLUE, true);
    private static final Styler EVENT_LOG_MESSAGE_STYLE = createStyle(DARKRED);
    private static final Styler BUBBLE_ENTRY_STYLE = createStyle(RED, true);
    private static final Styler DATA_STYLE = createStyle(DARKBLUE);
    private static final int HORIZONTAL_SPACING = 4;
    private static final int INDENT_SPACING = HORIZONTAL_SPACING * 4;
    private static final int VERTICAL_SPACING = 3;

    protected EventLogInput eventLogInput;
    protected EventLogTable eventLogTable;
    protected int fontHeight;
    private IEvent contextEvent;

    static {
        FontData systemFont = JFaceResources.getDefaultFont().getFontData()[0];
        FONT = new Font(null, systemFont.getName(), systemFont.getHeight(), SWT.NORMAL);
        BOLD_FONT = new Font(null, systemFont.getName(), systemFont.getHeight(), SWT.BOLD);
    }

    public EventLogTableRowRenderer(EventLogTable eventLogTable) {
        this.eventLogTable = eventLogTable;
    }

    public void setInput(Object eventLogInput) {
        this.eventLogInput = (EventLogInput)eventLogInput;
    }

    public int getRowHeight(GC gc) {
        if (fontHeight == 0) {
            Font oldFont = gc.getFont();
            gc.setFont(FONT);
            fontHeight = gc.getFontMetrics().getHeight();
            gc.setFont(oldFont);
        }

        return fontHeight + VERTICAL_SPACING;
    }

    @Override
    public void drawCell(GC gc, EventLogEntryReference eventLogEntryReference, int index, boolean isSelected) {
        Assert.isTrue(eventLogEntryReference != null);

        gc.setAntialias(SWT.OFF);

        EventLogEntry eventLogEntry = eventLogEntryReference.getEventLogEntry(eventLogInput);
        contextEvent = eventLogEntry.getEvent();
        long eventNumber = contextEvent.getEventNumber();

        try {
            if (eventLogInput.getFile() != null) {
                IMarker[] markers = eventLogInput.getFile().findMarkers(IMarker.BOOKMARK, true, IResource.DEPTH_ZERO);
                boolean marked = false;
                for (int i = 0; i < markers.length; i++)
                    if (markers[i].getAttribute("EventNumber", "-1").equals(String.valueOf(eventNumber))) {
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

        StyledString styledString = getStyledText(eventLogEntryReference, index, isSelected);
        int x = getIndentation(eventLogEntryReference, index);

        Image image = getImage(eventLogEntryReference, index);
        if (image != null) {
            gc.drawImage(image, x, (getRowHeight(gc) - image.getBounds().height) / 2);
            x += image.getBounds().width + HORIZONTAL_SPACING;
        }

        drawStyledString(gc, styledString, x, VERTICAL_SPACING / 2);
    }

    protected void drawStyledString(GC gc, StyledString styledString, int x, int y) {
        TextLayout textLayout = new TextLayout(gc.getDevice());
        textLayout.setText(styledString.getString());
        for (StyleRange styleRange : styledString.getStyleRanges()) {
            textLayout.setStyle(styleRange, styleRange.start, styleRange.start + styleRange.length);
        }
        textLayout.draw(gc, x, y);
        textLayout.dispose();
    }


    @Override
    public String getTooltipText(EventLogEntryReference eventLogEntryReference) {
        EventLogEntry eventLogEntry = eventLogEntryReference.getEventLogEntry(eventLogInput);

        if (eventLogEntry instanceof BeginSendEntry) {
            BeginSendEntry beginSendEntry = (BeginSendEntry)eventLogEntry;
            String detail = beginSendEntry.getDetail();

            if (detail != null) {
                int longestLineLength = 0;
                for (String line : detail.split("\n"))
                    longestLineLength = Math.max(longestLineLength, line.length());
                return "<pre>" + detail + "</pre>";
            }
            else
                return "No message detail recorded. You can record message detail by setting the eventlog-message-detail-pattern configuration option in the ini file.";
        }

        return null;
    }

    private BeginSendEntry findBeginSendEntry(long previousEventNumber, int messageId) {
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

    @Override
    public int getIndentation(EventLogEntryReference eventLogEntryReference, int index) {
        int indent = HORIZONTAL_SPACING;
        if (index == 2) {
            EventLogEntry eventLogEntry = eventLogEntryReference.getEventLogEntry(eventLogInput);

            indent += eventLogEntry.getLevel() * INDENT_SPACING;

            if (!(eventLogEntry instanceof EventEntry))
                indent += INDENT_SPACING;

            if (eventLogEntry instanceof SendHopEntry || eventLogEntry instanceof SendDirectEntry)
                indent += INDENT_SPACING;
        }
        return indent;
    }

    @Override
    public Image getImage(EventLogEntryReference eventLogEntryReference, int columnIndex) {
        if (columnIndex == 2) {
            EventLogEntry eventLogEntry = eventLogEntryReference.getEventLogEntry(eventLogInput);

            String className = eventLogEntry.getClassName();
            Event event = eventLogEntry.getEvent();

            if (eventLogEntry instanceof EventEntry && event.isSelfMessageProcessingEvent())
                return ImageFactory.global().getImage(ImageFactory.EVENLOG_IMAGE_SELF_EVENT);
            else if (eventLogEntry instanceof BeginSendEntry) {
                int index = eventLogEntry.getEntryIndex();
                int count = event.getNumEventLogEntries();

                if (count > index + 1) {
                    EventLogEntry nextEventLogEntry = event.getEventLogEntry(index + 1);

                    if (nextEventLogEntry instanceof EndSendEntry)
                        return ImageFactory.global().getImage(ImageFactory.EVENLOG_IMAGE_SCHEDULE_AT);
                }
            }

            return ImageFactory.global().getImage(ImageFactory.EVENTLOG_IMAGE_DIR + className.substring(0, className.length() - 5));

        }
        return null;
    }

    @Override
    public StyledString getStyledText(EventLogEntryReference eventLogEntryReference, int index, boolean isSelected) {
        EventLogEntry eventLogEntry = eventLogEntryReference.getEventLogEntry(eventLogInput);
        contextEvent = eventLogEntry.getEvent();
        long eventNumber = contextEvent.getEventNumber();
        BigDecimal simulationTime = contextEvent.getSimulationTime();
        boolean isEventLogEntry = eventLogEntry instanceof EventEntry;

        switch (index) {
        case 0:
            return new Builder(isSelected)
                     .append("#" + eventNumber, isEventLogEntry ? EVENT_ENTRY_EVENT_NUMBER_STYLE : EVENT_LOG_ENTRY_EVENT_NUMBER_STYLE)
                     .getStyledString();
        case 1:
            return new Builder(isSelected)
                     .append(simulationTime + "s", isEventLogEntry ? EVENT_ENTRY_SIMULATION_TIME_STYLE : EVENT_LOG_ENTRY_SIMULATION_TIME_STYLE)
                     .getStyledString();
        case 2:
            switch (eventLogTable.getDisplayMode()) {
                case DESCRIPTIVE:
                    return getDescriptiveStyledText(eventLogEntry, isSelected);
                case RAW:
                    return getRawStyledText(eventLogEntry, isSelected);
                default:
                    throw new RuntimeException("Unknown display mode");
            }
        default:
            Assert.isTrue(false);
            return null;
        }
    }

    private StyledString getRawStyledText(EventLogEntry eventLogEntry, boolean isSelected) {
        Builder builder = new Builder(isSelected);
        if (!(eventLogEntry instanceof EventLogMessageEntry)) {
            builder.append(eventLogEntry.getAsString() + " ", BOLD_CONSTANT_TEXT_STYLE);
        }

        PStringVector stringVector = eventLogEntry.getAttributeNames();
        for (int i = 0; i < stringVector.size(); i++)
        {
            String name = stringVector.get(i);
            builder.constant(name + " ");
            String value = eventLogEntry.getAsString(name);
            if (StringUtils.isEmpty(value))
                builder.raw("\"\" ");
            else
                builder.raw(value + " ");
        }
        return builder.getStyledString();
    }

    private StyledString getDescriptiveStyledText(EventLogEntry eventLogEntry, boolean isSelected) {
        Builder builder = new Builder(isSelected);

        long eventNumber = contextEvent.getEventNumber();

        if (eventLogEntry instanceof EventEntry) {
            if (eventNumber == 0) {
                builder.constant("Setting up ").module(1);
            }
            else {
                IMessageDependency cause = contextEvent.getCause();

                builder.constant("Event in ")
                       .module(contextEvent.getModuleId(), EventLogTable.NameMode.FULL_PATH);

                MessageEntry beginSendEntry = cause != null ? cause.getMessageEntry() : null;
                if (beginSendEntry != null) {
                    builder.constant(" on arrival of ");

                    if (contextEvent.isSelfMessageProcessingEvent())
                        builder.constant("self ");

                    builder.message(beginSendEntry);

                    if (!contextEvent.isSelfMessageProcessingEvent())
                        builder.constant(" from ").module(beginSendEntry.getContextModuleId());

                    IEvent causeEvent = cause.getCauseEvent();
                    if (causeEvent != null && causeEvent.getModuleId() != beginSendEntry.getContextModuleId())
                        builder.constant(" called from ").module(causeEvent.getModuleId());
                }
            }
        }
        else {
            if (eventLogEntry instanceof EventLogMessageEntry) {
                EventLogMessageEntry eventLogMessageEntry = (EventLogMessageEntry)eventLogEntry;
                builder.eventLogMessage(eventLogMessageEntry.getText());
            }
            else if (eventLogEntry instanceof BubbleEntry) {
                BubbleEntry bubbleEntry = (BubbleEntry)eventLogEntry;
                builder.constant("Bubble");

                if (contextEvent.getModuleId() != bubbleEntry.getContextModuleId())
                    builder.constant(" in ").module(bubbleEntry.getContextModuleId());

                builder.constant(": ").bubble(bubbleEntry.getText());
            }
            else if (eventLogEntry instanceof ModuleMethodBeginEntry) {
                ModuleMethodBeginEntry moduleMethodBeginEntry = (ModuleMethodBeginEntry)eventLogEntry;
                builder.constant("Begin calling ");
                String method = moduleMethodBeginEntry.getMethod();

                if (StringUtils.isEmpty(method))
                    builder.constant("method");
                else
                    builder.data(method);

                builder.constant(" in ").module(moduleMethodBeginEntry.getToModuleId());

                if (contextEvent.getModuleId() != moduleMethodBeginEntry.getContextModuleId())
                    builder.constant(" from ").module(moduleMethodBeginEntry.getFromModuleId());
            }
            else if (eventLogEntry instanceof ModuleMethodEndEntry) {
                builder.constant("End calling module");
            }
            else if (eventLogEntry instanceof ModuleCreatedEntry) {
                ModuleCreatedEntry moduleCreatedEntry = (ModuleCreatedEntry)eventLogEntry;
                builder.constant("Creating ").module(moduleCreatedEntry);
                int parentModuleId = moduleCreatedEntry.getParentModuleId();
                if (parentModuleId != -1)
                    builder.constant(" under ").module(parentModuleId);
            }
            else if (eventLogEntry instanceof ModuleDeletedEntry) {
                ModuleDeletedEntry moduleDeletedEntry = (ModuleDeletedEntry)eventLogEntry;
                builder.constant("Deleting ").module(moduleDeletedEntry.getModuleId());
            }
            else if (eventLogEntry instanceof GateCreatedEntry) {
                GateCreatedEntry gateCreatedEntry = (GateCreatedEntry)eventLogEntry;
                builder.constant("Creating ").gate(gateCreatedEntry)
                       .constant(" in ").module(gateCreatedEntry.getModuleId());
            }
            else if (eventLogEntry instanceof GateDeletedEntry) {
                GateDeletedEntry gateDeletedEntry = (GateDeletedEntry)eventLogEntry;
                builder.constant("Deleting ").gate(gateDeletedEntry.getModuleId(), gateDeletedEntry.getGateId())
                       .constant(" in ").module(gateDeletedEntry.getModuleId());
            }
            else if (eventLogEntry instanceof ConnectionCreatedEntry) {
                ConnectionCreatedEntry connectionCreatedEntry = (ConnectionCreatedEntry)eventLogEntry;
                builder.constant("Creating ")
                       .connection(connectionCreatedEntry.getSourceModuleId(), connectionCreatedEntry.getSourceGateId(),
                               connectionCreatedEntry.getDestModuleId(), connectionCreatedEntry.getDestGateId());
            }
            else if (eventLogEntry instanceof ConnectionDeletedEntry) {
                ConnectionDeletedEntry connectionDeletedEntry = (ConnectionDeletedEntry)eventLogEntry;
                builder.constant("Deleting ")
                       .connection(connectionDeletedEntry.getSourceModuleId(), connectionDeletedEntry.getSourceGateId());
            }
            else if (eventLogEntry instanceof ConnectionDisplayStringChangedEntry) {
                ConnectionDisplayStringChangedEntry connectionDisplayStringChangedEntry = (ConnectionDisplayStringChangedEntry)eventLogEntry;
                builder.constant("Display string changed for ")
                       .connection(connectionDisplayStringChangedEntry.getSourceModuleId(), connectionDisplayStringChangedEntry.getSourceGateId())
                       .constant(" to ")
                       .data(connectionDisplayStringChangedEntry.getDisplayString());
            }
            else if (eventLogEntry instanceof ModuleDisplayStringChangedEntry) {
                ModuleDisplayStringChangedEntry moduleDisplayStringChangedEntry = (ModuleDisplayStringChangedEntry)eventLogEntry;
                builder.constant("Display string changed");

                if (contextEvent.getModuleId() != moduleDisplayStringChangedEntry.getModuleId())
                    builder.constant(" for ").module(moduleDisplayStringChangedEntry.getModuleId());

                builder.constant(" to ").data(moduleDisplayStringChangedEntry.getDisplayString());
            }
            else if (eventLogEntry instanceof CancelEventEntry) {
                CancelEventEntry cancelEventEntry = (CancelEventEntry)eventLogEntry;
                builder.constant("Cancelling self ");
                BeginSendEntry beginSendEntry = findBeginSendEntry(cancelEventEntry.getPreviousEventNumber(), cancelEventEntry.getMessageId());

                if (beginSendEntry == null)
                    beginSendEntry = findBeginSendEntry(contextEvent.getEventEntry().getCauseEventNumber(), cancelEventEntry.getMessageId());

                builder.message(beginSendEntry);
            }
            else if (eventLogEntry instanceof BeginSendEntry) {
                BeginSendEntry beginSendEntry = (BeginSendEntry)eventLogEntry;
                boolean isSelfMessage = contextEvent.isSelfMessage(beginSendEntry);

                if (isSelfMessage)
                    builder.constant("Scheduling self ");
                else
                    builder.constant("Sending ");

                builder.message(beginSendEntry);

                if (isSelfMessage)
                    builder.constant(" for ");
                else
                    builder.constant(" arriving at ");

                EndSendEntry endSendEntry = contextEvent.getEndSendEntry(beginSendEntry);

                if (endSendEntry == null)
                    builder.data("<unknown>");
                else
                    builder.constant(" kind = ");
                builder.data(String.valueOf(beginSendEntry.getMessageKind()));
                if (!beginSendEntry.getMessageClassName().equals("cMessage"))
                    builder.constant(" length = ").data(String.valueOf(beginSendEntry.getMessageLength()));
            }
            else if (eventLogEntry instanceof EndSendEntry) {
                EndSendEntry endSendEntry = (EndSendEntry)eventLogEntry;
                builder.constant("Arrival at ").messageArrivalTime(endSendEntry);
            }
            else if (eventLogEntry instanceof SendHopEntry) {
                SendHopEntry sendHopEntry = (SendHopEntry)eventLogEntry;
                builder.constant("Sending through ").module(sendHopEntry.getSenderModuleId())
                       .constant(" ").gate(sendHopEntry.getSenderModuleId(), sendHopEntry.getSenderGateId());

                if (sendHopEntry.getTransmissionDelay().doubleValue() != 0) {
                    builder.constant(" transmission delay = ").data(sendHopEntry.getTransmissionDelay() + "s");
                }

                if (sendHopEntry.getPropagationDelay().doubleValue() != 0) {
                    builder.constant(" propagation delay = ").data(sendHopEntry.getPropagationDelay() + "s");
                }
            }
            else if (eventLogEntry instanceof SendDirectEntry) {
                SendDirectEntry sendDirectEntry = (SendDirectEntry)eventLogEntry;
                builder.constant("Sending direct message ");

                if (contextEvent.getModuleId() != sendDirectEntry.getContextModuleId()) {
                    builder.constant("from ").module(sendDirectEntry.getSenderModuleId());
                }

                builder.constant(" to ").module(sendDirectEntry.getDestModuleId())
                       .constant(" ").gate(sendDirectEntry.getDestModuleId(), sendDirectEntry.getDestGateId())
                       .constant(" with transmission delay ").data(sendDirectEntry.getTransmissionDelay() + "s")
                       .constant(" and propagation delay ").data(sendDirectEntry.getPropagationDelay() + "s");
            }
            else if (eventLogEntry instanceof CreateMessageEntry) {
                CreateMessageEntry createMessageEntry = (CreateMessageEntry)eventLogEntry;
                builder.constant("Creating ").message(createMessageEntry);
            }
            else if (eventLogEntry instanceof CloneMessageEntry) {
                CloneMessageEntry cloneMessageEntry = (CloneMessageEntry)eventLogEntry;
                builder.constant("Cloning ").message(cloneMessageEntry);
            }
            else if (eventLogEntry instanceof DeleteMessageEntry) {
                DeleteMessageEntry deleteMessageEntry = (DeleteMessageEntry)eventLogEntry;
                builder.constant("Deleting ").message(deleteMessageEntry);
            }
            else if (eventLogEntry instanceof SimulationBeginEntry) {
                SimulationBeginEntry simulationBeginEntry = (SimulationBeginEntry)eventLogEntry;
                builder.constant("Simulation started with runId ").data(simulationBeginEntry.getRunId());
            }
            else if (eventLogEntry instanceof SimulationEndEntry) {
                SimulationEndEntry simulationEndEntry = (SimulationEndEntry)eventLogEntry;
                builder.constant(simulationEndEntry.getIsError() ? "Simulation aborted with error code " : "Simulation finished with result code ")
                       .data(String.valueOf(simulationEndEntry.getResultCode()))
                       .constant(" : ").data(simulationEndEntry.getMessage());
            }
            else if (eventLogEntry instanceof KeyframeEntry) {
                KeyframeEntry keyframeEntry = (KeyframeEntry)eventLogEntry;
                builder.constant("Keyframe with ");
                String simulationStateEntries = keyframeEntry.getSimulationStateEntries();
                if (!StringUtils.isEmpty(simulationStateEntries))
                    builder.data(String.valueOf(simulationStateEntries.split(",").length));
                else
                    builder.data("0");
                builder.constant(" simulation state events");
            }
            else
                throw new RuntimeException("Unknown eventlog entry: " + eventLogEntry.getClassName());
        }

        return builder.getStyledString();
    }

    private class Builder {
        StyledString styledString = new StyledString();
        boolean isSelected;

        Builder(boolean isSelected) {
            this.isSelected = isSelected;
        }

        StyledString getStyledString() {
            return styledString;
        }

        private Styler selectedStyle(final Styler style) {
            return new Styler() {
                @Override
                public void applyStyles(TextStyle textStyle) {
                    style.applyStyles(textStyle);
                    textStyle.foreground = Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT);
                }
            };
        }

        Builder append(String string, Styler style) {
            string = string.replaceAll("\n", "\\\\n");
            styledString.append(string, isSelected ? selectedStyle(style) : style);
            return this;
        }

        Builder constant(String string) {
            return append(string, CONSTANT_TEXT_STYLE);
        }

        Builder data(String value) {
            return append(value, DATA_STYLE);
        }

        Builder raw(String value) {
            return append(value, RAW_VALUE_STYLE);
        }

        Builder bubble(String text) {
            return append(text, BUBBLE_ENTRY_STYLE);
        }

        Builder eventLogMessage(String text) {
            return append(text, EVENT_LOG_MESSAGE_STYLE);
        }

        Builder module(int moduleId) {
            return module(eventLogInput.getEventLog().getModuleCreatedEntry(moduleId));
        }

        Builder module(int moduleId, NameMode nameMode) {
            return module(eventLogInput.getEventLog().getModuleCreatedEntry(moduleId), nameMode);
        }

        Builder module(ModuleCreatedEntry moduleCreatedEntry) {
            return module(moduleCreatedEntry, eventLogTable.getNameMode());
        }

        Builder module(ModuleCreatedEntry moduleCreatedEntry, NameMode nameMode) {
            append("module ", CONSTANT_TEXT_STYLE);

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

                append("(" + typeName + ") ", TYPE_STYLE);

                switch (nameMode) {
                case SMART_NAME:
                    if (contextEvent.getModuleId() == moduleCreatedEntry.getModuleId())
                        moduleFullName(moduleCreatedEntry);
                    else
                        moduleFullPath(moduleCreatedEntry);
                    break;
                case FULL_NAME:
                    moduleFullName(moduleCreatedEntry);
                    break;
                case FULL_PATH:
                    moduleFullPath(moduleCreatedEntry);
                    break;
                }
            }
            else
                append("<unknown>", NAME_STYLE);

            return this;
        }

        Builder moduleFullPath(ModuleCreatedEntry moduleCreatedEntry) {
            return append(eventLogInput.getEventLogTableFacade().ModuleCreatedEntry_getModuleFullPath(moduleCreatedEntry.getCPtr()), NAME_STYLE);
        }

        Builder moduleFullName(ModuleCreatedEntry moduleCreatedEntry) {
            return append(moduleCreatedEntry.getFullName(), NAME_STYLE);
        }

        Builder gate(int moduleId, int gateId) {
            return gate(eventLogInput.getEventLog().getGateCreatedEntry(moduleId, gateId));
        }

        Builder gate(GateCreatedEntry gateCreatedEntry) {
            append("gate ", CONSTANT_TEXT_STYLE);

            if (gateCreatedEntry != null) {
                int index = gateCreatedEntry.getIndex();
                append(gateCreatedEntry.getName() + (index != -1 ? "[" + index + "]" : ""), NAME_STYLE);
            }
            else
                append("<unknown>", NAME_STYLE);
            return this;
        }

        Builder connection(int sourceModuleId, int sourceGateId) {
            return connection(sourceModuleId, sourceGateId, -1, -1);
        }

        Builder connection(int sourceModuleId, int sourceGateId, int destModuleId, int destGateId) {
            append("connection from ", CONSTANT_TEXT_STYLE);

            ModuleCreatedEntry sourceModuleCreatedEntry = eventLogInput.getEventLog().getModuleCreatedEntry(sourceModuleId);

            if (sourceModuleCreatedEntry != null) {
                module(sourceModuleCreatedEntry);
                append(" ", CONSTANT_TEXT_STYLE);
                gate(sourceModuleId, sourceGateId);
            }

            ModuleCreatedEntry destModuleCreatedEntry = eventLogInput.getEventLog().getModuleCreatedEntry(destModuleId);

            if (destModuleCreatedEntry != null) {
                append(" to ", CONSTANT_TEXT_STYLE);
                module(destModuleCreatedEntry);
                append(" ", CONSTANT_TEXT_STYLE);
                gate(destModuleId, destGateId);
            }
            return this;
        }

        Builder message(MessageEntry messageEntry) {
            append("message ", CONSTANT_TEXT_STYLE);

            if (messageEntry != null) {
                append("(" + messageEntry.getMessageClassName() + ") ", TYPE_STYLE);
                append(messageEntry.getMessageName(), NAME_STYLE);
            }
            else
                append("<unknown>", NAME_STYLE);
            return this;
        }

        Builder messageArrivalTime(EndSendEntry endSendEntry) {
            BigDecimal arrivalTime = endSendEntry.getArrivalTime();
            BigDecimal simulationTime = endSendEntry.getEvent().getSimulationTime();
            append(arrivalTime + "s", DATA_STYLE);
            append(", now + ", CONSTANT_TEXT_STYLE);
            append(TimeUtils.secondsToTimeString(arrivalTime.toBigDecimal().subtract(simulationTime.toBigDecimal())), DATA_STYLE);
            return this;
        }

    }

    private static Styler createStyle(Color foreground) {
        return createStyle(foreground, null, false);
    }

    private static Styler createStyle(Color foreground, boolean bold) {
        return createStyle(foreground, null, bold);
    }

    private static Styler createStyle(final Color foreground, final Color background, final boolean bold) {
        return new Styler() {
            @Override
            public void applyStyles(TextStyle textStyle) {
                textStyle.foreground = foreground;
                textStyle.background = background;
                textStyle.font = bold ? BOLD_FONT : FONT;
            }
        };
    }
}
