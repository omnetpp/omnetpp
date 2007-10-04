package org.omnetpp.test.gui.inifileeditor.parameterspage;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.InifileFormEditorAccess;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TreeAccess;

public class ParameterDragDropTest extends InifileEditorTestCase {

    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }

    private void assertTextEditorContentMatches(String content) {
        findInifileEditor().ensureActiveTextEditor().assertContentIgnoringWhiteSpace(content);
    }

    public void testDrag() throws Exception {
        // move several parameters by dragging them
        prepareTest(
                "[General]\n" +
                "**.par1 = 100\n" +
                "**.par2 = 200\n" +
                "[Config Foo]\n" +
                "**.foo1 = 100\n" +
                "**.foo2 = 200\n" +
                "**.foo3 = 300\n");
        CompositeAccess parametersPage = findInifileEditor().ensureActiveFormPage("Parameters");
        parametersPage.findComboAfterLabel("Config.*").selectItem("Config Foo");

        TreeAccess tree = parametersPage.findTree();
        tree.findTreeItemByContent(".*foo1.*").reveal().clickLeftEdge();
        tree.holdDownModifiers(SWT.CONTROL);
        tree.findTreeItemByContent(".*foo3.*").reveal().clickLeftEdge();
        tree.releaseModifiers(SWT.CONTROL);

        tree.findTreeItemByContent(".*foo3.*").dragTo(tree.findTreeItemByContent("General"));

        assertTextEditorContentMatches(
                "[General]\n" +
                "**.par1 = 100\n" +
                "**.par2 = 200\n" +
                "**.foo1 = 100\n" +
                "**.foo3 = 300\n" +
                "[Config Foo]\n" +
                "**.foo2 = 200\n");
    }
}
