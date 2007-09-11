package org.omnetpp.test.gui.inifileeditor;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

public class IniFileEditorTestSuite
	extends TestCase
{
	public static Test suite() {
		TestSuite suite = new TestSuite();
		suite.addTestSuite(IniFileEditorTest.class);
		suite.addTestSuite(SectionHierarchyTest.class);
		return suite;
	}		 
}
