package org.omnetpp.test.gui.nededitor.main;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.util.WorkspaceUtils;

import org.eclipse.swt.SWT;

import org.omnetpp.test.gui.access.NedEditorAccess;
import org.omnetpp.test.gui.nededitor.NedFileTestCase;

public class SaveFileTest 
	extends NedFileTestCase
{
    final String CONTENT = "simple Test {}";

    public void testSaveFile() throws Throwable {
	    createEmptyFile();
        openFileFromProjectExplorerView();
        typeIntoTextualNedEditor(CONTENT);
		NedEditorAccess nedEditor = findNedEditor();
		nedEditor.ensureActiveTextEditor();
		nedEditor.saveWithHotKey();
		nedEditor.closeWithHotKey();
		WorkspaceUtils.assertFileExistsWithContent(filePath, CONTENT);
	}

	public void testSaveFileAs() throws Throwable  {
		createFileWithContent(CONTENT);
		openFileFromProjectExplorerView();
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.chooseFromMainMenu("File|Save As.*");
		ShellAccess shell = WorkbenchWindowAccess.findShellByTitle(".*Save As.*");
		String newFileName = "testRenamed.ned";
		shell.findTextAfterLabel("File name:").typeIn(newFileName);
		shell.pressKey(SWT.CR);
		setFileName(newFileName);
        WorkspaceUtils.assertFileExistsWithContent(filePath, CONTENT);
		assertBothEditorsAreAccessible();
	}
}
