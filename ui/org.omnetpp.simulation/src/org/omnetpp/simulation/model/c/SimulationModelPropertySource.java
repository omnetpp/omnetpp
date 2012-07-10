package org.omnetpp.simulation.model.c;

import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.common.properties.PropertySource;

public class SimulationModelPropertySource extends PropertySource {
    private static final String PROPERTY_ROOT_MODULE = "rootModule";

    private IPropertySourceProvider propertySourceProvider;
    private SimulationModel simulation;

    public SimulationModelPropertySource(IPropertySourceProvider propertySourceProvider, SimulationModel simulation) {
        this.propertySourceProvider = propertySourceProvider;
        this.simulation = simulation;
    }

    @org.omnetpp.common.properties.Property(id = PROPERTY_ROOT_MODULE)
    public IPropertySource getSourceModule() { return propertySourceProvider.getPropertySource(simulation.getRootModule()); }
}
