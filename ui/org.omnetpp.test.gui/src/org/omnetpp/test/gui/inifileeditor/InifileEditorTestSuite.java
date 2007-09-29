package org.omnetpp.test.gui.inifileeditor;

import junit.framework.Test;
import junit.framework.TestSuite;

public class InifileEditorTestSuite
	extends TestSuite
{
	public InifileEditorTestSuite() {
		addTestSuite(DummyInifileEditorTest.class);
		addTestSuite(SectionHierarchyTest.class);
	}		 

	public static Test suite() {
        return new InifileEditorTestSuite();
    }        
}
