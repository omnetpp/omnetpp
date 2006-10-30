package org.omnetpp.ned.editor.graph.properties;

import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumSet;
import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.properties.EditableComboBoxPropertyDescriptor;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.resources.NEDResourcesPlugin;

public class ConnectionPropertySource extends MergedPropertySource {

    // Display property source
    protected static class ConnectionDisplayPropertySource extends DisplayPropertySource {
        protected static IPropertyDescriptor[] propertyDescArray;
        protected ConnectionNodeEx model;

        public ConnectionDisplayPropertySource(ConnectionNodeEx model) {
            super(model);
            this.model = model;
            setDisplayString(model.getDisplayString());
            // define which properties should be displayed in the property sheet
            // we do not support all properties currently, just colow, width ans style
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.CONNECTION_COL, 
                    								 DisplayString.Prop.CONNECTION_STYLE));
            
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.TEXT, DisplayString.Prop.TEXTPOS));
            supportedProperties.add(DisplayString.Prop.TOOLTIP);
        }

        @Override
        public void modelChanged() {
            if(model != null)
                setDisplayString(model.getDisplayString());
        }

    }

    // Connection specific properties
    protected static class BasePropertySource implements IPropertySource2 {
        public enum Prop { Channel, SrcGate, DestGate }
        protected IPropertyDescriptor[] descriptors;
        protected ConnectionNodeEx model;
        EditableComboBoxPropertyDescriptor channelProp;

        public BasePropertySource(ConnectionNodeEx connectionNodeModel) {
            model = connectionNodeModel;
            
            // set up property descriptors
			channelProp = new EditableComboBoxPropertyDescriptor(Prop.Channel, "channel");
            channelProp.setCategory("Base");
            channelProp.setDescription("The channel type of the connection");
            
            PropertyDescriptor srcGateProp = new PropertyDescriptor(Prop.SrcGate, "source-gate");
            srcGateProp.setCategory("Base");
            srcGateProp.setDescription("The source gate of the connection (read only)");

            PropertyDescriptor destGateProp = new PropertyDescriptor(Prop.DestGate, "dest-gate");
            destGateProp.setCategory("Base");
            destGateProp.setDescription("The destination gate of the connection (read only)");

            descriptors = new IPropertyDescriptor[] { channelProp, srcGateProp, destGateProp };
        }

        public Object getEditableValue() {
            return this;
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
        	//fill the connection combobox with channel types
        	List<String> channelNames = new ArrayList<String>(NEDResourcesPlugin.getNEDResources().getChannelNames());
        	Collections.sort(channelNames);
  			channelProp.setItems(channelNames);
            return descriptors;
        }

        public Object getPropertyValue(Object propName) {
            if (Prop.Channel.equals(propName))  
                return model.getType(); 
            
            if (Prop.SrcGate.equals(propName))  
                return model.getSrcGateFullyQualified(); 

            if (Prop.DestGate.equals(propName))  
                return model.getDestGateFullyQualified(); 
            
            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
            if (Prop.Channel.equals(propName)) 
                model.setType(value.toString());
        }

        public boolean isPropertySet(Object propName) {
            if (Prop.Channel.equals(propName)) 
            	return Prop.Channel.equals(propName) && !"".equals(model.getType()) && (model.getType() != null);

            return false;
        }

        public void resetPropertyValue(Object propName) {
            if (Prop.Channel.equals(propName)) 
            	model.setType(null);
        }

        public boolean isPropertyResettable(Object propName) {
            return Prop.Channel.equals(propName);
        }
    }

    // constructor 
    public ConnectionPropertySource(ConnectionNodeEx connectionNodeModel) {
        super(connectionNodeModel);
        // create a nested displayPropertySources
        mergePropertySource(new BasePropertySource(connectionNodeModel));	
        mergePropertySource(new ConnectionDisplayPropertySource(connectionNodeModel));
        
    }
}

