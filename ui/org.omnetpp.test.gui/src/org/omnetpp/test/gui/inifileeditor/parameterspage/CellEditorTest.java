package org.omnetpp.test.gui.inifileeditor.parameterspage;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.util.WorkspaceUtils;

public class CellEditorTest extends InifileEditorTestCase {
    final String nedText = 
        "network TestNetwork {\n" +
        "  parameters:\n" +
        "    int par1;\n" +
        "    int par2;\n" +
        "    int par3;\n" +
        "    int par4;\n" +
        "    int par5;\n" +
        "}\n";
    final String content = 
        "[General]\n" +
        "network = TestNetwork\n" +
        "**.par1 = 100\n" +
        "**.par2 = 200\n" +
        "**.par3 = 300\n" +
        "[Config Foo]\n" +
        "**.par4 = 400\n" +
        "**.par5 = 500\n";

    private TreeAccess prepareTest() throws Exception {
        createFileWithContent(content);
        WorkspaceUtils.createFileWithContent(projectName+"/test.ned", nedText);
        openFileFromProjectExplorerView();
        CompositeAccess parametersPage = findInifileEditor().ensureActiveFormPage("Parameters");
        parametersPage.findComboAfterLabel("Config.*").selectItem("Config Foo");
        return parametersPage.findTree();
    }

    private void assertTextEditorContentMatches(String content) {
        findInifileEditor().ensureActiveTextEditor().assertContentIgnoringWhiteSpace(content);
    }

    public void testKeyUp1() throws Exception {
        // edit previous line
        TreeAccess tree = prepareTest();
        tree.findTreeItemByContent(".*par3").activateCellEditor(1);
        tree.pressKeySequence("foo");
        tree.pressKey(SWT.ARROW_UP);
        tree.pressKeySequence("bar");
        tree.pressEnter();
        assertTextEditorContentMatches(content.replace("300", "foo").replace("200", "bar"));
    }

    public void testKeyUp2() throws Exception {
        // edit top line (cannot move up)
        TreeAccess tree = prepareTest();
        tree.findTreeItemByContent(".*par4").activateCellEditor(1);
        tree.pressKeySequence("foo");
        tree.pressKey(SWT.ARROW_UP); // already at the top, so UP key won't do anything
        tree.pressKeySequence("bar");
        tree.pressEnter();
        assertTextEditorContentMatches(content.replace("400", "foobar"));
    }

    public void testKeyDown() throws Exception {
        // edit next line
        TreeAccess tree = prepareTest();
        tree.findTreeItemByContent(".*par2").activateCellEditor(1);
        tree.pressKeySequence("foo");
        tree.pressKey(SWT.ARROW_DOWN);
        tree.pressKeySequence("bar");
        tree.pressEnter();
        assertTextEditorContentMatches(content.replace("200", "foo").replace("300", "bar"));
    }

    public void testKeyDown2() throws Exception {
        // edit bottom line (cannot move down)
        TreeAccess tree = prepareTest();
        tree.findTreeItemByContent(".*par5").activateCellEditor(1);
        tree.pressKeySequence("foo");
        tree.pressKey(SWT.ARROW_DOWN); // already at the bottom, so DOWN key won't do anything
        tree.pressKeySequence("bar");
        tree.pressEnter();
        assertTextEditorContentMatches(content.replace("500", "foobar"));
    }

    public void testEscKey() throws Exception {
        // Esc should cancel editing
        TreeAccess tree = prepareTest();
        tree.findTreeItemByContent(".*par4").activateCellEditor(1);
        tree.pressKeySequence("foo");
        tree.pressKey(SWT.ESC);
        tree.pressEnter();  // why not
        assertTextEditorContentMatches(content); // unchanged
    }

    public void testFocusLost1() throws Exception {
        // left edge of same tree cell clicked (e.g. to select cell): cell editor should commit
        TreeAccess tree = prepareTest();
        tree.findTreeItemByContent(".*par4").activateCellEditor(1);
        tree.pressKeySequence("foo");
        tree.findTreeItemByContent(".*par4").clickLeftEdge();
        assertTextEditorContentMatches(content.replace("400", "foo"));
    }

    public void testFocusLost2() throws Exception {
        // another cell clicked for editing: cell editor should commit, 
        // and other editor should become activated
        TreeAccess tree = prepareTest();
        tree.findTreeItemByContent(".*par5").activateCellEditor(1);
        tree.pressKeySequence("foo");
        tree.findTreeItemByContent(".*par2").clickColumn(1);
        tree.pressKeySequence("bar"); // verify new cell editor became active
        tree.pressEnter();
        assertTextEditorContentMatches(content.replace("500", "foo").replace("200", "bar"));
    }

    public void testFocusLost3() throws Exception {
        // another widget clicked
        TreeAccess tree = prepareTest();
        tree.findTreeItemByContent(".*par5").activateCellEditor(1);
        tree.pressKeySequence("foo");
        WorkbenchWindowAccess.getWorkbenchWindowAccess().findViewPartByTitle("Project Explorer.*", false).activateWithMouseClick();
        assertTextEditorContentMatches(content.replace("500", "foo"));
    }

    public void testContentAssistWithKeyboardSelection() throws Exception {
        TreeAccess tree = prepareTest();
        tree.findTreeItemByContent(".*par2").activateCellEditor(1);
        tree.pressKey(SWT.DEL);
        tree.pressKey(' ', SWT.CTRL); // Content Assist should come up
        WorkbenchWindowAccess.chooseFromContentAssistPopupWithKeyboard("exponential\\(mean\\).*");
        tree.pressKeySequence("+10"); // append some text to see cell editor is still open
        tree.pressEnter(); // commit cell editor
        assertTextEditorContentMatches(content.replace("200", "exponential(mean)+10"));
    }
    
    public void testContentAssistWithMouseSelection() throws Exception {
        TreeAccess tree = prepareTest();
        tree.findTreeItemByContent(".*par2").activateCellEditor(1);
        tree.pressKeySequence("expon");
        tree.pressKey(' ', SWT.CTRL); // Content Assist should come up, with "exponential()" as first proposal
        WorkbenchWindowAccess.chooseFromContentAssistPopupWithMouse("exponential\\(mean\\).*");
        tree.pressKeySequence("+10"); // append some text to see cell editor is still open
        tree.pressEnter(); // commit cell editor
        assertTextEditorContentMatches(content.replace("200", "exponential(mean)+10"));
    }
}
