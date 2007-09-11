package org.omnetpp.test.gui.inifileeditor;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.MultiPageEditorPartAccess;
import org.omnetpp.test.gui.access.StyledTextAccess;
import org.omnetpp.test.gui.access.ViewPartAccess;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;
import org.omnetpp.test.gui.util.WorkbenchUtils;
import org.omnetpp.test.gui.util.WorkspaceUtils;

public class SectionHierarchyTest
	extends IniFileTestCase 
{
	@Override
	protected void setUpInternal() throws Exception {
		super.setUpInternal();
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		WorkspaceUtils.createFileWithContent(filePath, "");
		WorkbenchUtils.findInProjectExplorerView(filePath).reveal().doubleClick();
		workbenchWindow.findEditorPartByTitle(fileName); //TODO .assertClass(InifileEditor.class)
	}
	
	public void testWrongNetwork() throws Throwable {
		WorkbenchWindowAccess workbenchWindowAccess = Access.getWorkbenchWindowAccess();
		MultiPageEditorPartAccess multiPageEditorPart = workbenchWindowAccess.findMultiPageEditorPartByTitle(fileName);
		multiPageEditorPart.activatePage("Text");
		StyledTextAccess styledText = multiPageEditorPart.findStyledText();
		styledText.assertHasFocus();
		styledText.pressKey('A', SWT.CTRL); // select all
		styledText.typeIn("Hello");

		// The "Problems" view must display a "No such NED network" error
		ViewPartAccess problemsView = workbenchWindowAccess.findViewPartByTitle("Problems", true);
		problemsView.activateWithMouseClick();
		problemsView.findTree().findTreeItemByContent("Line must be in the form.*");
	}
	
}
