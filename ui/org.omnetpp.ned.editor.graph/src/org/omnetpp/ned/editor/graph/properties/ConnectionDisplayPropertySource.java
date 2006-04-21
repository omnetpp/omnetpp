package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.DisplayString;

public class ConnectionDisplayPropertySource extends DisplayPropertySource {
    protected static IPropertyDescriptor[] propertyDescArray;
    protected ConnectionNodeEx model;


    public ConnectionDisplayPropertySource(ConnectionNodeEx model) {
        super(model);
        this.model = model;
        setDisplayString(model.getDisplayString());
        // define which properties should be displayed in the property sheet
        supportedProperties = EnumSet.range(DisplayString.Prop.ROUTING_MODE, 
                                            DisplayString.Prop.BENDPOINTS);
        supportedProperties.addAll(EnumSet.range(DisplayString.Prop.TEXT, DisplayString.Prop.TEXTPOS));
        supportedProperties.add(DisplayString.Prop.TOOLTIP);
    }

    @Override
    protected void fireDisplayStringChanged() {
        if(model != null)
            model.setDisplayString(getDisplayString());
    }

    @Override
    public void modelChanged() {
        if(model != null)
            setDisplayString(model.getDisplayString());
    }

}
