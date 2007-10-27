package org.omnetpp.test.gui.scave;

import junit.framework.Test;
import junit.framework.TestSuite;

public class ScaveTestSuite
    extends TestSuite
{
    public ScaveTestSuite() {
        addTestSuite(OpenFileTest.class);
        addTestSuite(InputsPageTest.class);
        addTestSuite(BrowseDataPageTest.class);
        addTestSuite(CreateDatasetsAndChartsTest.class);
        addTestSuite(DatasetsAndChartsPageTest.class);
        addTestSuite(DatasetViewTest.class);
        addTestSuite(ExportTest.class);
    }

    public static Test suite() {
        return new ScaveTestSuite();
    }        
}
