package org.omnetpp.test.gui.inifileeditor.parameterspage;

import org.omnetpp.test.gui.access.InifileEditorAccess;
import org.omnetpp.test.gui.access.InifileFormEditorAccess;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

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
    
    //TODO more tests:
    //  delete multiple lines at once, also from non-contiguous region
    
}
