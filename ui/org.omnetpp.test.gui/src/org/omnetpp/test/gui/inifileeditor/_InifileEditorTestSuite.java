package org.omnetpp.test.gui.inifileeditor;

import junit.framework.Test;
import junit.framework.TestSuite;

public class _InifileEditorTestSuite
	extends TestSuite
{
	public _InifileEditorTestSuite() {
		addTestSuite(_InifileEditorTest.class);
		addTestSuite(SectionHierarchyTest.class);
	}		 

	public static Test suite() {
        return new _InifileEditorTestSuite();
    }        
}
