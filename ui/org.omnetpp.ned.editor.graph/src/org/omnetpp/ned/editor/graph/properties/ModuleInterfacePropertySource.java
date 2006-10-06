package org.omnetpp.ned.editor.graph.properties;

import org.omnetpp.ned2.model.ModuleInterfaceNodeEx;

public class ModuleInterfacePropertySource extends DelegatingPropertySource {

    public ModuleInterfacePropertySource(ModuleInterfaceNodeEx channelNodeModel) {
    	super(channelNodeModel);
        addPropertySource(new NamePropertySource(channelNodeModel));
    }

}
