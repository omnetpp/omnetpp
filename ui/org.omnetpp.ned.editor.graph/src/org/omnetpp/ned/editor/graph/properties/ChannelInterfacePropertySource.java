package org.omnetpp.ned.editor.graph.properties;

import org.omnetpp.ned2.model.ChannelInterfaceNodeEx;

public class ChannelInterfacePropertySource extends MergedPropertySource {


    public ChannelInterfacePropertySource(ChannelInterfaceNodeEx nodeModel) {
    	super(nodeModel);
        mergePropertySource(new NamePropertySource(nodeModel));
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(nodeModel),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
    }

}
