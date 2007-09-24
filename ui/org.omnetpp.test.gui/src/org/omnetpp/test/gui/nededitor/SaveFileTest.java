package org.omnetpp.test.gui.nededitor;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.MultiPageEditorPartAccess;
import org.omnetpp.test.gui.access.ShellAccess;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;

public class SaveFileTest 
	extends NedFileTestCase
{
	public void testSaveFile() throws Throwable {
		createNewNedFileByWizard();
		typeIntoTextualNedEditor("simple Test {}");
		MultiPageEditorPartAccess multiPageEditorPart = findMultiPageEditor();
		multiPageEditorPart.activatePage("Text");
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
