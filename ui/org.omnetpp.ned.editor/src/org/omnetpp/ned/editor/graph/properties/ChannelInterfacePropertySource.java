package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.omnetpp.ned.editor.graph.properties.util.DelegatingPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.DisplayPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ExtendsListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.NamePropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ParameterListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.TypeNameValidator;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;

/**
 * Property source for channel interfaces
 *
 * @author rhornig
 */
public class ChannelInterfacePropertySource extends MergedPropertySource {

    protected static class ChannelInterfaceDisplayPropertySource extends DisplayPropertySource {
        protected ChannelInterfaceElementEx model;

        public ChannelInterfaceDisplayPropertySource(ChannelInterfaceElementEx model) {
            super(model);
            this.model = model;
            // define which properties should be displayed in the property sheet
            // we do not support all properties currently, just color, width and style
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.CONNECTION_COL,
                                                     DisplayString.Prop.CONNECTION_STYLE));
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.TEXT, DisplayString.Prop.TEXTPOS));
            supportedProperties.add(DisplayString.Prop.TOOLTIP);
        }

    }

    public ChannelInterfacePropertySource(ChannelInterfaceElementEx nodeModel) {
    	super(nodeModel);
        mergePropertySource(new NamePropertySource(nodeModel, new TypeNameValidator(nodeModel)));
        mergePropertySource(new DelegatingPropertySource(
                new ExtendsListPropertySource(nodeModel),
                ExtendsListPropertySource.CATEGORY,
                ExtendsListPropertySource.DESCRIPTION));
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(nodeModel),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        // create a displayPropertySource
        mergePropertySource(new ChannelInterfaceDisplayPropertySource(nodeModel));
    }

}
