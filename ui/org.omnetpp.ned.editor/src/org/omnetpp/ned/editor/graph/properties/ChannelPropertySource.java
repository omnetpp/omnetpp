/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties;

import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumSet;
import java.util.List;

import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
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
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;

/*
 * @author rhornig
 */
public class ChannelPropertySource extends MergedPropertySource {

    protected static class ChannelDisplayPropertySource extends DisplayPropertySource {
        protected ChannelElementEx model;

        public ChannelDisplayPropertySource(ChannelElementEx model) {
            super(model);
            this.model = model;
            // define which properties should be displayed in the property sheet
            // we do not support all properties currently, just color, width and style
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.ROUTING_CONSTRAINT,
                    								 DisplayString.Prop.CONNECTION_STYLE));
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.TEXT, DisplayString.Prop.TEXTPOS));
            supportedProperties.add(DisplayString.Prop.TOOLTIP);
        }
    }

    /**
     * Constructor
     * @param channelElement
     */
    public ChannelPropertySource(final ChannelElementEx channelElement) {
    	super(channelElement);
        mergePropertySource(new NamePropertySource(channelElement, new TypeNameValidator(channelElement)));
        // extends
        mergePropertySource(new ExtendsPropertySource(channelElement) {
            @Override
            protected List<String> getPossibleValues() {
                INedResources res = NedResourcesPlugin.getNedResources();
                INedTypeLookupContext context = channelElement.getEnclosingLookupContext();
                List<String> channelNames = new ArrayList<String>(res.getVisibleTypeNames(context, INedResources.CHANNEL_FILTER));
                // remove ourselves to avoid direct cycle
                channelNames.remove(channelElement.getName());
                // add type names that need fully qualified names
                channelNames.addAll(res.getInvisibleTypeNames(context, INedResources.CHANNEL_FILTER));
                Collections.sort(channelNames);
                return channelNames;
            }
        });
        // interfaces
        mergePropertySource(new DelegatingPropertySource(
                new InterfacesListPropertySource(channelElement),
                InterfacesListPropertySource.CATEGORY,
                InterfacesListPropertySource.DESCRIPTION));
        // parameters
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(channelElement),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        // create a displayPropertySource
        mergePropertySource(new ChannelDisplayPropertySource(channelElement));
    }

}
