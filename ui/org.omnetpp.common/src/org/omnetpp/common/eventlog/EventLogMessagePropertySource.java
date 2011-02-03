package org.omnetpp.common.eventlog;

import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.properties.PropertySource;

public class EventLogMessagePropertySource extends PropertySource {
    private static final String PROPERTY_NAME                   = "Name";
    private static final String PROPERTY_CLASS_NAME             = "ClassName";

    private static final String PROPERTY_KIND                   = "Kind";
    private static final String PROPERTY_PRIORITY               = "Priority";
    private static final String PROPERTY_BIT_LENGTH             = "Length";
    private static final String PROPERTY_BIT_ERROR              = "Error";

    private static final String PROPERTY_ID                     = "Id";
    private static final String PROPERTY_TREE_ID                = "TreeId";
    private static final String PROPERTY_ENCAPSULATION_ID       = "EncapsulationId";
    private static final String PROPERTY_ENCAPSULATIUON_TREE_ID = "EncapsulationTreeId";

    private static final String PROPERTY_SENDER_MODULE          = "SenderModule";
    private static final String PROPERTY_SENDER_GATE            = "SenderGate";

    private static final String PROPERTY_ARRIVAL_MODULE         = "ArrivalModule";
    private static final String PROPERTY_ARRIVAL_GATE           = "ArrivalGate";

    private static final String PROPERTY_SENDING_TIME           = "SendingTime";
    private static final String PROPERTY_ARRIVAL_TIME           = "ArrivalTime";

    private IPropertySourceProvider propertySourceProvider;
    private EventLogMessage eventLogMessage;

    public EventLogMessagePropertySource(IPropertySourceProvider propertySourceProvider, EventLogMessage eventLogMessage) {
        this.eventLogMessage = eventLogMessage;
        this.propertySourceProvider = propertySourceProvider;
    }

    @Override
    public Object getEditableValue() {
        return eventLogMessage.getName();
    }

    @org.omnetpp.common.properties.Property(id = PROPERTY_NAME, category = "Name")
    public String getName() { return eventLogMessage.getName(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_CLASS_NAME, category = "Name")
    public String getClassName() { return eventLogMessage.getClassName(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_KIND)
    public short getKind() { return eventLogMessage.getKind(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_PRIORITY)
    public short getPriority() { return eventLogMessage.getSchedulingPriority(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_BIT_LENGTH)
    public int getBitLength() { return eventLogMessage.getBitLength(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_BIT_ERROR)
    public boolean getBitError() { return eventLogMessage.hasBitError(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_ID, category = "Id")
    public int getId() { return eventLogMessage.getId(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_TREE_ID, category = "Id")
    public int getTreeId() { return eventLogMessage.getTreeId(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_ENCAPSULATION_ID, category = "Id")
    public int getEncapsulationId() { return eventLogMessage.getEncapsulationId(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_ENCAPSULATIUON_TREE_ID, category = "Id")
    public int getEncapsulationTreeId() { return eventLogMessage.getEncapsulationTreeId(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_SENDING_TIME, category = "Time")
    public BigDecimal getSendingTime() { return eventLogMessage.getSendingTime(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_ARRIVAL_TIME, category = "Time")
    public BigDecimal getArrivalTime() { return eventLogMessage.getArrivalTime(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_SENDER_MODULE, category = "Sender")
    public IPropertySource getSenderModule() { return propertySourceProvider.getPropertySource(eventLogMessage.getSenderModule()); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_SENDER_GATE, category = "Sender")
    public IPropertySource getSenderGate() { return propertySourceProvider.getPropertySource(eventLogMessage.getSenderGate()); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_ARRIVAL_MODULE, category = "Arrival")
    public IPropertySource getArrivalModule() { return propertySourceProvider.getPropertySource(eventLogMessage.getArrivalModule()); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_ARRIVAL_GATE, category = "Arrival")
    public IPropertySource getArrivalGate() { return propertySourceProvider.getPropertySource(eventLogMessage.getArrivalGate()); }

}
