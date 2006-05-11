package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.ned2.model.ConnectionNodeEx;

public class ConnectionPropertySource extends AbstractNedPropertySource {

    protected static IPropertyDescriptor[] descriptors;
    
    public enum Prop { Channel, SrcModule, DestModule , Display }

    static {
        PropertyDescriptor channelProp = new TextPropertyDescriptor(Prop.Channel, "Channel");
        PropertyDescriptor displayProp = new TextPropertyDescriptor(Prop.Display, "Display");
        descriptors = new IPropertyDescriptor[] { channelProp, displayProp };
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

    @Override
    public Object getEditableValue() {
        // we don't need this if we don't want to embed this property source into an other propertysource
        return model.toString();
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        return descriptors;
    }

    @Override
    public Object getPropertyValue(Object propName) {
        if (Prop.Channel.equals(propName)) { 
            return model.getChannelType(); 
        }
        if (Prop.Display.equals(propName)) { 
            return connectionDisplayPropertySource; 
        }
        return null;
    }

    @Override
    public void setPropertyValue(Object propName, Object value) {
        if (Prop.Channel.equals(propName)) {
            model.setChannelType(value.toString());
        }
        if (Prop.Display.equals(propName)) {
            model.getDisplayString().set(value.toString());
        }
    }

    @Override
    public boolean isPropertySet(Object propName) {
        return Prop.Channel.equals(propName) || Prop.Display.equals(propName);
    }

    @Override
    public void resetPropertyValue(Object propName) {
        if (Prop.Display.equals(propName)) {
            model.getDisplayString().set(null);
        }
    }

    @Override
    public boolean isPropertyResettable(Object propName) {
        return Prop.Display.equals(propName);
    }

}
