package org.omnetpp.test.gui.core;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

import org.omnetpp.test.gui.inifileeditor.IniFileEditorTestSuite;
import org.omnetpp.test.gui.nededitor.NedEditorTestSuite;

public class IDETestSuite
	extends TestCase
{
	public static Test suite() {
		TestSuite suite = new TestSuite();
		suite.addTestSuite(IniFileEditorTestSuite.class);
		suite.addTestSuite(NedEditorTestSuite.class);
		return suite;
	}		 
}
