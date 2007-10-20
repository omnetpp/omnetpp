package org.omnetpp.test.gui.inifileeditor;

import org.eclipse.swt.SWT;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.MultiPageEditorPartAccess;
import com.simulcraft.test.gui.access.StyledTextAccess;
import com.simulcraft.test.gui.access.TextEditorAccess;
import com.simulcraft.test.gui.access.ViewPartAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.util.WorkbenchUtils;
import com.simulcraft.test.gui.util.WorkspaceUtils;

public class ConfigTest extends InifileEditorTestCase {
    @Override
    protected void setUpInternal() throws Exception {
        super.setUpInternal();
        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
        WorkspaceUtils.createFileWithContent(filePath, "");
        WorkbenchUtils.findInProjectExplorerView(filePath).reveal().doubleClick();
        workbenchWindow.findEditorPartByTitle(fileName); //TODO .assertClass(InifileEditor.class)
    }
    
    public void testSyntaxError() throws Throwable {
        //XXX refine
        WorkbenchWindowAccess workbenchWindowAccess = Access.getWorkbenchWindow();
        MultiPageEditorPartAccess multiPageEditorPart = workbenchWindowAccess.findMultiPageEditorPartByTitle(fileName);
        multiPageEditorPart.ensureActiveEditor("Text");
        StyledTextAccess styledText = multiPageEditorPart.findStyledText();
        styledText.assertHasFocus();
        styledText.pressKey('A', SWT.CTRL); // select all
        styledText.typeIn("Hello");

        // The "Problems" view must display a "No such NED network" error
        ViewPartAccess problemsView = workbenchWindowAccess.findViewPartByTitle("Problems", true);
        problemsView.activateWithMouseClick();
        problemsView.findTree().findTreeItemByContent("Line must be in the form.*");
    }

    public void testWrongNetwork() throws Throwable {
        WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
        TextEditorAccess textualEditor = (TextEditorAccess)workbenchWindow.findMultiPageEditorPartByTitle(fileName).ensureActiveEditor("Text");

        textualEditor.findStyledText().typeIn("[General] \nnetwork = Undefined");
        
        WorkbenchUtils.assertErrorMessageInProblemsView(".*No such NED network.*");
    }

}
