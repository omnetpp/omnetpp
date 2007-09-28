package org.omnetpp.test.gui.nededitor;

import org.eclipse.swt.SWT;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;

public class SaveFileTest 
	extends NedFileTestCase
{
	public void testSaveFile() throws Throwable {
		createNewNedFileByWizard();
		typeIntoTextualNedEditor("simple Test {}");
		MultiPageEditorPartAccess multiPageEditorPart = findMultiPageEditor();
		multiPageEditorPart.ensureActiveEditor("Text");
		multiPageEditorPart.saveWithHotKey();
		multiPageEditorPart.closeWithHotKey();
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
