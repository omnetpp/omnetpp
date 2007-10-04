package org.omnetpp.test.gui.nededitor.textual;


import junit.framework.Test;
import junit.framework.TestSuite;

public class TextuallEditorTestSuite
	extends TestSuite
{
	public TextuallEditorTestSuite() {
        addTestSuite(SimpleModuleTypeTest.class);
	}

    public static Test suite() {
        return new TextuallEditorTestSuite();
    }        
}
