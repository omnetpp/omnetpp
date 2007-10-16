package org.omnetpp.test.gui.inifileeditor;

import junit.framework.Test;
import junit.framework.TestSuite;

import org.omnetpp.test.gui.inifileeditor.parameterspage.TreeCellEditorTest;
import org.omnetpp.test.gui.inifileeditor.parameterspage.DeleteParameterTest;
import org.omnetpp.test.gui.inifileeditor.parameterspage.EditParameterTest;
import org.omnetpp.test.gui.inifileeditor.parameterspage.NewParameterTest;
import org.omnetpp.test.gui.inifileeditor.parameterspage.ParameterDragDropTest;
import org.omnetpp.test.gui.inifileeditor.parameterspage.ParametersPageTest;
import org.omnetpp.test.gui.inifileeditor.sectionspage.SectionDeleteTest;
import org.omnetpp.test.gui.inifileeditor.sectionspage.SectionDialogTest;
import org.omnetpp.test.gui.inifileeditor.sectionspage.SectionDragDropTest;
import org.omnetpp.test.gui.inifileeditor.sectionspage.SectionsPageTest;
import org.omnetpp.test.gui.inifileeditor.views.InifileOutlineViewTest;
import org.omnetpp.test.gui.inifileeditor.views.ModuleHierarchyViewTest;
import org.omnetpp.test.gui.inifileeditor.views.ParametersViewTest;

public class InifileEditorTestSuite
	extends TestSuite
{
	public InifileEditorTestSuite() {
	    addTestSuite(ConfigTest.class);
	    addTestSuite(IncludedInifilesTest.class);
	    addTestSuite(InifileEditorTestCase.class);
	    addTestSuite(InifileEditorTestSuite.class);
	    addTestSuite(InifileEditorUtils.class);
	    addTestSuite(NewInifileWizardTest.class);
	    addTestSuite(ParametersTest.class);
	    addTestSuite(PerObjectConfigTest.class);
	    addTestSuite(TreeCellEditorTest.class);
	    addTestSuite(DeleteParameterTest.class);
	    addTestSuite(EditParameterTest.class);
	    addTestSuite(NewParameterTest.class);
	    addTestSuite(ParameterDragDropTest.class);
	    addTestSuite(ParametersPageTest.class);
	    addTestSuite(SectionDeleteTest.class);
	    addTestSuite(SectionDialogTest.class);
	    addTestSuite(SectionDragDropTest.class);
	    addTestSuite(SectionsPageTest.class);
	    addTestSuite(InifileOutlineViewTest.class);
	    addTestSuite(ModuleHierarchyViewTest.class);
	    addTestSuite(ParametersViewTest.class);
	}		 

	public static Test suite() {
        return new InifileEditorTestSuite();
    }        
}
