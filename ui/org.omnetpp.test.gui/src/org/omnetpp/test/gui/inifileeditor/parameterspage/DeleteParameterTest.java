package org.omnetpp.test.gui.inifileeditor.parameterspage;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.InifileEditorAccess;
import org.omnetpp.test.gui.access.InifileFormEditorAccess;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TreeAccess;

public class DeleteParameterTest extends InifileEditorTestCase {
    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }

    private void assertTextEditorContentMatches(String content) {
        findInifileEditor().ensureActiveTextEditor().assertContentIgnoringWhiteSpace(content);
    }

    public void testDeleteParameter() throws Exception {
        prepareTest(
                "[General]\n" +
                "[Config Foo]\n" +
                "network = FooNetwork\n" +
                "**.par1 = 100\n" +
                "**.fooParam = 200\n");
        InifileEditorAccess inifileEditor = findInifileEditor();
        InifileFormEditorAccess formEditor = inifileEditor.ensureActiveFormEditor();
        CompositeAccess parametersPage = formEditor.activateCategoryPage("Parameters");
        parametersPage.findComboAfterLabel("Config.*").selectItem("Config Foo");

        TreeAccess tree = parametersPage.findTree();
        tree.findTreeItemByContent(".*fooParam.*").click();
        parametersPage.findButtonWithLabel("Remove").activateWithMouseClick();

        assertTextEditorContentMatches(
                "[General]\n" +
                "[Config Foo]\n" +
                "network = FooNetwork\n" +
                "**.par1 = 100\n");
    }

    public void testDeleteSeveralParameters() throws Exception {
        // delete multiple lines at once, from non-contiguous region
        prepareTest(
                "[General]\n" +
                "**.par1 = 100\n" +
                "**.par2 = 200\n" +
                "**.par3 = 300\n" +
                "**.par4 = 400\n" +
                "[Config Foo]\n" +
                "network = FooNetwork\n" +
                "**.foo1 = 100\n" +
                "**.foo2 = 200\n" +
                "**.foo3 = 300\n" +
                "**.foo4 = 400\n");
        InifileFormEditorAccess formEditor = findInifileEditor().ensureActiveFormEditor();
        CompositeAccess parametersPage = formEditor.activateCategoryPage("Parameters");
        parametersPage.findComboAfterLabel("Config.*").selectItem("Config Foo");

        TreeAccess tree = parametersPage.findTree();
        tree.holdDownModifiers(SWT.CONTROL);
        tree.findTreeItemByContent(".*foo3.*").reveal().clickLeftEdge();
        Access.sleep(2);  // if this is missing, foo3 won't get selected (???)
        tree.findTreeItemByContent(".*foo2.*").reveal().clickLeftEdge();
        tree.findTreeItemByContent(".*par1.*").reveal().clickLeftEdge();
        tree.findTreeItemByContent(".*par4.*").reveal().clickLeftEdge();
        tree.releaseModifiers(SWT.CONTROL);
        
        //parametersPage.findButtonWithLabel("Remove").activateWithMouseClick();
        parametersPage.pressKey(SWT.DEL);

        assertTextEditorContentMatches(
                "[General]\n" +
                "**.par2 = 200\n" +
                "**.par3 = 300\n" +
                "[Config Foo]\n" +
                "network = FooNetwork\n" +
                "**.foo1 = 100\n" +
                "**.foo4 = 400\n");
    }
    
}
