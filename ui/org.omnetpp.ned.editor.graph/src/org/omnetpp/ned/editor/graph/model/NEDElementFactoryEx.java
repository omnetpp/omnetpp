package org.omnetpp.ned.editor.graph.model;

import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.NEDElementFactory;

public class NEDElementFactoryEx extends NEDElementFactory {
	
	public NEDElement createNodeWithTag(String tagname) {
        if (tagname.equals("ned-file"))
            return new NedFileNodeEx();
        if (tagname.equals("compound-module"))
            return new CompoundModuleNodeEx();
        if (tagname.equals("submodule"))
            return new SubmoduleNodeEx();
        if (tagname.equals("connection"))
            return new ConnectionNodeEx();

        return super.createNodeWithTag(tagname);
	}

	public NEDElement createNodeWithTag(int tagcode) {
        if (tagcode==NED_NED_FILE)
            return new NedFileNodeEx();
        if (tagcode==NED_COMPOUND_MODULE)
            return new CompoundModuleNodeEx();
        if (tagcode==NED_SUBMODULE)
            return new SubmoduleNodeEx();
        if (tagcode==NED_CONNECTION)
            return new ConnectionNodeEx();
		return super.createNodeWithTag(tagcode);
	}
}
