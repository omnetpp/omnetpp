package org.omnetpp.sequencechart.widgets;

import java.math.BigDecimal;
import java.util.ArrayList;

import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.TimeUtils;
import org.omnetpp.eventlog.engine.ComponentMethodBeginEntry;
import org.omnetpp.eventlog.engine.EventLogMessageEntry;
import org.omnetpp.eventlog.engine.FilteredMessageDependency;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.IMessageDependency;
import org.omnetpp.eventlog.engine.MessageEntry;
import org.omnetpp.eventlog.engine.MessageReuseDependency;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.SequenceChartFacade;
import org.omnetpp.sequencechart.SequenceChartPlugin;

public final class SequenceChartLabelProvider implements ISequenceChartLabelProvider {
    private static final String ANSI_CONTROL_SEQUENCE_REGEX = "\u001b\\[[\\d;]*[A-HJKSTfimnsu]";
    private static final String TOOL_IMAGE_DIR = "icons/full/etool16/";

    private boolean debug = false;

    private IEventLog eventLog;
    private EventLogInput eventLogInput;
    private SequenceChartFacade sequenceChartFacade;

    @Override
    public final void setEventLogInput(EventLogInput newEventLogInput) {
        if (newEventLogInput == null) {
            eventLog = null;
            eventLogInput = null;
            sequenceChartFacade = null;
        }
        else {
            eventLog = newEventLogInput.getEventLog();
            eventLogInput = newEventLogInput;
            sequenceChartFacade = newEventLogInput.getSequenceChartFacade();
        }
    }

    @Override
    public final String getAxisLabel(ModuleTreeItem axisModule) {
        String fullPath = axisModule.getModuleFullPath();
        return fullPath.substring(fullPath.indexOf('.') + 1);
    }

    @Override
    public final String getTickLabel(BigDecimal tick) {
        return TimeUtils.secondsToTimeString(tick);
    }

    @Override
    public final String getEventLabel(long eventPtr) {
        return "#" + sequenceChartFacade.IEvent_getEventNumber(eventPtr);
    }

    @Override
    public final String getMessageDependencyLabel(long messageDependencyPtr) {
        if (sequenceChartFacade.IMessageDependency_isFilteredMessageDependency(messageDependencyPtr))
            return sequenceChartFacade.FilteredMessageDependency_getBeginMessageName(messageDependencyPtr) +
                   " -> " +
                   sequenceChartFacade.FilteredMessageDependency_getEndMessageName(messageDependencyPtr);
        else
            return sequenceChartFacade.IMessageDependency_getMessageName(messageDependencyPtr);
    }

    @Override
    public final String getDescriptiveText(ArrayList<Object> objects, boolean formatted) {
        String result = "";
        String boldStart = formatted ? "<b>" : "";
        String boldEnd = formatted ? "</b>" : "";
        for (Object object : objects) {
            if (object instanceof IEvent) {
                IEvent event = (IEvent)object;
                if (result.length() != 0)
                    result += "<br/>";
                result += getEventText(event, formatted);

                if (objects.stream().filter(o -> o instanceof IEvent).count() == 1) {
                    for (int i = 0; i < event.getNumEventLogMessages(); i++) {
                        EventLogMessageEntry eventLogMessageEntry = event.getEventLogMessage(i);

                        result += "<br/><span style=\"color:rgb(127, 0, 85)\"> - " + eventLogMessageEntry.getText().replaceAll(ANSI_CONTROL_SEQUENCE_REGEX, "") + "</span>";

                        if (i == 100) {
                            result += "<br/><br/>Content stripped after 100 lines. See Eventlog Table for more details.";
                            break;
                        }
                    }
                }
                result += "<br/>";
            }
            else if (object instanceof IMessageDependency) {
                IMessageDependency messageDependency = (IMessageDependency)object;
                if (result.length() != 0)
                    result += "<br/>";
                result += getMessageDependencyText(messageDependency, formatted);
                result += "<br/>";
            }
            else if (object instanceof ComponentMethodBeginEntry) {
                ComponentMethodBeginEntry componentMethodCall = (ComponentMethodBeginEntry)object;
                if (result.length() != 0)
                    result += "<br/>";
                result += getComponentMethodCallText(componentMethodCall, formatted);
                result += "<br/>";
            }
            else if (object instanceof ModuleTreeItem) {
                ModuleTreeItem axisModule = (ModuleTreeItem)object;
                if (result.length() != 0)
                    result += "<br/>";
                result += getAxisText(axisModule, formatted);
                result += "<br/>";
            }
            else if (object instanceof Double) {
                if (!eventLog.isEmpty() && result.length() != 0) {
                    double timelineCoordinate = (Double)object;
                    if (result.length() != 0)
                        result += "<br/>";
                    org.omnetpp.common.engine.BigDecimal simulationTime = sequenceChartFacade.getSimulationTimeForTimelineCoordinate(timelineCoordinate);
                    result += boldStart + "At" + boldEnd + " t = " + boldStart + getSimulationTimeText(simulationTime) + boldEnd;
                    IEvent event = sequenceChartFacade.getLastEventNotAfterTimelineCoordinate(timelineCoordinate);
                    if (event != null && !event.getSimulationTime().equals(simulationTime))
                        result += ", just after event #" + event.getEventNumber();
                    event = sequenceChartFacade.getFirstEventNotBeforeTimelineCoordinate(timelineCoordinate);
                    if (event != null && !event.getSimulationTime().equals(simulationTime))
                        result += ", just before event #" + event.getEventNumber();
                }
                result += "<br/>";
            }
        }
        if (!result.equals("") && !result.equals("<br/>"))
            return result;
        else
            return null;
    }

