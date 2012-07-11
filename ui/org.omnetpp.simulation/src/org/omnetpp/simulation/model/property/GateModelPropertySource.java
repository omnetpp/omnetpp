package org.omnetpp.simulation.model.property;

import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.simulation.model.cGate;

public class GateModelPropertySource extends PropertySource {
    private static final String PROPERTY_ID           = "Id";
    private static final String PROPERTY_NAME         = "Name";
    private static final String PROPERTY_INDEX        = "Index";
    private static final String PROPERTY_VECTOR_SIZE  = "VectorSize";
    private static final String PROPERTY_OWNER_MODULE = "OwnerModule";

    private IPropertySourceProvider propertySourceProvider;
    private cGate simulationGate;

    public GateModelPropertySource(IPropertySourceProvider propertySourceProvider, cGate simulationGate) {
        this.propertySourceProvider = propertySourceProvider;
        this.simulationGate = simulationGate;
    }

    @Override
    public Object getEditableValue() {
        return simulationGate.getName();
    }

    @org.omnetpp.common.properties.Property(id = PROPERTY_ID, category = "Id")
    public int getId() { return simulationGate.getId(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_NAME, category = "Name")
    public String getName() { return simulationGate.getName(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_INDEX)
    public int getIndex() { return simulationGate.getIndex(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_VECTOR_SIZE)
    public int getVectorSize() { return simulationGate.getVectorSize(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_OWNER_MODULE)
    public IPropertySource getOwnerModule() { return propertySourceProvider.getPropertySource(simulationGate.getOwnerModule()); }
}
