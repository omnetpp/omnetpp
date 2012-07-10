package org.omnetpp.simulation.model.c;

import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.common.properties.PropertySource;

public class ModuleModelPropertySource extends PropertySource {
    private static final String PROPERTY_ID             = "Id";
    private static final String PROPERTY_FULL_NAME      = "FullName";
    private static final String PROPERTY_FULL_PATH      = "FullPath";
    private static final String PROPERTY_INDEX          = "Index";
    private static final String PROPERTY_TYPE_NAME      = "TypeName";
    private static final String PROPERTY_CLASS_NAME     = "ClassName";
    private static final String PROPERTY_DISPLAY_STRING = "DisplayString";
    private static final String PROPERTY_PARENT         = "Parent";

    private IPropertySourceProvider propertySourceProvider;
    private ModuleModel simulationModule;

    public ModuleModelPropertySource(IPropertySourceProvider propertySourceProvider, ModuleModel simulationModule) {
        this.propertySourceProvider = propertySourceProvider;
        this.simulationModule = simulationModule;
    }

    @Override
    public Object getEditableValue() {
        return simulationModule.getFullPath();
    }

    @org.omnetpp.common.properties.Property(id = PROPERTY_ID, category = "Id")
    public int getId() { return simulationModule.getId(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_FULL_NAME, category = "Name")
    public String getFullName() { return simulationModule.getFullName(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_FULL_PATH, category = "Name")
    public String getFullPath() { return simulationModule.getFullPath(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_TYPE_NAME, category = "Name")
    public String getTypeName() { return simulationModule.getTypeName(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_CLASS_NAME, category = "Name")
    public String getClassName() { return simulationModule.getClassName(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_INDEX)
    public int getIndex() { return simulationModule.getIndex(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_DISPLAY_STRING)
    public String getDisplayString() { return simulationModule.getDisplayString() == null ? "" : simulationModule.getDisplayString().toString(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_PARENT)
    public IPropertySource getParent() { return propertySourceProvider.getPropertySource(simulationModule.getParentModule()); }
}
