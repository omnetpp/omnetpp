package org.omnetpp.test.gui.nededitor;

import junit.framework.Test;
import junit.framework.TestSuite;

public class NedEditorTestSuite
	extends TestSuite
{
	public NedEditorTestSuite() {
		addTestSuite(OpenFileTest.class);
        addTestSuite(SaveFileTest.class);
        addTestSuite(NedPathTest.class);
		addTestSuite(SimpleModuleTypeTest.class);
		addTestSuite(NedOutlineViewTest.class);
		addTest(GraphicalEditorTestSuite.suite());
	}

    public static Test suite() {
        return new NedEditorTestSuite();
    }        
}
