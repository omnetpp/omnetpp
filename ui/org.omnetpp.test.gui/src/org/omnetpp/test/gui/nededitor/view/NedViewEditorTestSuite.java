package org.omnetpp.test.gui.nededitor.view;


import junit.framework.Test;
import junit.framework.TestSuite;

public class NedViewEditorTestSuite
	extends TestSuite
{
	public NedViewEditorTestSuite() {
        addTestSuite(NedOutlineViewTest.class);
	}

    public static Test suite() {
        return new NedViewEditorTestSuite();
    }        
}
