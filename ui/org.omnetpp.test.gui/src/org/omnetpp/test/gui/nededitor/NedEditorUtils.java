package org.omnetpp.test.gui.nededitor;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.MultiPageEditorPartAccess;
import org.omnetpp.test.gui.access.ShellAccess;
import org.omnetpp.test.gui.access.StyledTextAccess;
import org.omnetpp.test.gui.access.TextEditorAccess;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;
import org.omnetpp.test.gui.util.WorkbenchUtils;
import org.omnetpp.test.gui.util.WorkspaceUtils;

public class NedEditorUtils
{
	public static void createNewNedFileByWizard(String parentFolder, String fileName) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		WorkbenchUtils.choosePerspectiveFromDialog(".*OMN.*"); // so that we have "New|NED file" in the menu
		workbenchWindow.chooseFromMainMenu("File|New.*|Network Description.*");
		ShellAccess shell = Access.findShellByTitle("New NED File");
		shell.findTextAfterLabel(".*parent folder.*").clickAndType(parentFolder);
		shell.findTextAfterLabel("File name.*").clickAndType(fileName);
		shell.findButtonWithLabel("Finish").activateWithMouseClick();
		WorkspaceUtils.assertFileExists(parentFolder + "/" + fileName); // make sure file got created
	}

	public static void typeIntoTextualNedEditor(String fileName, String nedSource) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		MultiPageEditorPartAccess multiPageEditorPart = workbenchWindow.findMultiPageEditorPartByTitle(fileName);
		TextEditorAccess textualEditor = (TextEditorAccess)multiPageEditorPart.activatePage("Text");
		StyledTextAccess styledText = textualEditor.findStyledText();
		styledText.pressKey('A', SWT.CTRL); // "Select all"
		styledText.typeIn(nedSource);
	}

	public static void assertNoErrorInNedSource(String fileName, String nedSource) {
		NedEditorUtils.typeIntoTextualNedEditor(fileName, nedSource);
		WorkbenchUtils.assertNoErrorMessageInProblemsView();
	}

	public static void assertErrorInNedSource(String fileName, String nedSource, String errorText) {
		NedEditorUtils.typeIntoTextualNedEditor(fileName, nedSource);
		WorkbenchUtils.assertErrorMessageInProblemsView(errorText);
		//TODO: do something in the graphical editor: check error markers are there, etc
	}
}
