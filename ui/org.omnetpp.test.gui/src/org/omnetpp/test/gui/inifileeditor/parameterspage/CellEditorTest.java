package org.omnetpp.test.gui.inifileeditor.parameterspage;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TextAccess;
import com.simulcraft.test.gui.access.TreeAccess;

public class CellEditorTest extends InifileEditorTestCase {
    final String content = 
        "[General]\n" +
        "**.par1 = 100\n" +
        "**.par2 = 200\n" +
        "**.par3 = 300\n" +
        "**.par4 = 400\n" +
        "[Config Foo]\n" +
        "**.par5 = 500\n" +
        "**.par6 = 600\n";

    private TreeAccess prepareTest() throws Exception {
        createFileWithContent(content);
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
    }

    public void testKeyDown() throws Exception {
        // edit next line
    }

    public void testKeyDown2() throws Exception {
        // edit bottom line (cannot move down)
    }

    public void testEscKey() throws Exception {
        // Esc should cancel editing
    }

    public void testFocusLost1() throws Exception {
        // left edge of same tree cell clicked, to select cell
    }

    public void testFocusLost2() throws Exception {
        // another cell clicked for editing
    }

    public void testFocusLost3() throws Exception {
        // another widget clicked
    }

    public void testContentAssistWithMouseSelection() throws Exception {
        // another widget clicked
    }

    public void testContentAssistWithKeyboardSelection() throws Exception {
        // another widget clicked
    }
    
}
