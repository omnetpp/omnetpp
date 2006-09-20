package org.omnetpp.ned2.model;

import org.omnetpp.ned2.model.pojo.NEDElementFactory;
import org.omnetpp.ned2.model.pojo.SimpleModuleNode;

public class NEDElementFactoryEx extends NEDElementFactory {

	public NEDElement createNodeWithTag(String tagname, NEDElement parent) {
        if (tagname.equals("ned-file"))
            return new NedFileNodeEx(parent);
        if (tagname.equals("compound-module"))
            return new CompoundModuleNodeEx(parent);
        if (tagname.equals("submodule"))
            return new SubmoduleNodeEx(parent);
        if (tagname.equals("simple-module"))
            return new SimpleModuleNodeEx(parent);
        if (tagname.equals("connection"))
            return new ConnectionNodeEx(parent);
        //if (tagname.equals("gate"))
        //    return new GateNodeEx(parent);

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
        //if (tagcode==NED_GATE)
        //    return new GateNodeEx(parent);
        
		return super.createNodeWithTag(tagcode, parent);
	}
}
