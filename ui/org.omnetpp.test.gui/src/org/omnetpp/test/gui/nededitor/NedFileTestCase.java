package org.omnetpp.test.gui.nededitor;

import org.omnetpp.test.gui.access.NedEditorAccess;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.core.ProjectFileTestCase;


public class NedFileTestCase
	extends ProjectFileTestCase
{
	public NedFileTestCase() {
		super("test.ned");
	}

	public void assertBothEditorsAreAccessible() {
		NedEditorUtils.assertBothEditorsAreAccessible(fileName);
	}

	protected NedEditorAccess findNedEditor() {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		return (NedEditorAccess)workbenchWindow.findMultiPageEditorPartByTitle(fileName);
	}

	protected void createNewNedFileByWizard() {
		NedEditorUtils.createNewNedFileByWizard(projectName, fileName);
	}

	protected void typeIntoTextualNedEditor(String nedSource) {
		NedEditorUtils.typeIntoTextualNedEditor(fileName, nedSource);
	}
}
