package org.omnetpp.test.gui.nededitor;

import org.omnetpp.test.gui.inifileeditor.IniFileEditorTestSuite;

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
		addTestSuite(GraphicalEditorTest.class);
	}		 

    public static Test suite() {
        return new NedEditorTestSuite();
    }        
}
