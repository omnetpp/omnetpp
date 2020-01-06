package org.omnetpp.sequencechart.widgets;

import java.math.BigDecimal;

import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.common.util.TimeUtils;
import org.omnetpp.eventlog.engine.FilteredMessageDependency;
import org.omnetpp.eventlog.engine.SequenceChartFacade;

public class SequenceChartStyleProvider {
    private static final Color AXIS_LABEL_COLOR = ColorFactory.BLACK;
    private static final Color TICK_LABEL_COLOR = ColorFactory.BLACK;

    private static final Color MESSAGE_SEND_COLOR = ColorFactory.BLUE;
    private static final Color MESSAGE_REUSE_COLOR = ColorFactory.GREEN4;
    private static final Color MIXED_MESSAGE_DEPENDENCY_COLOR = ColorFactory.CYAN4;
    private static final Color MODULE_METHOD_CALL_COLOR = ColorFactory.ORANGE3;

    private static final Color INITIALIZATION_EVENT_BORDER_COLOR = ColorFactory.BLACK;
    private static final Color INITIALIZATION_EVENT_BACKGROUND_COLOR = ColorFactory.WHITE;
    private static final Color EVENT_BORDER_COLOR = ColorFactory.RED4;
    private static final Color EVENT_BACKGROUND_COLOR = ColorFactory.RED;
    private static final Color SELF_EVENT_BORDER_COLOR = ColorFactory.GREEN4;
    private static final Color SELF_EVENT_BACKGROUND_COLOR = ColorFactory.GREEN2;

    private static final int[] DOTTED_LINE_PATTERN = new int[] {2, 2}; // 2px black, 2px gap
    private static final int[] DASHED_LINE_PATTERN = new int[] {4, 4}; // 4px black, 4px gap

    private SequenceChartFacade sequenceChartFacade;

    public SequenceChartStyleProvider() {
    }

    public void setEventLogInput(EventLogInput eventLogInput) {
        this.sequenceChartFacade = eventLogInput.getSequenceChartFacade();
    }

    // axis

    public String getAxisLabel(ModuleTreeItem axisModule) {
        return axisModule.getModuleFullPath();
    }

    public Font getAxisLabelFont(ModuleTreeItem axisModule) {
        return JFaceResources.getDefaultFont();
    }

    public Color getAxisLabelColor(ModuleTreeItem axisModule) {
        return AXIS_LABEL_COLOR;
    }

    // tick

    public String getTickLabel(BigDecimal tick) {
        return TimeUtils.secondsToTimeString(tick);
    }

    public Font getTickLabelFont(BigDecimal tick) {
        return JFaceResources.getDefaultFont();
    }

    public Color getTickLabelColor(BigDecimal tick) {
        return TICK_LABEL_COLOR;
    }

    // event

    public String getEventLabel(long eventPtr) {
        return "#" + sequenceChartFacade.IEvent_getEventNumber(eventPtr);
    }

    public Font getEventLabelFont(long eventPtr) {
        return JFaceResources.getDefaultFont();
    }

    public Color getEventLabelColor(long eventPtr) {
        if (isInitializationEvent(eventPtr))
            return INITIALIZATION_EVENT_BORDER_COLOR;
        else if (sequenceChartFacade.IEvent_isSelfMessageProcessingEvent(eventPtr))
            return SELF_EVENT_BORDER_COLOR;
        else
            return EVENT_BORDER_COLOR;
    }

    public Color getEventStrokeColor(long eventPtr) {
        return getEventLabelColor(eventPtr);
    }

    public Color getEventFillColor(long eventPtr) {
        if (isInitializationEvent(eventPtr))
            return INITIALIZATION_EVENT_BACKGROUND_COLOR;
        else if (sequenceChartFacade.IEvent_isSelfMessageProcessingEvent(eventPtr))
            return SELF_EVENT_BACKGROUND_COLOR;
        else
            return EVENT_BACKGROUND_COLOR;
    }

    // message dependency

