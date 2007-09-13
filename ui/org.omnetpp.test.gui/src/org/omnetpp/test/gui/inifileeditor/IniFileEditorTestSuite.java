package org.omnetpp.test.gui.inifileeditor;

import junit.framework.Test;
import junit.framework.TestSuite;

public class IniFileEditorTestSuite
	extends TestSuite
{
	public IniFileEditorTestSuite() {
		addTestSuite(IniFileEditorTest.class);
		addTestSuite(SectionHierarchyTest.class);
	}		 

	public static Test suite() {
        return new IniFileEditorTestSuite();
    }        
}
