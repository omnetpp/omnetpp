package org.omnetpp.simulation.model.c;

import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.common.properties.PropertySource;

public class ConnectionModelPropertySource extends PropertySource {
    private static final String PROPERTY_SOURCE_MODULE      = "SourceModule";
    private static final String PROPERTY_SOURCE_GATE        = "SourceGate";
    private static final String PROPERTY_DESTINATION_MODULE = "DestinationModule";
    private static final String PROPERTY_DESTINATION_GATE   = "DestinationGate";
    private static final String PROPERTY_DISPLAY_STRING     = "DisplayString";

    private IPropertySourceProvider propertySourceProvider;
    private ConnectionModel simulationConnection;

    public ConnectionModelPropertySource(IPropertySourceProvider propertySourceProvider, ConnectionModel simulationConnection) {
        this.propertySourceProvider = propertySourceProvider;
        this.simulationConnection = simulationConnection;
    }

    @org.omnetpp.common.properties.Property(id = PROPERTY_SOURCE_MODULE, category = "Source")
    public IPropertySource getSourceModule() { return propertySourceProvider.getPropertySource(simulationConnection.getSourceModule()); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_SOURCE_GATE, category = "Source")
    public IPropertySource getSourceGate() { return propertySourceProvider.getPropertySource(simulationConnection.getSourceGate()); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_DESTINATION_MODULE, category = "Destination")
    public IPropertySource getDestinationModule() { return propertySourceProvider.getPropertySource(simulationConnection.getDestinationModule()); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_DESTINATION_GATE, category = "Destination")
    public IPropertySource getDestinationGate() { return propertySourceProvider.getPropertySource(simulationConnection.getDestinationGate()); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_DISPLAY_STRING)
    public String getDisplayString() { return simulationConnection.getDisplayString() == null ? "" : simulationConnection.getDisplayString().toString(); }
}
