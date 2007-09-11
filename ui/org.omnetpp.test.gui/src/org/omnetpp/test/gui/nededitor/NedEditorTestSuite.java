package org.omnetpp.test.gui.nededitor;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

public class NedEditorTestSuite
	extends TestCase
{
	public static Test suite() {
		TestSuite suite = new TestSuite();
		suite.addTestSuite(OpenFileTest.class);
		suite.addTestSuite(SimpleModuleTypeTest.class);
		suite.addTestSuite(GraphicalEditorTest.class);
		return suite;
	}		 
}
