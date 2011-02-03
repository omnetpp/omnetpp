package org.omnetpp.common.eventlog;

import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.common.properties.PropertySource;

public class EventLogModulePropertySource extends PropertySource {
    private static final String PROPERTY_ID                 = "Id";
    private static final String PROPERTY_FULL_NAME          = "FullName";
    private static final String PROPERTY_FULL_PATH          = "FullPath";
    private static final String PROPERTY_INDEX              = "Index";
    private static final String PROPERTY_TYPE_NAME          = "TypeName";
    private static final String PROPERTY_CLASS_NAME         = "ClassName";
    private static final String PROPERTY_DISPLAY_STRING     = "DisplayString";
    private static final String PROPERTY_PARENT             = "Parent";

    private IPropertySourceProvider propertySourceProvider;
    private EventLogModule eventLogModule;

    public EventLogModulePropertySource(IPropertySourceProvider propertySourceProvider, EventLogModule eventLogModule) {
        this.propertySourceProvider = propertySourceProvider;
        this.eventLogModule = eventLogModule;
    }

    @Override
    public Object getEditableValue() {
        return eventLogModule.getFullPath();
    }

    @org.omnetpp.common.properties.Property(id = PROPERTY_ID, category = "Id")
    public int getId() { return eventLogModule.getId(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_FULL_NAME, category = "Name")
    public String getFullName() { return eventLogModule.getFullName(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_FULL_PATH, category = "Name")
    public String getFullPath() { return eventLogModule.getFullPath(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_TYPE_NAME, category = "Name")
    public String getTypeName() { return eventLogModule.getTypeName(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_CLASS_NAME, category = "Name")
    public String getClassName() { return eventLogModule.getClassName(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_INDEX)
    public int getIndex() { return eventLogModule.getIndex(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_DISPLAY_STRING)
    public String getDisplayString() { return eventLogModule.getDisplayString() == null ? "" : eventLogModule.getDisplayString().toString(); }

    @org.omnetpp.common.properties.Property(id = PROPERTY_PARENT)
    public IPropertySource getParent() { return propertySourceProvider.getPropertySource(eventLogModule.getParentModule()); }
}
