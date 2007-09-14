package org.omnetpp.test.gui.nededitor;

import junit.framework.Test;
import junit.framework.TestSuite;

public class GraphicalEditorTestSuite
	extends TestSuite
{
	public GraphicalEditorTestSuite() {
        addTestSuite(CreateWithGraphicalEditorTest.class);
		addTestSuite(DeleteWithGraphicalEditorTest.class);
        addTestSuite(CreateComplexModelWithGraphicalEditorTest.class);
	}

    public static Test suite() {
        return new GraphicalEditorTestSuite();
    }        
}
