package org.omnetpp.test.gui.nededitor;

import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Tree;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.InstanceofPredicate;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.StyledTextAccess;
import com.simulcraft.test.gui.access.TextEditorAccess;
import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.util.WorkbenchUtils;
import com.simulcraft.test.gui.util.WorkspaceUtils;

public class NedEditorUtils
{
	public static void createNewNedFileByWizard(String parentFolder, String fileName) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		WorkbenchUtils.ensurePerspectiveActivated(".*OMN.*"); // so that we have "New|NED file" in the menu
		workbenchWindow.chooseFromMainMenu("File|New.*|Network Description.*");
		ShellAccess shell = Access.findShellByTitle("New NED File");
		shell.findTextAfterLabel(".*parent folder.*").clickAndTypeOver(parentFolder);
		shell.findTextAfterLabel("File name.*").clickAndTypeOver(fileName);
		shell.findButtonWithLabel("Finish").activateWithMouseClick();
		WorkspaceUtils.assertFileExists(parentFolder + "/" + fileName); // make sure file got created
	}

	public static void typeIntoTextualNedEditor(String fileName, String nedSource) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		MultiPageEditorPartAccess multiPageEditorPart = workbenchWindow.findMultiPageEditorPartByTitle(fileName);
		TextEditorAccess textualEditor = (TextEditorAccess)multiPageEditorPart.ensureActiveEditor("Text");
		StyledTextAccess styledText = textualEditor.findStyledText();
		styledText.pressKey('A', SWT.CTRL); // "Select all"
		styledText.typeIn(nedSource);
	}

    public static void setDefaultNedPath(String projectName) {
        setNedPath(projectName, null);
    }
    
    public static void setNedPath(String projectName, String path) {
        ShellAccess shell = WorkbenchUtils.openProjectPropertiesFromProjectExplorerView(projectName);
        List<Control> trees = shell.collectDescendantControls(new InstanceofPredicate(Tree.class));
        final TreeAccess panelSelectorTree = new TreeAccess((Tree)Access.findControlMaximizing(trees, new Access.IValue() {
            public double valueOf(Object object) {
                return -((Control)object).getBounds().x;
            }
        }));
        panelSelectorTree.findTreeItemByContent("NED Source Folders").click();
        TreeAccess nedPathTree = new TreeAccess((Tree)shell.findDescendantControl(new IPredicate() {
            public boolean matches(Object object) {
                return object instanceof Tree && object != panelSelectorTree.getWidget();
            }
        }));
        nedPathTree.findTreeItemByPath(path == null ? projectName : projectName + "/" + path).ensureChecked();
        shell.findButtonWithLabel("OK").click();
    }

	public static void assertBothEditorsAreAccessible(String fileName) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		MultiPageEditorPartAccess multiPageEditorPart = workbenchWindow.findMultiPageEditorPartByTitle(fileName);
		multiPageEditorPart.ensureActiveEditor("Graphical");
		multiPageEditorPart.ensureActiveEditor("Text");
	}
}
