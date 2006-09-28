package org.omnetpp.ned2.model;

import org.omnetpp.ned2.model.pojo.NEDElementFactory;

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
        
		return super.createNodeWithTag(tagcode, parent);
	}
}
