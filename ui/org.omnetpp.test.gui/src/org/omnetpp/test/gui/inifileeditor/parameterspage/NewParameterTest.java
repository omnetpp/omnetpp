package org.omnetpp.test.gui.inifileeditor.parameterspage;

import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TreeAccess;

public class NewParameterTest extends InifileEditorTestCase {

    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }

    private void assertTextEditorContentMatches(String content) {
        findInifileEditor().ensureActiveTextEditor().assertContentIgnoringWhiteSpace(content);
    }

    public void testAddAndRenameParameter() throws Exception {
        // check adding a parameter
        prepareTest(
                "[General]\n" +
        		"[Config Foo]\n" +
        		"network = FooNetwork\n" +
        		"**.par1 = 100\n");
        CompositeAccess parametersPage = findInifileEditor().ensureActiveFormPage("Parameters");
        parametersPage.findComboAfterLabel("Config.*").selectItem("Config Foo");

        parametersPage.findButtonWithLabel("New").selectWithMouseClick();
        TreeAccess tree = parametersPage.findTree();
        tree.findTreeItemByContent(".*newKey.*").clickAndTypeOver(0, "**.fooParam");
        // note: detailed cell editor tests are in EditParameterTest

        assertTextEditorContentMatches(
                "[General]\n" +
                "[Config Foo]\n" +
                "network = FooNetwork\n" +
                "**.par1 = 100\n" +
                "**.fooParam =\n");
    }

    public void testAddSeveralParameters() throws Exception {
        // clicking "New" several times generates unique names
        prepareTest(
                "[General]\n" +
                "**.par1 = 100\n");
        CompositeAccess parametersPage = findInifileEditor().ensureActiveFormPage("Parameters");

        parametersPage.findButtonWithLabel("New").selectWithMouseClick();
        parametersPage.findButtonWithLabel("New").selectWithMouseClick();
        parametersPage.findButtonWithLabel("New").selectWithMouseClick();

        assertTextEditorContentMatches(
                "[General]\n" +
                "**.par1 = 100\n" +
                "**.newKey2 =\n" +
                "**.newKey1 =\n" +
                "**.newKey =\n");
    }

}
