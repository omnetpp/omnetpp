package org.omnetpp.ned.editor.graph.properties;

import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumSet;
import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.properties.util.DelegatingPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.DisplayPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ParameterListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.TypePropertySource;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.ConnectionNodeEx;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
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
            // we do not support all properties currently, just color, width and style
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.CONNECTION_COL,
                    								 DisplayString.Prop.CONNECTION_STYLE));

            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.TEXT, DisplayString.Prop.TEXTCOLOR));
            supportedProperties.add(DisplayString.Prop.TOOLTIP);
        }

    }

    // Connection specific properties
    protected static class BasePropertySource implements IPropertySource2 {
        public enum Prop { SrcGate, DestGate }
        protected IPropertyDescriptor[] descriptors;
        protected ConnectionNodeEx model;

        public BasePropertySource(ConnectionNodeEx connectionNodeModel) {
            model = connectionNodeModel;

            // set up property descriptors
            PropertyDescriptor srcGateProp = new PropertyDescriptor(Prop.SrcGate, "source-gate");
            srcGateProp.setCategory("Base");
            srcGateProp.setDescription("The source gate of the connection - (read only)");

            PropertyDescriptor destGateProp = new PropertyDescriptor(Prop.DestGate, "dest-gate");
            destGateProp.setCategory("Base");
            destGateProp.setDescription("The destination gate of the connection - (read only)");

            descriptors = new IPropertyDescriptor[] { srcGateProp, destGateProp };
        }

        public Object getEditableValue() {
            return this;
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            return descriptors;
        }

        public Object getPropertyValue(Object propName) {
            if (Prop.SrcGate.equals(propName))
                return model.getSrcGateFullyQualified();

            if (Prop.DestGate.equals(propName))
                return model.getDestGateFullyQualified();

            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
        }

        public boolean isPropertySet(Object propName) {
            return false;
        }

        public void resetPropertyValue(Object propName) {
        }

        public boolean isPropertyResettable(Object propName) {
            return false;
        }
    }

    // constructor
    public ConnectionPropertySource(ConnectionNodeEx connectionNodeModel) {
        super(connectionNodeModel);
        // create a nested displayPropertySources
        mergePropertySource(new BasePropertySource(connectionNodeModel));
        // type
        mergePropertySource(new TypePropertySource(connectionNodeModel) {
            @Override
            protected List<String> getPossibleValues() {
                List<String> channelNames = new ArrayList<String>(NEDResourcesPlugin.getNEDResources().getChannelNames());
                Collections.sort(channelNames);
              return channelNames;
            }
        });
        mergePropertySource(new ConnectionDisplayPropertySource(connectionNodeModel));
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(connectionNodeModel),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));

    }
}

