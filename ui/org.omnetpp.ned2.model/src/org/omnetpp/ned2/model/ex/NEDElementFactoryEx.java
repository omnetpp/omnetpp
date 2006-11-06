package org.omnetpp.ned2.model.ex;

import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.NEDElementFactory;

/**
 * @author rhornig
 * Create model elements based on string or numeric id
 */
public class NEDElementFactoryEx extends NEDElementFactory {

	public NEDElement createNodeWithTag(String tagname, NEDElement parent) {
        if (tagname.equals(NedFileNodeEx.getStaticTagName()))
            return new NedFileNodeEx(parent);
        if (tagname.equals(CompoundModuleNodeEx.getStaticTagName()))
            return new CompoundModuleNodeEx(parent);
        if (tagname.equals(SubmoduleNodeEx.getStaticTagName()))
            return new SubmoduleNodeEx(parent);
        if (tagname.equals(SimpleModuleNodeEx.getStaticTagName()))
            return new SimpleModuleNodeEx(parent);
        if (tagname.equals(ConnectionNodeEx.getStaticTagName()))
            return new ConnectionNodeEx(parent);
        if (tagname.equals(ChannelNodeEx.getStaticTagName()))
            return new ChannelNodeEx(parent);
        if (tagname.equals(ChannelInterfaceNodeEx.getStaticTagName()))
            return new ChannelInterfaceNodeEx(parent);
        if (tagname.equals(ModuleInterfaceNodeEx.getStaticTagName()))
            return new ModuleInterfaceNodeEx(parent);
        if (tagname.equals(GateNodeEx.getStaticTagName()))
            return new GateNodeEx(parent);
        if (tagname.equals(ParamNodeEx.getStaticTagName()))
            return new ParamNodeEx(parent);

        return super.createNodeWithTag(tagname, parent);
	}

	public NEDElement createNodeWithTag(int tagcode, NEDElement parent) {
        if (tagcode==NED_NED_FILE)
            return new NedFileNodeEx(parent);
        if (tagcode==NED_COMPOUND_MODULE)
            return new CompoundModuleNodeEx(parent);
        if (tagcode==NED_SUBMODULE)
            return new SubmoduleNodeEx(parent);
        if (tagcode==NED_SIMPLE_MODULE)
            return new SimpleModuleNodeEx(parent);
        if (tagcode==NED_CONNECTION)
            return new ConnectionNodeEx(parent);
        if (tagcode==NED_CHANNEL)
            return new ChannelNodeEx(parent);
        if (tagcode==NED_CHANNEL_INTERFACE)
            return new ChannelInterfaceNodeEx(parent);
        if (tagcode==NED_MODULE_INTERFACE)
            return new ModuleInterfaceNodeEx(parent);
        if (tagcode==NED_GATE)
            return new GateNodeEx(parent);
        if (tagcode==NED_PARAM)
            return new ParamNodeEx(parent);
        
		return super.createNodeWithTag(tagcode, parent);
	}
}