    @Override
    public final String getEventText(IEvent event, boolean formatted) {
        String boldStart = formatted ? "<b>" : "";
        String boldEnd = formatted ? "</b>" : "";

        ModuleCreatedEntry moduleCreatedEntry = eventLog.getModuleCreatedEntry(event.getModuleId());
        String result = boldStart + "Event" + boldEnd + " #" + boldStart + event.getEventNumber() + boldEnd;

        if (formatted)
            result += " at t = " + boldStart + getSimulationTimeText(event.getSimulationTime()) + boldEnd;

        result += " in ";

        if (formatted)
            result += "module ";

        result += getModuleText(moduleCreatedEntry, formatted) + " (id = " + event.getModuleId() + ")";

        IMessageDependency messageDependency = event.getCause();
        MessageEntry messageEntry = null;

        if (messageDependency instanceof FilteredMessageDependency)
            messageDependency = ((FilteredMessageDependency)messageDependency).getEndMessageDependency();

        if (messageDependency != null)
            messageEntry = messageDependency.getMessageEntry();

        if (formatted && messageEntry != null) {
            result += " message (" + messageEntry.getMessageClassName() + ") " + boldStart + messageEntry.getMessageName() + boldEnd;
            result += getMessageIdText(messageEntry, formatted);
        }

        if (debug)
            result += "<br/>Timeline Coordinate: " + sequenceChartFacade.getTimelineCoordinateBegin(event);

        return result;
    }

    @Override
    public final String getModuleText(ModuleCreatedEntry moduleCreatedEntry, boolean formatted) {
        String boldStart = formatted ? "<b>" : "";
        String boldEnd = formatted ? "</b>" : "";
        String moduleName = (formatted ? eventLogInput.getEventLogTableFacade().ModuleCreatedEntry_getModuleFullPath(moduleCreatedEntry.getCPtr()) : moduleCreatedEntry.getFullName());
        String typeName = moduleCreatedEntry.getNedTypeName();

        if (!formatted && typeName.contains("."))
            typeName = StringUtils.substringAfterLast(typeName, ".");

        return "(" + typeName + ") " +  boldStart + moduleName + boldEnd;
    }

    @Override
    public final String getAxisText(ModuleTreeItem axisModule, boolean formatted) {
        String boldStart = formatted ? "<b>" : "";
        String boldEnd = formatted ? "</b>" : "";
        String moduleName = (formatted ? axisModule.getModuleFullPath() : axisModule.getModuleName());
        String moduleId = " (id = " + axisModule.getModuleId() + ")";

        return boldStart + "Axis" + boldEnd + " (" + axisModule.getNedTypeName() + ") " + boldStart + moduleName + boldEnd + moduleId;
    }

    @Override
    public final String getMessageDependencyText(IMessageDependency messageDependency, boolean formatted) {
        String boldStart = formatted ? "<b>" : "";
        String boldEnd = formatted ? "</b>" : "";
        if (sequenceChartFacade.IMessageDependency_isFilteredMessageDependency(messageDependency.getCPtr())) {
            FilteredMessageDependency filteredMessageDependency = (FilteredMessageDependency)messageDependency;
            MessageEntry beginMessageEntry = filteredMessageDependency.getBeginMessageDependency().getMessageEntry();
            MessageEntry endMessageEntry = filteredMessageDependency.getEndMessageDependency().getMessageEntry();
            boolean sameMessage = beginMessageEntry.getMessageId() == endMessageEntry.getMessageId();

            String kind = "";
            switch (((FilteredMessageDependency)messageDependency).getKind()) {
                case FilteredMessageDependency.Kind.SENDS:
                    kind = "message sends ";
                    break;
                case FilteredMessageDependency.Kind.REUSES:
                    kind = "message reuses ";
                    break;
                case FilteredMessageDependency.Kind.MIXED:
                    kind = "message sends and message reuses ";
                    break;
                default:
                    throw new RuntimeException("Unknown kind");
            }
            String result = boldStart + "Derived " + boldEnd + kind + getMessageNameText(beginMessageEntry, formatted);
            if (!sameMessage)
                result += " -> " + getMessageNameText(endMessageEntry, formatted);

            if (formatted)
                result += getMessageDependencyEventNumbersText(messageDependency, formatted) + getSimulationTimeDeltaText(messageDependency, formatted);

            result += getMessageIdText(beginMessageEntry, formatted);
            if (!sameMessage)
                result += " -> " + getMessageIdText(endMessageEntry, formatted);

            if (formatted) {
                if (beginMessageEntry.getDetail() != null) {
                    if (!sameMessage)
                        result += "<br/>First: ";

                    result += getMessageDetailText(beginMessageEntry, formatted);
                }

                if (!sameMessage && endMessageEntry.getDetail() != null) {
                    result += "<br/>Last: ";
                    result += getMessageDetailText(endMessageEntry, formatted);
                }
            }

            return result;
        }
        else {
            MessageEntry beginSendEntry = messageDependency.getMessageEntry();
            String result = boldStart + (messageDependency instanceof MessageReuseDependency ? "Reusing " : "Sending ") + boldEnd + getMessageNameText(beginSendEntry, formatted);

            if (formatted)
                result += getMessageDependencyEventNumbersText(messageDependency, formatted) + getSimulationTimeDeltaText(messageDependency, formatted);

            result += getMessageIdText(beginSendEntry, formatted);

            if (formatted && beginSendEntry.getDetail() != null)
                result += getMessageDetailText(beginSendEntry, formatted);

            return result;
        }
    }

