/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.pojo.NedElementFactory;

/**
 * Create model elements based on string or numeric tag code
 *
 * @author rhornig, andras
 */
public class NedElementFactoryEx extends NedElementFactory {

    private static NedElementFactoryEx instance;

    public static NedElementFactoryEx getInstance() {
        return instance;
    }

    public static void setInstance(NedElementFactoryEx inst) {
        instance = inst;
    }

    /**
     * Note: this method cannot be used with element types that need a NED resolver instance
     * (those that implement IHasResolver); use the 3-arg method for those element types.
     */
    @Override
    public INedElement createElement(String tagname, INedElement parent) {
        return createElement(null, tagname, parent);
    }

    /**
     * Note: this method cannot be used with element types that need a NED resolver instance
     * (those that implement IHasResolver); use the 3-arg method for those element types.
     */
    @Override
    public INedElement createElement(int tagcode, INedElement parent) {
        return createElement(null, tagcode, parent);
    }

    public INedElement createElement(INedTypeResolver resolver, String tagname) {
        return createElement(resolver, tagname, null);
    }

    public INedElement createElement(INedTypeResolver resolver, String tagname, INedElement parent) {
        // NED elements that need a resolver
        if (tagname.equals(NedFileElementEx.getStaticTagName()))
            return new NedFileElementEx(resolver, parent);
        if (tagname.equals(CompoundModuleElementEx.getStaticTagName()))
            return new CompoundModuleElementEx(resolver, parent);
        if (tagname.equals(SubmoduleElementEx.getStaticTagName()))
            return new SubmoduleElementEx(resolver, parent);
        if (tagname.equals(SimpleModuleElementEx.getStaticTagName()))
            return new SimpleModuleElementEx(resolver, parent);
        if (tagname.equals(ConnectionElementEx.getStaticTagName()))
            return new ConnectionElementEx(resolver, parent);
        if (tagname.equals(ChannelElementEx.getStaticTagName()))
            return new ChannelElementEx(resolver, parent);
        if (tagname.equals(ChannelInterfaceElementEx.getStaticTagName()))
            return new ChannelInterfaceElementEx(resolver, parent);
        if (tagname.equals(ModuleInterfaceElementEx.getStaticTagName()))
            return new ModuleInterfaceElementEx(resolver, parent);

        // other NED elements
        if (tagname.equals(GateElementEx.getStaticTagName()))
            return new GateElementEx(parent);
        if (tagname.equals(ParamElementEx.getStaticTagName()))
            return new ParamElementEx(parent);
        if (tagname.equals(PropertyElementEx.getStaticTagName()))
            return new PropertyElementEx(parent);

        // MSG elements
        if (tagname.equals(MsgFileElementEx.getStaticTagName()))
            return new MsgFileElementEx(parent);
        if (tagname.equals(MessageElementEx.getStaticTagName()))
            return new MessageElementEx(parent);
        if (tagname.equals(PacketElementEx.getStaticTagName()))
            return new PacketElementEx(parent);
        if (tagname.equals(ClassElementEx.getStaticTagName()))
            return new ClassElementEx(parent);
        if (tagname.equals(StructElementEx.getStaticTagName()))
            return new StructElementEx(parent);
        if (tagname.equals(EnumElementEx.getStaticTagName()))
            return new EnumElementEx(parent);

        return super.createElement(tagname, parent);
    }

    public INedElement createElement(INedTypeResolver resolver, int tagcode) {
        return createElement(resolver, tagcode, null);
    }

    public INedElement createElement(INedTypeResolver resolver, int tagcode, INedElement parent) {
        // NED elements that need a resolver
        if (tagcode==NED_NED_FILE)
            return new NedFileElementEx(resolver, parent);
        if (tagcode==NED_COMPOUND_MODULE)
            return new CompoundModuleElementEx(resolver, parent);
        if (tagcode==NED_SUBMODULE)
            return new SubmoduleElementEx(resolver, parent);
        if (tagcode==NED_SIMPLE_MODULE)
            return new SimpleModuleElementEx(resolver, parent);
        if (tagcode==NED_CONNECTION)
            return new ConnectionElementEx(resolver, parent);
        if (tagcode==NED_CHANNEL)
            return new ChannelElementEx(resolver, parent);
        if (tagcode==NED_CHANNEL_INTERFACE)
            return new ChannelInterfaceElementEx(resolver, parent);
        if (tagcode==NED_MODULE_INTERFACE)
            return new ModuleInterfaceElementEx(resolver, parent);

        // other NED elements
        if (tagcode==NED_GATE)
            return new GateElementEx(parent);
        if (tagcode==NED_PARAM)
            return new ParamElementEx(parent);
        if (tagcode==NED_PROPERTY)
            return new PropertyElementEx(parent);

        // MSG elements
        if (tagcode==NED_MSG_FILE)
            return new MsgFileElementEx(parent);
        if (tagcode==NED_MESSAGE)
            return new MessageElementEx(parent);
        if (tagcode==NED_PACKET)
            return new PacketElementEx(parent);
        if (tagcode==NED_CLASS)
            return new ClassElementEx(parent);
        if (tagcode==NED_STRUCT)
            return new StructElementEx(parent);
        if (tagcode==NED_ENUM)
            return new EnumElementEx(parent);

        return super.createElement(tagcode, parent);
    }
}
