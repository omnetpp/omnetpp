/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.omnetpp.ned.core.INedResources;
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
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.TEXT, DisplayString.Prop.TEXT_POS));
            supportedProperties.add(DisplayString.Prop.TOOLTIP);
        }
    }

    /**
     * Constructor
     * @param modelElement
     */
    public ChannelPropertySource(final ChannelElementEx modelElement) {
        super(modelElement);
        mergePropertySource(new NamePropertySource(modelElement, new TypeNameValidator(modelElement)));
        // extends
        mergePropertySource(new ExtendsPropertySource(modelElement, getPossibleTypeDisplayNames(modelElement, INedResources.CHANNEL_FILTER)));
        // interfaces
        mergePropertySource(new DelegatingPropertySource(
                new InterfacesListPropertySource(modelElement),
                InterfacesListPropertySource.CATEGORY,
                InterfacesListPropertySource.DESCRIPTION));
        // parameters
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(modelElement),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        // create a displayPropertySource
        mergePropertySource(new ChannelDisplayPropertySource(modelElement));
    }

}
