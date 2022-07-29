package org.omnetpp.sequencechart.widgets;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.omnetpp.common.util.BigDecimal;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.eventlog.IEvent;
import org.omnetpp.eventlog.IMessageDependency;
import org.omnetpp.eventlog.entry.ComponentMethodBeginEntry;

public interface ISequenceChartStyleProvider
{
    public void setEventLogInput(EventLogInput eventLogInput);

    public Color getBackgroundColor();

    public Color getSelectionColor();

    public Color getHighlightColor();

    public Color getBookmarkColor();

    public Color getZeroSimulationTimeRegionColor();

    public Color getLongArrowheadColor();

    public Color getInfoBackgroundColor();

    public Color getInfoLabelColor();

    public Color getGutterBackgroundColor();

    public Color getGutterBorderColor();

    public Color getTickLineColor();

    public Color getMouseTickLineColor();

    public Color getAxisHeaderColor(ModuleTreeItem axisModule);

    public Color getAxisColor(ModuleTreeItem axisModule);

    public Font getAxisLabelFont(ModuleTreeItem axisModule);

    public Color getAxisLabelColor(ModuleTreeItem axisModule);

    public Font getTickLabelFont(BigDecimal tick);

    public Color getTickLabelColor(BigDecimal tick);

    public Font getEventLabelFont(IEvent event);

    public Color getEventLabelColor(IEvent event);

    public Color getEventStrokeColor(IEvent event);

    public Color getEventFillColor(IEvent event);

    public Font getMessageDependencyLabelFont(IMessageDependency messageDependency);

    public Color getMessageDependencyColor(IMessageDependency messageDependency);

    public int getMessageDependencyLineStyle(IMessageDependency messageDependency);

    public int[] getMessageDependencyLineDash(IMessageDependency messageDependency);

    public Color getComponentMethodCallColor(ComponentMethodBeginEntry moduleMethodCall);

    public int getComponentMethodCallLineStyle(ComponentMethodBeginEntry messageDependency);

    public int[] getComponentMethodCallLineDash(ComponentMethodBeginEntry moduleMethodCall);

    public int getComponentMethodCallReturnLineStyle(ComponentMethodBeginEntry messageDependency);

    public int[] getComponentMethodCallReturnLineDash(ComponentMethodBeginEntry moduleMethodCall);

    public int getTextSpacing();

    public int getTickSpacing();

    /**
     * Returns the extra y distance before and after first and last axes.
     */
    public int getAxisOffset();

    public int getArrowheadLength();

    public int getArrowheadWidth();

    /**
     * Returns the vertical radius of ellipse for message arrows on one axis.
     */
    public int getMinimumHalfEllipseHeight();

    /**
     * Returns the width for too long message lines and half ellipses.
     */
    public int getLongMessageArrowWidth();

    public int getEventRadius();

    /**
     * Returns the radius of the event selection mark circle.
     */
    public int getEventSelectionRadius();
}