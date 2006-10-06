package org.omnetpp.ned.editor.graph.properties;

import org.omnetpp.ned2.model.ChannelInterfaceNodeEx;

public class ChannelInterfacePropertySource extends DelegatingPropertySource {

    public ChannelInterfacePropertySource(ChannelInterfaceNodeEx channelNodeModel) {
    	super(channelNodeModel);
        addPropertySource(new NamePropertySource(channelNodeModel));
    }

}
