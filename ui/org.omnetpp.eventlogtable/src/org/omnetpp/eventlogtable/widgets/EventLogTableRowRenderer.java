/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.eventlogtable.widgets;

import java.util.ArrayList;

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
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.TimeUtils;
import org.omnetpp.common.virtualtable.IVirtualTableRowRenderer;
import org.omnetpp.eventlog.Event;
import org.omnetpp.eventlog.EventLogEntry;
import org.omnetpp.eventlog.EventLogMessageEntry;
import org.omnetpp.eventlog.IEvent;
import org.omnetpp.eventlog.IMessageDependency;
import org.omnetpp.eventlog.entry.BeginSendEntry;
import org.omnetpp.eventlog.entry.BubbleEntry;
import org.omnetpp.eventlog.entry.CancelEventEntry;
import org.omnetpp.eventlog.entry.CloneMessageEntry;
import org.omnetpp.eventlog.entry.ComponentMethodBeginEntry;
import org.omnetpp.eventlog.entry.ComponentMethodEndEntry;
import org.omnetpp.eventlog.entry.ConnectionCreatedEntry;
import org.omnetpp.eventlog.entry.ConnectionDeletedEntry;
import org.omnetpp.eventlog.entry.ConnectionDisplayStringChangedEntry;
import org.omnetpp.eventlog.entry.CreateMessageEntry;
import org.omnetpp.eventlog.entry.CustomChangedEntry;
import org.omnetpp.eventlog.entry.CustomCreatedEntry;
import org.omnetpp.eventlog.entry.CustomDeletedEntry;
import org.omnetpp.eventlog.entry.CustomEntry;
import org.omnetpp.eventlog.entry.CustomFoundEntry;
import org.omnetpp.eventlog.entry.DeleteMessageEntry;
import org.omnetpp.eventlog.entry.EndSendEntry;
import org.omnetpp.eventlog.entry.EventEntry;
import org.omnetpp.eventlog.entry.GateCreatedEntry;
import org.omnetpp.eventlog.entry.GateDeletedEntry;
import org.omnetpp.eventlog.entry.GateDescriptionEntry;
import org.omnetpp.eventlog.entry.MessageDescriptionEntry;
import org.omnetpp.eventlog.entry.ModuleCreatedEntry;
import org.omnetpp.eventlog.entry.ModuleDeletedEntry;
import org.omnetpp.eventlog.entry.ModuleDescriptionEntry;
import org.omnetpp.eventlog.entry.ModuleDisplayStringChangedEntry;
import org.omnetpp.eventlog.entry.SendDirectEntry;
import org.omnetpp.eventlog.entry.SendHopEntry;
import org.omnetpp.eventlog.entry.SimulationBeginEntry;
import org.omnetpp.eventlog.entry.SimulationEndEntry;
import org.omnetpp.eventlogtable.widgets.EventLogTable.NameMode;

