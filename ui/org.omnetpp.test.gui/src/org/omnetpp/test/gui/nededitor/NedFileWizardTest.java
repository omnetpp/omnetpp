package org.omnetpp.test.gui.nededitor;

import com.simulcraft.test.gui.util.WorkspaceUtils;

import org.omnetpp.common.util.StringUtils;


public class NedFileWizardTest 
	extends NedFileTestCase
{
	public void testNewEmptyFile() throws Throwable {
        NedEditorUtils.createNewNedFileByWizard(projectName, fileName, null);
	    WorkspaceUtils.assertFileExistsWithContentIgnoringWhiteSpace(filePath, "//\n// TODO Place comment here\n//\n\n");
	}
	
    public void testNewSimpleModuleFile() throws Throwable {
        NedEditorUtils.createNewNedFileByWizard(projectName, fileName, ".*new Simple.*");
        String moduleName = StringUtils.substringBefore(fileName, ".");
        WorkspaceUtils.assertFileExistsWithContentIgnoringWhiteSpace(filePath, "//\n// TODO Place comment here\n//\n\nsimple "+moduleName+" {\n  parameters:\n  gates:\n}\n");
    }
    
    public void testNewCompoundModuleFile() throws Throwable {
        NedEditorUtils.createNewNedFileByWizard(projectName, fileName, ".*new Compound.*");
        String moduleName = StringUtils.substringBefore(fileName, ".");
        WorkspaceUtils.assertFileExistsWithContentIgnoringWhiteSpace(filePath, "//\n// TODO Place comment here\n//\n\nmodule "+moduleName+" {\n  parameters:\n  gates:\n  submodules:\n  connections:\n}\n");
    }
    
    public void testNewNetworkFile() throws Throwable {
        NedEditorUtils.createNewNedFileByWizard(projectName, fileName, ".*toplevel Network.*");
        String moduleName = StringUtils.substringBefore(fileName, ".");
        WorkspaceUtils.assertFileExistsWithContentIgnoringWhiteSpace(filePath, "//\n// TODO Place comment here\n//\n\nnetwork "+moduleName+" {\n  parameters:\n  submodules:\n  connections:\n}\n");
    }
    
}
