package org.omnetpp.sequencechart.widgets;

import java.util.ArrayList;

import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.common.util.BigDecimal;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.TimeUtils;
import org.omnetpp.eventlog.EventLogMessageEntry;
import org.omnetpp.eventlog.FilteredMessageDependency;
import org.omnetpp.eventlog.IEvent;
import org.omnetpp.eventlog.IEventLog;
import org.omnetpp.eventlog.IMessageDependency;
import org.omnetpp.eventlog.MessageReuseDependency;
import org.omnetpp.eventlog.SequenceChartFacade;
import org.omnetpp.eventlog.entry.ComponentMethodBeginEntry;
import org.omnetpp.eventlog.entry.MessageDescriptionEntry;
import org.omnetpp.eventlog.entry.ModuleCreatedEntry;
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
    public final String getEventLabel(IEvent event) {
        return "#" + event.getEventNumber();
    }

    @Override
    public final String getMessageDependencyLabel(IMessageDependency messageDependency) {
        if (messageDependency instanceof FilteredMessageDependency) {
            String beginMessageName = messageDependency.getBeginMessageDescriptionEntry().getMessageName();
            String endMessageName = messageDependency.getEndMessageDescriptionEntry().getMessageName();
            return beginMessageName.equals(endMessageName) ? beginMessageName : beginMessageName + " -> " + endMessageName;
        }
        else if (messageDependency.getEndMessageDescriptionEntry() != null)
            return messageDependency.getEndMessageDescriptionEntry().getMessageName();
        else
            return messageDependency.getBeginMessageDescriptionEntry().getMessageName();
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
                    BigDecimal simulationTime = sequenceChartFacade.getSimulationTimeForTimelineCoordinate(timelineCoordinate);
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

        ModuleCreatedEntry moduleCreatedEntry = eventLog.getEventLogEntryCache().getModuleCreatedEntry(event.getModuleId());
        String result = boldStart + "Event" + boldEnd + " #" + boldStart + event.getEventNumber() + boldEnd;

        if (formatted)
            result += " at t = " + boldStart + getSimulationTimeText(event.getSimulationTime()) + boldEnd;

        result += " in ";

        if (formatted)
            result += "module ";

        result += getModuleText(moduleCreatedEntry, formatted) + " (id = " + event.getModuleId() + ")";

        IMessageDependency messageDependency = event.getCause();
        MessageDescriptionEntry messageDescriptionEntry = null;

        if (messageDependency instanceof FilteredMessageDependency)
            messageDependency = ((FilteredMessageDependency)messageDependency).getEndMessageDependency();

        if (messageDependency != null)
            messageDescriptionEntry = messageDependency.getBeginMessageDescriptionEntry();

        if (formatted && messageDescriptionEntry != null) {
            result += " message (" + messageDescriptionEntry.getMessageClassName() + ") " + boldStart + messageDescriptionEntry.getMessageName() + boldEnd;
            result += getMessageIdText(messageDescriptionEntry, formatted);
        }

        if (debug)
            result += "<br/>Timeline Coordinate: " + sequenceChartFacade.getTimelineCoordinateBegin(event);

        return result;
    }

    @Override
    public final String getModuleText(ModuleCreatedEntry moduleCreatedEntry, boolean formatted) {
        String boldStart = formatted ? "<b>" : "";
        String boldEnd = formatted ? "</b>" : "";
        String moduleName = (formatted ? eventLogInput.getEventLogTableFacade().ModuleDescriptionEntry_getModuleFullPath(moduleCreatedEntry) : moduleCreatedEntry.getFullName());
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
        if (messageDependency instanceof FilteredMessageDependency) {
            FilteredMessageDependency filteredMessageDependency = (FilteredMessageDependency)messageDependency;
            MessageDescriptionEntry beginMessageDescriptionEntry = filteredMessageDependency.getBeginMessageDependency().getEndMessageDescriptionEntry();
            MessageDescriptionEntry endMessageDescriptionEntry = filteredMessageDependency.getEndMessageDependency().getEndMessageDescriptionEntry();
            boolean sameMessage = beginMessageDescriptionEntry.getMessageId() == endMessageDescriptionEntry.getMessageId();

            String kind = "";
            switch (((FilteredMessageDependency)messageDependency).getKind()) {
                case SENDS:
                    kind = "message sends ";
                    break;
                case REUSES:
                    kind = "message reuses ";
                    break;
                case MIXED:
                    kind = "message sends and message reuses ";
                    break;
                default:
                    throw new RuntimeException("Unknown kind");
            }
            String result = boldStart + "Derived " + boldEnd + kind + getMessageNameText(beginMessageDescriptionEntry, formatted);
            if (!sameMessage)
                result += " -> " + getMessageNameText(endMessageDescriptionEntry, formatted);

            if (formatted)
                result += getMessageDependencyEventNumbersText(messageDependency, formatted) + getSimulationTimeDeltaText(messageDependency, formatted);

            result += getMessageIdText(beginMessageDescriptionEntry, formatted);
            if (!sameMessage)
                result += " -> " + getMessageIdText(endMessageDescriptionEntry, formatted);

            if (formatted) {
                if (beginMessageDescriptionEntry.getDetail() != null) {
                    if (!sameMessage)
                        result += "<br/>First: ";

                    result += getMessageDetailText(beginMessageDescriptionEntry, formatted);
                }

                if (!sameMessage && endMessageDescriptionEntry.getDetail() != null) {
                    result += "<br/>Last: ";
                    result += getMessageDetailText(endMessageDescriptionEntry, formatted);
                }
            }

            return result;
        }
        else {
            MessageDescriptionEntry beginSendEntry = messageDependency.getEndMessageDescriptionEntry();
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
    public final String getMessageDetailText(MessageDescriptionEntry messageDescriptionEntry, boolean formatted) {
        String detail = messageDescriptionEntry.getDetail();
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
    public final String getMessageNameText(MessageDescriptionEntry messageDescriptionEntry, boolean formatted) {
        String boldStart = formatted ? "<b>" : "";
        String boldEnd = formatted ? "</b>" : "";
        return "(" + messageDescriptionEntry.getMessageClassName() + ") " + boldStart + messageDescriptionEntry.getMessageName() + boldEnd;
    }

    @Override
    public final String getComponentMethodCallText(ComponentMethodBeginEntry componentMethodCall, boolean formatted) {
        String boldStart = formatted ? "<b>" : "";
        String boldEnd = formatted ? "</b>" : "";
        ModuleCreatedEntry fromModuleCreatedEntry = eventLog.getEventLogEntryCache().getModuleCreatedEntry(componentMethodCall.getSourceComponentId()); // null if component is a channel
        ModuleCreatedEntry toModuleCreatedEntry = eventLog.getEventLogEntryCache().getModuleCreatedEntry(componentMethodCall.getTargetComponentId()); // null if component is a channel
        String result = boldStart + "Calling " + componentMethodCall.getMethodName() + boldEnd;
        if (fromModuleCreatedEntry != null)
            result += " from module " + getModuleText(fromModuleCreatedEntry, formatted);
        if (toModuleCreatedEntry != null)
            result += " to module " + getModuleText(toModuleCreatedEntry, formatted);
        return result;
    }

    @Override
    public final String getSimulationTimeDeltaText(IMessageDependency messageDependency, boolean formatted) {
        BigDecimal causeSimulationTime = messageDependency.getCauseSimulationTime();
        BigDecimal consequenceSimulationTime = messageDependency.getConsequenceSimulationTime();
        return " dt = " + getSimulationTimeText(consequenceSimulationTime.subtract(causeSimulationTime));
    }

    @Override
    public final String getMessageIdText(MessageDescriptionEntry messageDescriptionEntry, boolean formatted) {
        long messageId = messageDescriptionEntry.getMessageId();
        String result = " (id = " + messageId;

        if (formatted) {
            long messageTreeId = messageDescriptionEntry.getMessageTreeId();
            if (messageTreeId != -1 && messageTreeId != messageId)
                result += ", tree id = " + messageTreeId;

            long messageEncapsulationId = messageDescriptionEntry.getMessageEncapsulationId();
            if (messageEncapsulationId != -1 && messageEncapsulationId != messageId)
                result += ", encapsulation id = " + messageEncapsulationId;

            long messageEncapsulationTreeId = messageDescriptionEntry.getMessageEncapsulationTreeId();
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

    private String getSimulationTimeText(BigDecimal simulationTime) {
        // TODO: make this a parameter
        if (true)
            return TimeUtils.secondsToTimeString(simulationTime);
        else
            return simulationTime.toString();
    }
}