public class EventLogTableRowRenderer implements IVirtualTableRowRenderer<EventLogEntryReference> {
    private static final boolean isLightTheme = !DisplayUtils.isDarkTheme(); 
    private static final Font FONT;
    private static final Font BOLD_FONT;
    private static final Color FOREGROUND_COLOR = DisplayUtils.FOREGROUND_COLOR;
    private static final Color HIGHLIGHT_COLOR = isLightTheme ? ColorFactory.BLUE3 : ColorFactory.CYAN3;
    private static final Color MESSAGE_COLOR = isLightTheme ? new Color(null, 127, 0, 85) : ColorFactory.SALMON;
    private static final Color BUBBLE_COLOR = isLightTheme ? ColorFactory.RED2 : ColorFactory.PERU;
    private static final Color FAINT_FOREGROUND_COLOR = isLightTheme ? ColorFactory.GREY83 : ColorFactory.GREY40;
    private static final Color BOOKMARK_COLOR = isLightTheme ? ColorFactory.LIGHT_CYAN : new Color(48, 32, 64);
    private static final Styler EVENT_ENTRY_EVENT_NUMBER_STYLE = createStyle(FOREGROUND_COLOR);
    private static final Styler EVENT_LOG_ENTRY_EVENT_NUMBER_STYLE = createStyle(FAINT_FOREGROUND_COLOR);
    private static final Styler EVENT_ENTRY_SIMULATION_TIME_STYLE = createStyle(FOREGROUND_COLOR);
    private static final Styler EVENT_LOG_ENTRY_SIMULATION_TIME_STYLE = createStyle(FAINT_FOREGROUND_COLOR);
    private static final Styler CONSTANT_TEXT_STYLE = createStyle(FOREGROUND_COLOR);
    private static final Styler BOLD_CONSTANT_TEXT_STYLE = createStyle(FOREGROUND_COLOR, true);
    private static final Styler RAW_VALUE_STYLE = createStyle(HIGHLIGHT_COLOR);
    private static final Styler TYPE_STYLE = createStyle(HIGHLIGHT_COLOR);
    private static final Styler NAME_STYLE = createStyle(HIGHLIGHT_COLOR, true);
    private static final Styler EVENT_LOG_MESSAGE_STYLE = createStyle(MESSAGE_COLOR);
    private static final Styler BUBBLE_ENTRY_STYLE = createStyle(BUBBLE_COLOR, true);
    private static final Styler DATA_STYLE = createStyle(HIGHLIGHT_COLOR);
    private static final int HORIZONTAL_SPACING = 4;
    private static final int INDENT_SPACING = HORIZONTAL_SPACING * 4;
    private static final int VERTICAL_SPACING = 3;
    private static final String ANSI_CONTROL_SEQUENCE_REGEX = "\u001b\\[[\\d;]*[A-HJKSTfimnsu]";

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

    @Override
    public void setInput(Object eventLogInput) {
        this.eventLogInput = (EventLogInput)eventLogInput;
    }

    @Override
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