    @Override
    public final String getMessageDetailText(MessageEntry messageEntry, boolean formatted) {
        String detail = messageEntry.getDetail();
        int longestLineLength = 0;
        for (String line : detail.split("\n"))
            longestLineLength = Math.max(longestLineLength, line.length());
        return "<br/><pre>" + StringUtils.quoteForHtml(detail) + "</pre>";
    }

    @Override
    public final String getMessageDependencyEventNumbersText(IMessageDependency messageDependency, boolean formatted) {
        return " (#" + messageDependency.getCauseEventNumber() + " -> #" + messageDependency.getConsequenceEventNumber() + ")";
    }

    @Override
    public final String getMessageNameText(MessageEntry messageEntry, boolean formatted) {
        String boldStart = formatted ? "<b>" : "";
        String boldEnd = formatted ? "</b>" : "";
        return "(" + messageEntry.getMessageClassName() + ") " + boldStart + messageEntry.getMessageName() + boldEnd;
    }

    @Override
    public final String getComponentMethodCallText(ComponentMethodBeginEntry componentMethodCall, boolean formatted) {
        String boldStart = formatted ? "<b>" : "";
        String boldEnd = formatted ? "</b>" : "";
        ModuleCreatedEntry fromModuleCreatedEntry = eventLog.getModuleCreatedEntry(componentMethodCall.getSourceComponentId()); // null if component is a channel
        ModuleCreatedEntry toModuleCreatedEntry = eventLog.getModuleCreatedEntry(componentMethodCall.getTargetComponentId()); // null if component is a channel
        String result = boldStart + "Calling " + componentMethodCall.getMethod() + boldEnd;
        if (fromModuleCreatedEntry != null)
            result += " from module " + getModuleText(fromModuleCreatedEntry, formatted);
        if (toModuleCreatedEntry != null)
            result += " to module " + getModuleText(toModuleCreatedEntry, formatted);
        return result;
    }

    @Override
    public final String getSimulationTimeDeltaText(IMessageDependency messageDependency, boolean formatted) {
        org.omnetpp.common.engine.BigDecimal causeSimulationTime = messageDependency.getCauseSimulationTime();
        org.omnetpp.common.engine.BigDecimal consequenceSimulationTime = messageDependency.getConsequenceSimulationTime();
        return " dt = " + getSimulationTimeText(consequenceSimulationTime.subtract(causeSimulationTime));
    }

    @Override
    public final String getMessageIdText(MessageEntry messageEntry, boolean formatted) {
        int messageId = messageEntry.getMessageId();
        String result = " (id = " + messageId;

        if (formatted) {
            int messageTreeId = messageEntry.getMessageTreeId();
            if (messageTreeId != -1 && messageTreeId != messageId)
                result += ", tree id = " + messageTreeId;

            int messageEncapsulationId = messageEntry.getMessageEncapsulationId();
            if (messageEncapsulationId != -1 && messageEncapsulationId != messageId)
                result += ", encapsulation id = " + messageEncapsulationId;

            int messageEncapsulationTreeId = messageEntry.getMessageEncapsulationTreeId();
            if (messageEncapsulationTreeId != -1 && messageEncapsulationTreeId != messageEncapsulationId)
                result += ", encapsulation tree id = " + messageEncapsulationTreeId;
        }

        result += ")";
        return result;
    }

    @Override
    public final Image getExpandImage() {
        return SequenceChartPlugin.getCachedImage(TOOL_IMAGE_DIR + "expand.png");
    }

    @Override
    public final Image getCollapseImage() {
        return SequenceChartPlugin.getCachedImage(TOOL_IMAGE_DIR + "collapse.png");
    }

    @Override
    public final Image getCloseImage() {
        return SequenceChartPlugin.getCachedImage(TOOL_IMAGE_DIR + "close.png");
    }

    private String getSimulationTimeText(org.omnetpp.common.engine.BigDecimal simulationTime) {
        // TODO: make this a parameter
        if (true)
            return TimeUtils.secondsToTimeString(simulationTime.toBigDecimal());
        else
            return simulationTime.toString();
    }
}
