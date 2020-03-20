package org.omnetpp.sequencechart.widgets;

import java.math.BigDecimal;
import java.util.ArrayList;

import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.eventlog.engine.ComponentMethodBeginEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IMessageDependency;
import org.omnetpp.eventlog.engine.MessageEntry;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;

public interface ISequenceChartLabelProvider
{
    void setEventLogInput(EventLogInput eventLogInput);

    String getAxisLabel(ModuleTreeItem axisModule);

    String getTickLabel(BigDecimal tick);

    String getEventLabel(long eventPtr);

    String getMessageDependencyLabel(long messageDependencyPtr);

    /**
     * Returns a possibly multi-line human readable description for the given objects.
     */
    String getDescriptiveText(ArrayList<Object> objects, boolean formatted);

    /**
     * Returns a human readable description for the event.
     */
    String getEventText(IEvent event, boolean formatted);

    /**
     * Returns a human readable description for the module.
     */
    String getModuleText(ModuleCreatedEntry moduleCreatedEntry, boolean formatted);

    /**
     * Returns a human readable description for the module.
     */
    String getAxisText(ModuleTreeItem axisModule, boolean formatted);

    /**
     * Returns a human readable description for the message dependency.
     */
    String getMessageDependencyText(IMessageDependency messageDependency, boolean formatted);

    /**
     * Returns a human readable description for the message.
     */
    String getMessageDetailText(MessageEntry messageEntry, boolean formatted);

    /**
     * Returns a human readable description for the event numbers of the given message dependency.
     */
    String getMessageDependencyEventNumbersText(IMessageDependency messageDependency, boolean formatted);

    /**
     * Returns a human readable description for the message name.
     */
    String getMessageNameText(MessageEntry messageEntry, boolean formatted);

    /**
     * Returns a human readable description for the method call.
     */
    String getComponentMethodCallText(ComponentMethodBeginEntry componentMethodCall, boolean formatted);

    /**
     * Returns a human readable description for the simulation time delta.
     */
    String getSimulationTimeDeltaText(IMessageDependency messageDependency, boolean formatted);

    /**
     * Returns a human readable description for the message ids of the given message entry.
     */
    String getMessageIdText(MessageEntry messageEntry, boolean formatted);

    Image getExpandImage();

    Image getCollapseImage();

    Image getCloseImage();

}