    private BeginSendEntry findBeginSendEntry(long previousEventNumber, long messageId) {
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
        try {
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
                             .append(getSimulationTimeText(simulationTime), isEventLogEntry ? EVENT_ENTRY_SIMULATION_TIME_STYLE : EVENT_LOG_ENTRY_SIMULATION_TIME_STYLE)
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
        finally {
            contextEvent = null;
        }
    }

    private StyledString getRawStyledText(EventLogEntry eventLogEntry, boolean isSelected) {
        Builder builder = new Builder(isSelected);
        if (!(eventLogEntry instanceof EventLogMessageEntry)) {
            builder.append(eventLogEntry.getAsString() + " ", BOLD_CONSTANT_TEXT_STYLE);
        }

        ArrayList<String> stringVector = eventLogEntry.getAttributeNames();
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
            if (eventNumber == 0)
                builder.constant("Initializing ").module(1);
            else {
                String fingerprints = contextEvent.getEventEntry().getFingerprints();
                if (fingerprints != null)
                    builder.data(fingerprints + " ");
                IMessageDependency cause = contextEvent.getCause();

                builder.constant("Event in ")
                       .module(contextEvent.getModuleId(), EventLogTable.NameMode.FULL_PATH);

                MessageDescriptionEntry beginSendEntry = cause != null ? cause.getBeginMessageDescriptionEntry() : null;
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
            else if (eventLogEntry instanceof ComponentMethodBeginEntry) {
                ComponentMethodBeginEntry componentMethodBeginEntry = (ComponentMethodBeginEntry)eventLogEntry;
                builder.constant("Begin calling ");
                String method = componentMethodBeginEntry.getMethodName();

                if (StringUtils.isEmpty(method))
                    builder.constant("method");
                else
                    builder.data(method);

                builder.constant(" in ").module(componentMethodBeginEntry.getTargetComponentId());

                if (contextEvent.getModuleId() != componentMethodBeginEntry.getContextModuleId())
                    builder.constant(" from ").module(componentMethodBeginEntry.getSourceComponentId());
            }
            else if (eventLogEntry instanceof ComponentMethodEndEntry) {
                builder.constant("End calling module");
            }
            else if (eventLogEntry instanceof ModuleCreatedEntry) {
                ModuleCreatedEntry moduleCreatedEntry = (ModuleCreatedEntry)eventLogEntry;
                builder.constant("Creating ").module(moduleCreatedEntry);
                int parentModuleId = moduleCreatedEntry.getParentModuleId();
                if (parentModuleId != -1)
                    builder.constant(" under ").module(parentModuleId);
                builder.constant(" started");
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
                    builder.constant(" transmission delay = ").data(getSimulationTimeText(sendHopEntry.getTransmissionDelay()));
                }

                if (sendHopEntry.getPropagationDelay().doubleValue() != 0) {
                    builder.constant(" propagation delay = ").data(getSimulationTimeText(sendHopEntry.getPropagationDelay()));
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
                       .constant(" with transmission delay ").data(getSimulationTimeText(sendDirectEntry.getTransmissionDelay()))
                       .constant(" and propagation delay ").data(getSimulationTimeText(sendDirectEntry.getPropagationDelay()));
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
            else if (eventLogEntry instanceof CustomCreatedEntry) {
                CustomCreatedEntry customCreatedEntry = (CustomCreatedEntry)eventLogEntry;
                builder.constant("Custom created entry: ")
                       .constant("type = ")
                       .data(customCreatedEntry.getType())
                       .constant(", key = ")
                       .data(String.valueOf(customCreatedEntry.getKey()))
                       .constant(", content = ")
                       .data(customCreatedEntry.getContent());
            }
            else if (eventLogEntry instanceof CustomDeletedEntry) {
                CustomDeletedEntry customDeletedEntry = (CustomDeletedEntry)eventLogEntry;
                builder.constant("Custom deleted entry: ")
                       .constant("type = ")
                       .data(customDeletedEntry.getType())
                       .constant(", key = ")
                       .data(String.valueOf(customDeletedEntry.getKey()));
            }
            else if (eventLogEntry instanceof CustomChangedEntry) {
                CustomChangedEntry customChangedEntry = (CustomChangedEntry)eventLogEntry;
                builder.constant("Custom changed entry, ")
                       .constant("type = ")
                       .data(customChangedEntry.getType())
                       .constant(", key = ")
                       .data(String.valueOf(customChangedEntry.getKey()))
                       .constant(", content = ")
                       .data(customChangedEntry.getContent());
            }
            else if (eventLogEntry instanceof CustomFoundEntry) {
                CustomFoundEntry customFoundEntry = (CustomFoundEntry)eventLogEntry;
                builder.constant("Custom found entry: ")
                       .constant("type = ")
                       .data(customFoundEntry.getType())
                       .constant(", key = ")
                       .data(String.valueOf(customFoundEntry.getKey()))
                       .constant(", content = ")
                       .data(customFoundEntry.getContent());
            }
            else if (eventLogEntry instanceof CustomEntry) {
                CustomEntry customEntry = (CustomEntry)eventLogEntry;
                builder.constant("Custom entry: ")
                       .constant("type = ")
                       .data(customEntry.getType())
                       .constant(", key = ")
                       .data(String.valueOf(customEntry.getKey()))
                       .constant(", content = ")
                       .data(customEntry.getContent());
            }
            else
                throw new RuntimeException("Unknown eventlog entry: " + eventLogEntry.getClassName());
        }

        return builder.getStyledString();
    }

    private String getSimulationTimeText(BigDecimal simulationTime) {
        // TODO: make this a parameter
        if (true)
            return TimeUtils.secondsToTimeString(simulationTime.toBigDecimal());
        else
            return simulationTime.toString();
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
                    if (eventLogTable.getCanvas().isFocusControl())
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
            return append(value.replaceAll(ANSI_CONTROL_SEQUENCE_REGEX, ""), RAW_VALUE_STYLE);
        }

        Builder bubble(String text) {
            return append(text, BUBBLE_ENTRY_STYLE);
        }

        Builder eventLogMessage(String text) {
            // TODO: maybe implement actual formatting, using something like
            // https://github.com/mihnita/ansi-econsole ?
            return append(text.replaceAll(ANSI_CONTROL_SEQUENCE_REGEX, ""), EVENT_LOG_MESSAGE_STYLE);
        }

        Builder module(int moduleId) {
            return module(eventLogInput.getEventLog().getEventLogEntryCache().getModuleDescriptionEntry(moduleId));
        }

        Builder module(int moduleId, NameMode nameMode) {
            return module(eventLogInput.getEventLog().getEventLogEntryCache().getModuleDescriptionEntry(moduleId), nameMode);
        }

        Builder module(ModuleDescriptionEntry moduleDescriptionEntry) {
            return module(moduleDescriptionEntry, eventLogTable.getNameMode());
        }

        Builder module(ModuleDescriptionEntry moduleDescriptionEntry, NameMode nameMode) {
            append("module ", CONSTANT_TEXT_STYLE);

            if (moduleDescriptionEntry != null) {
                String typeName = null;

                switch (eventLogTable.getTypeMode()) {
                case NED:
                    typeName = moduleDescriptionEntry.getNedTypeName();
                    break;
                case CPP:
                    typeName = moduleDescriptionEntry.getModuleClassName();
                    break;
                }

                append("(" + typeName + ") ", TYPE_STYLE);

                switch (nameMode) {
                case SMART_NAME:
                    if (contextEvent.getModuleId() == moduleDescriptionEntry.getModuleId())
                        moduleFullName(moduleDescriptionEntry);
                    else
                        moduleFullPath(moduleDescriptionEntry);
                    break;
                case FULL_NAME:
                    moduleFullName(moduleDescriptionEntry);
                    break;
                case FULL_PATH:
                    moduleFullPath(moduleDescriptionEntry);
                    break;
                }
            }
            else
                append("<unknown>", NAME_STYLE);

            return this;
        }

        Builder moduleFullPath(ModuleDescriptionEntry moduleDescriptionEntry) {
            return append(eventLogInput.getEventLogTableFacade().ModuleDescriptionEntry_getModuleFullPath(moduleDescriptionEntry), NAME_STYLE);
        }

        Builder moduleFullName(ModuleDescriptionEntry moduleDescriptionEntry) {
            return append(moduleDescriptionEntry.getFullName(), NAME_STYLE);
        }

        Builder gate(int moduleId, int gateId) {
            return gate(eventLogInput.getEventLog().getEventLogEntryCache().getGateDescriptionEntry(moduleId, gateId));
        }

        Builder gate(GateDescriptionEntry gateDescriptionEntry) {
            append("gate ", CONSTANT_TEXT_STYLE);

            if (gateDescriptionEntry != null) {
                int index = gateDescriptionEntry.getIndex();
                append(gateDescriptionEntry.getName() + (index != -1 ? "[" + index + "]" : ""), NAME_STYLE);
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

            ModuleDescriptionEntry sourceModuleDescriptionEntry = eventLogInput.getEventLog().getEventLogEntryCache().getModuleDescriptionEntry(sourceModuleId);

            if (sourceModuleDescriptionEntry != null) {
                module(sourceModuleDescriptionEntry);
                append(" ", CONSTANT_TEXT_STYLE);
                gate(sourceModuleId, sourceGateId);
            }

            ModuleDescriptionEntry destModuleDescriptionEntry = eventLogInput.getEventLog().getEventLogEntryCache().getModuleDescriptionEntry(destModuleId);

            if (destModuleDescriptionEntry != null) {
                append(" to ", CONSTANT_TEXT_STYLE);
                module(destModuleDescriptionEntry);
                append(" ", CONSTANT_TEXT_STYLE);
                gate(destModuleId, destGateId);
            }
            return this;
        }

        Builder message(MessageDescriptionEntry messageDescriptionEntry) {
            append("message ", CONSTANT_TEXT_STYLE);

            if (messageDescriptionEntry != null) {
                append("(" + messageDescriptionEntry.getMessageClassName() + ") ", TYPE_STYLE);
                append(messageDescriptionEntry.getMessageName(), NAME_STYLE);
            }
            else
                append("<unknown>", NAME_STYLE);
            return this;
        }

        Builder messageArrivalTime(EndSendEntry endSendEntry) {
            BigDecimal arrivalTime = endSendEntry.getArrivalTime();
            BigDecimal simulationTime = endSendEntry.getEvent().getSimulationTime();
            append(getSimulationTimeText(arrivalTime), DATA_STYLE);
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
