package org.omnetpp.test.gui.util;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.ShellAccess;
import org.omnetpp.test.gui.access.TreeAccess;
import org.omnetpp.test.gui.access.TreeItemAccess;
import org.omnetpp.test.gui.access.ViewPartAccess;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;

public class WorkbenchUtils
{
	public static void choosePerspectiveFromDialog(String perspectiveLabel) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.chooseFromMainMenu("Window|Open Perspective|Other.*");
		ShellAccess dialog = Access.findShellByTitle("Open Perspective");
		dialog.findTable().findTableItemByContent(perspectiveLabel).reveal().doubleClick();
	}

	public static ViewPartAccess chooseViewFromDialog(String viewCategory, String viewLabel) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.chooseFromMainMenu("Window|Show View|Other.*");
		TreeAccess tree = Access.findShellByTitle("Show View").findTree();
		tree.findTreeItemByContent(viewCategory).reveal().click();
		tree.pressKey(SWT.ARROW_RIGHT); // open category node
		tree.findTreeItemByContent(viewLabel).reveal().doubleClick();
		//return workbenchWindow.findViewPartByTitle(viewLabel, true);
		return (ViewPartAccess) workbenchWindow.getActivePart();
	}

    public static ViewPartAccess ensureViewActivated(String viewCategory, String viewLabel) {
        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
        if (workbenchWindow.collectViewPartsByTitle(viewLabel, false).size() == 0)
            return chooseViewFromDialog(viewCategory, viewLabel);
        else {
            workbenchWindow.findViewPartByTitle(viewLabel).activateWithMouseClick();
            return (ViewPartAccess) workbenchWindow.getActivePart();
        }
    }

	public static TreeItemAccess findInProjectExplorerView(String path) {
		//ViewPartAccess projectExplorerView = workbenchWindow.findViewPartByTitle("Project Explorer", true);
		ViewPartAccess projectExplorerView = ensureViewActivated("General", "Project Explorer");
		TreeAccess tree = projectExplorerView.findTree();
		tree.assertHasFocus();
		return tree.findTreeItemByPath(path);
	}
	
	public static ShellAccess openProjectPropertiesFromProjectExplorerView(String projectName) {
        findInProjectExplorerView(projectName).activateContextMenuWithMouseClick().findMenuItemByLabel(".*Properties.*").activateWithMouseClick();
        return WorkbenchWindowAccess.findShellByTitle("Properties.*" + projectName + ".*");
	}

	public static void assertNoErrorMessageInProblemsView() {
		ViewPartAccess problemsView = Access.getWorkbenchWindowAccess().findViewPartByTitle("Problems", true);
		problemsView.activateWithMouseClick();
		// TODO: how do we know that validation has already taken place?
		problemsView.findTree().assertEmpty();
	}

	public static void assertErrorMessageInProblemsView(String errorText) {
		ViewPartAccess problemsView = Access.getWorkbenchWindowAccess().findViewPartByTitle("Problems", true);
		problemsView.activateWithMouseClick();
		problemsView.findTree().findTreeItemByContent(errorText);
	}
}
