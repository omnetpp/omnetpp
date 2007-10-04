package org.omnetpp.test.gui.nededitor.main;

import com.simulcraft.test.gui.util.WorkbenchUtils;
import com.simulcraft.test.gui.util.WorkspaceUtils;

import org.omnetpp.test.gui.nededitor.NedFileTestCase;


/**
 * Delete a NED file which is open in a NED editor. 
 * (This used to crash the Outline View / Property View.)
 * 
 * @author Andras
 */
public class DeleteOpenFileTest 
	extends NedFileTestCase
{
	public void testOpenFile() throws Throwable {
		createFileWithContent("simple Test {}");
		openFileFromProjectExplorerView();
		WorkbenchUtils.ensureViewActivated("General", "Outline");
        WorkbenchUtils.ensureViewActivated("General", "Properties");
		findNedEditor().ensureActiveGraphicalEditor();

		WorkspaceUtils.assertFileExists("/" + projectName + "/" + fileName);
		WorkspaceUtils.ensureFileNotExists(projectName, fileName);
		
		WorkbenchUtils.ensureViewActivated("General", "Outline");  // give views a chance to crash
	}
}