    public String getMessageDependencyLabel(long messageDependencyPtr) {
        if (sequenceChartFacade.IMessageDependency_isFilteredMessageDependency(messageDependencyPtr))
            return sequenceChartFacade.FilteredMessageDependency_getBeginMessageName(messageDependencyPtr) +
                   " -> " +
                   sequenceChartFacade.FilteredMessageDependency_getEndMessageName(messageDependencyPtr);
        else
            return sequenceChartFacade.IMessageDependency_getMessageName(messageDependencyPtr);
    }

    public Font getMessageDependencyLabelFont(long messageDependencyPtr) {
        return JFaceResources.getDefaultFont();
    }

    public Color getMessageDependencyLabelColor(long messageDependencyPtr) {
        return getMessageDependencyStrokeColor(messageDependencyPtr);
    }

    public Color getMessageDependencyStrokeColor(long messageDependencyPtr) {
        if (sequenceChartFacade.IMessageDependency_isFilteredMessageDependency(messageDependencyPtr)) {
            switch (sequenceChartFacade.FilteredMessageDependency_getKind(messageDependencyPtr)) {
                case FilteredMessageDependency.Kind.SENDS:
                    return MESSAGE_SEND_COLOR;
                case FilteredMessageDependency.Kind.REUSES:
                    return MESSAGE_REUSE_COLOR;
                case FilteredMessageDependency.Kind.MIXED:
                    return MIXED_MESSAGE_DEPENDENCY_COLOR;
                default:
                    throw new RuntimeException("Unknown kind");
            }
        }
        else {
            if (sequenceChartFacade.IMessageDependency_isReuse(messageDependencyPtr))
                return MESSAGE_REUSE_COLOR;
            else
                return MESSAGE_SEND_COLOR;
        }
    }

    public int getMessageDependencyLineStyle(long messageDependencyPtr) {
        if (sequenceChartFacade.IMessageDependency_isFilteredMessageDependency(messageDependencyPtr)) {
            switch (sequenceChartFacade.FilteredMessageDependency_getKind(messageDependencyPtr)) {
                case FilteredMessageDependency.Kind.SENDS:
                    return SWT.LINE_SOLID;
                case FilteredMessageDependency.Kind.REUSES:
                    return SWT.LINE_CUSTOM;
                case FilteredMessageDependency.Kind.MIXED:
                    return SWT.LINE_CUSTOM;
                default:
                    throw new RuntimeException("Unknown kind");
            }
        }
        else {
            if (sequenceChartFacade.IMessageDependency_isReuse(messageDependencyPtr))
                return SWT.LINE_CUSTOM;
            else
                return SWT.LINE_SOLID;
        }
    }

    public int[] getMessageDependencyLineDash(long messageDependencyPtr) {
        if (sequenceChartFacade.IMessageDependency_isFilteredMessageDependency(messageDependencyPtr)) {
            switch (sequenceChartFacade.FilteredMessageDependency_getKind(messageDependencyPtr)) {
                case FilteredMessageDependency.Kind.SENDS:
                    return null;
                case FilteredMessageDependency.Kind.REUSES:
                    return DOTTED_LINE_PATTERN; // SWT.LINE_DOT style is not what we want
                case FilteredMessageDependency.Kind.MIXED:
                    return DASHED_LINE_PATTERN;
                default:
                    throw new RuntimeException("Unknown kind");
            }
        }
        else {
            if (sequenceChartFacade.IMessageDependency_isReuse(messageDependencyPtr))
                return DOTTED_LINE_PATTERN; // SWT.LINE_DOT style is not what we want
            else
                return null;
        }
    }

    // method call

    public Color getModuleMethodCallColor(long moduleMethodCallPtr) {
        return MODULE_METHOD_CALL_COLOR;
    }

    public int getModuleMethodCallLineStyle(long messageDependencyPtr) {
        return SWT.LINE_SOLID;
    }

    public int[] getModuleMethodCallLineDash(long moduleMethodCallPtr) {
        return null;
    }

    // utils

    private boolean isInitializationEvent(long eventPtr) {
        return sequenceChartFacade.IEvent_getEventNumber(eventPtr) == 0;
    }
}
