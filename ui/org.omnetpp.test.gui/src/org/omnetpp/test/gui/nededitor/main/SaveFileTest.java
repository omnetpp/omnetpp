package org.omnetpp.test.gui.nededitor.main;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;

import org.eclipse.swt.SWT;

import org.omnetpp.test.gui.access.NedEditorAccess;
import org.omnetpp.test.gui.nededitor.NedFileTestCase;

public class SaveFileTest 
	extends NedFileTestCase
{
	public void testSaveFile() throws Throwable {
	    createFileWithContent("");
		typeIntoTextualNedEditor("simple Test {}");
		NedEditorAccess nedEditor = findNedEditor();
		nedEditor.ensureActiveTextEditor();
		nedEditor.saveWithHotKey();
		nedEditor.closeWithHotKey();
		assertFileExists();
	}

	public void testSaveFileAs() throws Throwable  {
		createFileWithContent("simple Test {}");
		openFileFromProjectExplorerView();
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.chooseFromMainMenu("File|Save As.*");
		ShellAccess shell = WorkbenchWindowAccess.findShellByTitle(".*Save As.*");
		String newFileName = "Renamed.ned";
		shell.findTextAfterLabel("File name:").typeIn(newFileName);
		shell.pressKey(SWT.CR);
		setFileName(newFileName);
		assertFileExists();
		assertBothEditorsAreAccessible();
	}
}
