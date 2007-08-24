package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.pojo.NEDElementFactory;

/**
 * Create model elements based on string or numeric tag code
 *
 * @author rhornig
 */
public class NEDElementFactoryEx extends NEDElementFactory {

	@Override
	public INEDElement createElement(String tagname, INEDElement parent) {
        if (tagname.equals(NedFileElementEx.getStaticTagName()))
            return new NedFileElementEx(parent);
        if (tagname.equals(CompoundModuleElementEx.getStaticTagName()))
            return new CompoundModuleElementEx(parent);
        if (tagname.equals(SubmoduleElementEx.getStaticTagName()))
            return new SubmoduleElementEx(parent);
        if (tagname.equals(SimpleModuleElementEx.getStaticTagName()))
            return new SimpleModuleElementEx(parent);
        if (tagname.equals(ConnectionElementEx.getStaticTagName()))
            return new ConnectionElementEx(parent);
        if (tagname.equals(ChannelElementEx.getStaticTagName()))
            return new ChannelElementEx(parent);
        if (tagname.equals(ChannelInterfaceElementEx.getStaticTagName()))
            return new ChannelInterfaceElementEx(parent);
        if (tagname.equals(ModuleInterfaceElementEx.getStaticTagName()))
            return new ModuleInterfaceElementEx(parent);
        if (tagname.equals(GateElementEx.getStaticTagName()))
            return new GateElementEx(parent);
        if (tagname.equals(ParamElementEx.getStaticTagName()))
            return new ParamElementEx(parent);
        if (tagname.equals(PropertyElementEx.getStaticTagName()))
            return new PropertyElementEx(parent);

        return super.createElement(tagname, parent);
	}

	@Override
	public INEDElement createElement(int tagcode, INEDElement parent) {
        if (tagcode==NED_NED_FILE)
            return new NedFileElementEx(parent);
        if (tagcode==NED_COMPOUND_MODULE)
            return new CompoundModuleElementEx(parent);
        if (tagcode==NED_SUBMODULE)
            return new SubmoduleElementEx(parent);
        if (tagcode==NED_SIMPLE_MODULE)
            return new SimpleModuleElementEx(parent);
        if (tagcode==NED_CONNECTION)
            return new ConnectionElementEx(parent);
        if (tagcode==NED_CHANNEL)
            return new ChannelElementEx(parent);
        if (tagcode==NED_CHANNEL_INTERFACE)
            return new ChannelInterfaceElementEx(parent);
        if (tagcode==NED_MODULE_INTERFACE)
            return new ModuleInterfaceElementEx(parent);
        if (tagcode==NED_GATE)
            return new GateElementEx(parent);
        if (tagcode==NED_PARAM)
            return new ParamElementEx(parent);
        if (tagcode==NED_PROPERTY)
            return new PropertyElementEx(parent);

		return super.createElement(tagcode, parent);
	}
}
