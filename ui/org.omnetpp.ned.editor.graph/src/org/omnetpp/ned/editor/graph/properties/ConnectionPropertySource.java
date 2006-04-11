package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.ned.editor.graph.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.SubmoduleDisplayString;

public class ConnectionPropertySource extends AbstractNedPropertySource {

    public static String PROP_NAME = "name"; //$NON-NLS-1$
    public static String PROP_DISPLAY = "display"; //$NON-NLS-1$
    protected static IPropertyDescriptor[] descriptors;
    
    static {
        PropertyDescriptor nameProp = new TextPropertyDescriptor(PROP_NAME, "Name");
        PropertyDescriptor displayProp = new TextPropertyDescriptor(PROP_DISPLAY, "Display");
        descriptors = new IPropertyDescriptor[] { nameProp, displayProp };
    }

    protected ConnectionNodeEx model;
    protected ConnectionDisplayPropertySource connectionDisplayPropertySource;
    
    public ConnectionPropertySource(ConnectionNodeEx connectionNodeModel) {
        super(connectionNodeModel);
        model = connectionNodeModel;
        // create a nested displayPropertySource
        connectionDisplayPropertySource = 
            new ConnectionDisplayPropertySource(model);
    }

    public Object getEditableValue() {
        // we don't need this if we don't want to embed this property source into an other propertysource
        return model.toString();
    }

    public IPropertyDescriptor[] getPropertyDescriptors() {
        return descriptors;
    }

    @Override
    public Object getPropertyValue(Object propName) {
        if (PROP_NAME.equals(propName)) { 
            return model.toString(); 
        }
        if (PROP_DISPLAY.equals(propName)) { 
            return connectionDisplayPropertySource; 
        }
        return null;
    }

    @Override
    public void setPropertyValue(Object propName, Object value) {
        if (PROP_NAME.equals(propName)) {
//            model.setName(value.toString());
        }
        if (PROP_DISPLAY.equals(propName)) {
            model.setDisplayString(new SubmoduleDisplayString(value.toString()));
        }
    }

    @Override
    public boolean isPropertySet(Object propName) {
        return PROP_NAME.equals(propName) || PROP_DISPLAY.equals(propName);
    }

    @Override
    public void resetPropertyValue(Object propName) {
        if (PROP_DISPLAY.equals(propName)) {
            model.setDisplayString(null);
        }
    }

    @Override
    public boolean isPropertyResettable(Object propName) {
        return PROP_DISPLAY.equals(propName);
    }

}
