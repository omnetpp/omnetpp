package org.omnetpp.common.eventlog;

import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.common.properties.PropertySource;

public class EventLogGatePropertySource extends PropertySource {
    private static final String PROPERTY_ID                 = "Id";
    private static final String PROPERTY_NAME               = "Name";
    private static final String PROPERTY_INDEX              = "Index";
    private static final String PROPERTY_VECTOR_SIZE        = "VectorSize";

    private IPropertySourceProvider propertySourceProvider;
    private EventLogGate eventLogGate;

    public EventLogGatePropertySource(IPropertySourceProvider iPropertySourceProvider, EventLogGate eventLogGate) {
        this.eventLogGate = eventLogGate;
    }

    @Override
    public Object getEditableValue() {
        return eventLogGate.getName();
    }

    @org.omnetpp.common.properties.Property(id = PROPERTY_ID, category = "Id")
    public int getId() { return eventLogGate.getId(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_NAME, category = "Name")
    public String getName() { return eventLogGate.getName(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_INDEX)
    public int getIndex() { return eventLogGate.getIndex(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_VECTOR_SIZE)
    public int getVectorSize() { return eventLogGate.getVectorSize(); }
}
