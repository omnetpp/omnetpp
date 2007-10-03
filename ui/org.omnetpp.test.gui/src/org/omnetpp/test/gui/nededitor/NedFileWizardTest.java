package org.omnetpp.test.gui.nededitor;

import com.simulcraft.test.gui.util.WorkspaceUtils;


public class NedFileWizardTest 
	extends NedFileTestCase
{
	public void testNewEmptyFile() throws Throwable {
        NedEditorUtils.createNewNedFileByWizard(projectName, fileName, null);
	    WorkspaceUtils.assertFileExistsWithContent(filePath, "");
	}
	
    public void testNewSimpleModuleFile() throws Throwable {
        NedEditorUtils.createNewNedFileByWizard(projectName, fileName, ".*Simple.*");
        WorkspaceUtils.assertFileExistsWithRegexpContent(filePath, ".*Simple.*");
    }
    
}
