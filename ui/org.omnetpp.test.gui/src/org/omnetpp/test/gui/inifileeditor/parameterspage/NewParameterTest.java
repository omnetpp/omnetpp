package org.omnetpp.test.gui.inifileeditor.parameterspage;

import org.omnetpp.test.gui.access.InifileEditorAccess;
import org.omnetpp.test.gui.access.InifileFormEditorAccess;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TreeAccess;

public class NewParameterTest extends InifileEditorTestCase {
    //FIXME cell editing very unreliable. Probably needs refactoring in the Inifile code
    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }

    private void assertTextEditorContentMatches(String content) {
        findInifileEditor().ensureActiveTextEditor().assertContentIgnoringWhiteSpace(content);
    }

    public void testAddParameter() throws Exception {
        // check adding a parameter
        prepareTest(
                "[General]\n" +
        		"[Config Foo]\n" +
        		"network = FooNetwork\n" +
        		"**.par1 = 100\n");
        InifileEditorAccess inifileEditor = findInifileEditor();
        InifileFormEditorAccess formEditor = inifileEditor.ensureActiveFormEditor();
        CompositeAccess parametersPage = formEditor.activateCategoryPage("Parameters");
        parametersPage.findComboAfterLabel("Config.*").selectItem("Config Foo");

        parametersPage.findButtonWithLabel("New").activateWithMouseClick();
        TreeAccess tree = parametersPage.findTree();
        // note: newly find item each time, because tree may have re-loaded
        tree.findTreeItemByContent(".*newKey.*").clickAndTypeOver(2, "some foo parameter");
        tree.findTreeItemByContent(".*newKey.*").clickAndTypeOver(1, "200");
        tree.findTreeItemByContent(".*newKey.*").clickAndTypeOver(0, "**.fooParam");  //FIXME this fails?? (value doesn't get written back)

        //XXX does not work in reverse order: 0,1,2
        //tree.findTreeItemByContent(".*newKey.*").clickAndTypeOver(0, "**.fooParam");
        //tree.findTreeItemByContent("**.fooParam").clickAndTypeOver(1, "200");
        //tree.findTreeItemByContent("**.fooParam").clickAndTypeOver(2, "some foo parameter");

        assertTextEditorContentMatches(
                "[General]\n" +
                "[Config Foo]\n" +
                "network = FooNetwork\n" +
                "**.par1 = 100\n" +
                "**.fooParam = 200  # some foo parameter\n");
    }
    
    //TODO more tests:
    //  fill in / blank out fields
    //  validation
    
}
