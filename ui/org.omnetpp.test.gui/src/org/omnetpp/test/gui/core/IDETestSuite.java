package org.omnetpp.test.gui.core;

import junit.framework.Test;
import junit.framework.TestSuite;

import org.omnetpp.test.gui.inifileeditor.IniFileEditorTestSuite;
import org.omnetpp.test.gui.nededitor.NedEditorTestSuite;

public class IDETestSuite
	extends TestSuite
{
    public IDETestSuite() {
        addTest(new IniFileEditorTestSuite());
        addTest(new NedEditorTestSuite());
    }

    public static Test suite() {
		return new IDETestSuite();
	}		 
}
