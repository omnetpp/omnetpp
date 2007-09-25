package org.omnetpp.test.gui.inifileeditor;

import org.eclipse.swt.SWT;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.StyledTextAccess;
import com.simulcraft.test.gui.access.ViewPartAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.util.WorkbenchUtils;
import com.simulcraft.test.gui.util.WorkspaceUtils;

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
