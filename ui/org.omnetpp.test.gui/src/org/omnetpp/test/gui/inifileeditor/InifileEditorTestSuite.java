package org.omnetpp.test.gui.inifileeditor;

import org.omnetpp.test.gui.inifileeditor.sectionspage.SectionsPageTest;
import org.omnetpp.test.gui.inifileeditor.views.InifileOutlineViewTest;
import org.omnetpp.test.gui.inifileeditor.views.ModuleHierarchyViewTest;
import org.omnetpp.test.gui.inifileeditor.views.ParametersViewTest;

import junit.framework.Test;
import junit.framework.TestSuite;

public class InifileEditorTestSuite
	extends TestSuite
{
	public InifileEditorTestSuite() {
	    // wizard
	    addTestSuite(NewInifileWizardTest.class);

	    // editor
	    addTestSuite(SectionsPageTest.class);
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
