package org.omnetpp.sequencechart.widgets;

import java.math.BigDecimal;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;

public interface ISequenceChartStyleProvider
{
    void setEventLogInput(EventLogInput eventLogInput);

    Color getBackgroundColor();

    Color getSelectionColor();

    Color getHighlightColor();

    Color getBookmarkColor();

    Color getZeroSimulationTimeRegionColor();

    Color getLongArrowheadColor();

    Color getInfoBackgroundColor();

    Color getInfoLabelColor();

    Color getGutterBackgroundColor();

    Color getGutterBorderColor();

    Color getTickLineColor();

    Color getMouseTickLineColor();

    Font getAxisLabelFont(ModuleTreeItem axisModule);

    Color getAxisLabelColor(ModuleTreeItem axisModule);

    Font getTickLabelFont(BigDecimal tick);

    Color getTickLabelColor(BigDecimal tick);

    Font getEventLabelFont(long eventPtr);

    Color getEventLabelColor(long eventPtr);

    Color getEventStrokeColor(long eventPtr);

    Color getEventFillColor(long eventPtr);

    Font getMessageDependencyLabelFont(long messageDependencyPtr);

    Color getMessageDependencyColor(long messageDependencyPtr);

    int getMessageDependencyLineStyle(long messageDependencyPtr);

    int[] getMessageDependencyLineDash(long messageDependencyPtr);

    Color getComponentMethodCallColor(long moduleMethodCallPtr);

    int getComponentMethodCallLineStyle(long messageDependencyPtr);

    int[] getComponentMethodCallLineDash(long moduleMethodCallPtr);

    int getTextSpacing();

    int getTickSpacing();

    /**
     * Returns the extra y distance before and after first and last axes.
     */
    int getAxisOffset();

    int getArrowheadLength();

    int getArrowheadWidth();

    /**
     * Returns the vertical radius of ellipse for message arrows on one axis.
     */
    int getMinimumHalfEllipseHeight();

    /**
     * Returns the width for too long message lines and half ellipses.
     */
    int getLongMessageArrowWidth();

    int getEventRadius();

    /**
     * Returns the radius of the event selection mark circle.
     */
    int getEventSelectionRadius();
}