package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.ned2.model.ChannelNodeEx;

public class ChannelPropertySource extends MergedPropertySource {

    protected static class ChannelDisplayPropertySource extends DisplayPropertySource {
        protected ChannelNodeEx model;

        public ChannelDisplayPropertySource(ChannelNodeEx model) {
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

    /**
     * Constructor
     * @param nodeModel
     */
    public ChannelPropertySource(ChannelNodeEx nodeModel) {
    	super(nodeModel);
        mergePropertySource(new NamePropertySource(nodeModel));
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(nodeModel),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        // create a displayPropertySource
        mergePropertySource(new ChannelDisplayPropertySource(nodeModel));
    }

}
