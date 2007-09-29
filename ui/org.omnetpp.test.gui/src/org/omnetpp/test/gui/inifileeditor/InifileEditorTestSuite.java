package org.omnetpp.test.gui.inifileeditor;

import junit.framework.Test;
import junit.framework.TestSuite;

public class InifileEditorTestSuite
	extends TestSuite
{
	public InifileEditorTestSuite() {
	    // wizard
	    addTestSuite(NewInifileWizardTest.class);

	    // editor
	    addTestSuite(SectionsTest.class);
	    addTestSuite(ConfigTest.class);
	    addTestSuite(PerObjectConfigTest.class);
	    addTestSuite(ParametersTest.class);
	    addTestSuite(IncludedInifilesTest.class);
	    
	    // views
	    addTestSuite(InifileOutlineViewTest.class);
	    addTestSuite(ModuleHierarchyViewTest.class);
	    addTestSuite(ParametersViewTest.class);
	}		 

	public static Test suite() {
        return new InifileEditorTestSuite();
    }        
}
