package org.omnetpp.sequencechart.widgets;

import java.math.BigDecimal;
import java.util.ArrayList;

import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.eventlog.IEvent;
import org.omnetpp.eventlog.IMessageDependency;
import org.omnetpp.eventlog.entry.ComponentMethodBeginEntry;
import org.omnetpp.eventlog.entry.MessageDescriptionEntry;
import org.omnetpp.eventlog.entry.ModuleCreatedEntry;

public interface ISequenceChartLabelProvider
{
    public void setEventLogInput(EventLogInput eventLogInput);

    public String getAxisLabel(ModuleTreeItem axisModule);

    public String getTickLabel(BigDecimal tick);

    public String getEventLabel(IEvent event);

    public String getMessageDependencyLabel(IMessageDependency messageDependency);

    /**
     * Returns a possibly multi-line human readable description for the given objects.
     */
    public String getDescriptiveText(ArrayList<Object> objects, boolean formatted);

    /**
     * Returns a human readable description for the event.
     */
    public String getEventText(IEvent event, boolean formatted);

    /**
     * Returns a human readable description for the module.
     */
    public String getModuleText(ModuleCreatedEntry moduleCreatedEntry, boolean formatted);

    /**
     * Returns a human readable description for the module.
     */
    public String getAxisText(ModuleTreeItem axisModule, boolean formatted);

    /**
     * Returns a human readable description for the message dependency.
     */
    public String getMessageDependencyText(IMessageDependency messageDependency, boolean formatted);

    /**
     * Returns a human readable description for the message.
     */
    public String getMessageDetailText(MessageDescriptionEntry messageDescriptionEntry, boolean formatted);

    /**
     * Returns a human readable description for the event numbers of the given message dependency.
     */
    public String getMessageDependencyEventNumbersText(IMessageDependency messageDependency, boolean formatted);

    /**
     * Returns a human readable description for the message name.
     */
    public String getMessageNameText(MessageDescriptionEntry messageDescriptionEntry, boolean formatted);

    /**
     * Returns a human readable description for the method call.
     */
    public String getComponentMethodCallText(ComponentMethodBeginEntry componentMethodCall, boolean formatted);

    /**
     * Returns a human readable description for the simulation time delta.
     */
    public String getSimulationTimeDeltaText(IMessageDependency messageDependency, boolean formatted);

    /**
     * Returns a human readable description for the message ids of the given message entry.
     */
    public String getMessageIdText(MessageDescriptionEntry messageDescriptionEntry, boolean formatted);

    public Image getExpandImage();

    public Image getCollapseImage();

    public Image getCloseImage();
}