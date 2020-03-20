package org.omnetpp.sequencechart.widgets;

import java.math.BigDecimal;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.eventlog.engine.FilteredMessageDependency;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.SequenceChartFacade;

public final class SequenceChartStyleProvider implements ISequenceChartStyleProvider {
    @SuppressWarnings("unused")
    private IEventLog eventLog;
    @SuppressWarnings("unused")
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
    public final Color getBackgroundColor() {
        return ColorFactory.WHITE;
    }

    @Override
    public final Color getSelectionColor() {
        return ColorFactory.RED;
    }

    @Override
    public final Color getHighlightColor() {
        return ColorFactory.DARK_RED;
    }

    @Override
    public final Color getBookmarkColor() {
        return ColorFactory.GREEN3;
    }

    @Override
    public final Color getZeroSimulationTimeRegionColor() {
        return ColorFactory.GREY94;
    }

    @Override
    public final Color getLongArrowheadColor() {
        return ColorFactory.WHITE;
    }

    @Override
    public final Color getInfoBackgroundColor() {
        return ColorFactory.LIGHT_CYAN;
    }

    @Override
    public final Color getInfoLabelColor() {
        return ColorFactory.BLACK;
    }

    @Override
    public final Color getGutterBackgroundColor() {
        return new Color(null, 255, 255, 160);
    }

    @Override
    public final Color getGutterBorderColor() {
        return ColorFactory.BLACK;
    }

    @Override
    public final Color getTickLineColor() {
        return ColorFactory.DARK_GREY;
    }

    @Override
    public final Color getMouseTickLineColor() {
        return ColorFactory.BLACK;
    }

    @Override
    public final Font getAxisLabelFont(ModuleTreeItem axisModule) {
        return null;
    }

    @Override
    public final Color getAxisLabelColor(ModuleTreeItem axisModule) {
        return ColorFactory.BLACK;
    }

    @Override
    public final Font getTickLabelFont(BigDecimal tick) {
        return null;
    }

    @Override
    public final Color getTickLabelColor(BigDecimal tick) {
        return ColorFactory.BLACK;
    }

    @Override
    public final Font getEventLabelFont(long eventPtr) {
        return null;
    }

    @Override
    public final Color getEventLabelColor(long eventPtr) {
        if (isInitializationEvent(eventPtr))
            return ColorFactory.BLACK;
        else if (sequenceChartFacade.IEvent_isSelfMessageProcessingEvent(eventPtr))
            return ColorFactory.GREEN4;
        else
            return ColorFactory.RED4;
    }

    @Override
    public final Color getEventStrokeColor(long eventPtr) {
        return getEventLabelColor(eventPtr);
    }

    @Override
    public final Color getEventFillColor(long eventPtr) {
        if (isInitializationEvent(eventPtr))
            return ColorFactory.WHITE;
        else if (sequenceChartFacade.IEvent_isSelfMessageProcessingEvent(eventPtr))
            return ColorFactory.GREEN2;
        else
            return ColorFactory.RED;
    }

    @Override
    public final Font getMessageDependencyLabelFont(long messageDependencyPtr) {
        return null;
    }

    @Override
    public final Color getMessageDependencyColor(long messageDependencyPtr) {
        Color messageSendColor = ColorFactory.BLUE;
        Color messageReuseColor = ColorFactory.GREEN4;

        if (sequenceChartFacade.IMessageDependency_isFilteredMessageDependency(messageDependencyPtr)) {
            switch (sequenceChartFacade.FilteredMessageDependency_getKind(messageDependencyPtr)) {
                case FilteredMessageDependency.Kind.SENDS:
                    return messageSendColor;
                case FilteredMessageDependency.Kind.REUSES:
                    return messageReuseColor;
                case FilteredMessageDependency.Kind.MIXED:
                    return ColorFactory.CYAN4;
                default:
                    throw new RuntimeException("Unknown kind");
            }
        }
        else {
            if (sequenceChartFacade.IMessageDependency_isReuse(messageDependencyPtr))
                return messageReuseColor;
            else
                return messageSendColor;
        }
    }

    @Override
    public final int getMessageDependencyLineStyle(long messageDependencyPtr) {
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

    @Override
    public final int[] getMessageDependencyLineDash(long messageDependencyPtr) {
        int[] dottedLinePattern = new int[] {2, 2}; // 2px black, 2px gap
        int[] dashedLinePattern = new int[] {4, 4}; // 4px black, 4px gap

        if (sequenceChartFacade.IMessageDependency_isFilteredMessageDependency(messageDependencyPtr)) {
            switch (sequenceChartFacade.FilteredMessageDependency_getKind(messageDependencyPtr)) {
                case FilteredMessageDependency.Kind.SENDS:
                    return null;
                case FilteredMessageDependency.Kind.REUSES:
                    return dottedLinePattern; // SWT.LINE_DOT style is not what we want
                case FilteredMessageDependency.Kind.MIXED:
                    return dashedLinePattern;
                default:
                    throw new RuntimeException("Unknown kind");
            }
        }
        else {
            if (sequenceChartFacade.IMessageDependency_isReuse(messageDependencyPtr))
                return dottedLinePattern; // SWT.LINE_DOT style is not what we want
            else
                return null;
        }
    }

    @Override
    public final Color getComponentMethodCallColor(long moduleMethodCallPtr) {
        return ColorFactory.ORANGE4;
    }

    @Override
    public final int getComponentMethodCallLineStyle(long messageDependencyPtr) {
        return SWT.LINE_SOLID;
    }

    @Override
    public final int[] getComponentMethodCallLineDash(long moduleMethodCallPtr) {
        return null;
    }

    @Override
    public final int getTextSpacing() {
        return 2;
    }

    @Override
    public final int getTickSpacing() {
        return 100;
    }

    @Override
    public final int getAxisOffset() {
        return 20;
    }

    @Override
    public final int getArrowheadLength() {
        return 10;
    }

    @Override
    public final int getArrowheadWidth() {
        return 7;
    }

    @Override
    public final int getMinimumHalfEllipseHeight() {
        return 15;
    }

    @Override
    public final int getLongMessageArrowWidth() {
        return 80;
    }

    @Override
    public final int getEventRadius() {
        return 3;
    }

    @Override
    public final int getEventSelectionRadius() {
        return 10;
    }

    private boolean isInitializationEvent(long eventPtr) {
        return sequenceChartFacade.IEvent_getEventNumber(eventPtr) == 0;
    }
}
