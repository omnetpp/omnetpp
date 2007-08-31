package org.omnetpp.ned.editor.graph.properties;

import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumSet;
import java.util.List;

import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.properties.util.DelegatingPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.DisplayPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ExtendsPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.InterfacesListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.NamePropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ParameterListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.TypeNameValidator;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.ChannelElementEx;

/**
 * Property source for channels
 *
 * @author rhornig
 */
public class ChannelPropertySource extends MergedPropertySource {

    protected static class ChannelDisplayPropertySource extends DisplayPropertySource {
        protected ChannelElementEx model;

        public ChannelDisplayPropertySource(ChannelElementEx model) {
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

    }

    /**
     * Constructor
     * @param nodeModel
     */
    public ChannelPropertySource(ChannelElementEx nodeModel) {
    	super(nodeModel);
        mergePropertySource(new NamePropertySource(nodeModel, new TypeNameValidator(nodeModel)));
        // extends
        mergePropertySource(new ExtendsPropertySource(nodeModel) {
            @Override
            protected List<String> getPossibleValues() {
              List<String> names = new ArrayList<String>(NEDResourcesPlugin.getNEDResources().getChannelQNames());
              Collections.sort(names);
              return names;
            }
        });
        // interfaces
        mergePropertySource(new DelegatingPropertySource(
                new InterfacesListPropertySource(nodeModel),
                InterfacesListPropertySource.CATEGORY,
                InterfacesListPropertySource.DESCRIPTION));
        // parameters
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(nodeModel),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        // create a displayPropertySource
        mergePropertySource(new ChannelDisplayPropertySource(nodeModel));
    }

}
