package org.omnetpp.test.gui.inifileeditor.parameterspage;

import org.omnetpp.test.gui.access.InifileEditorAccess;
import org.omnetpp.test.gui.access.InifileFormEditorAccess;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

import com.simulcraft.test.gui.access.CompositeAccess;

public class NewParameterTest extends InifileEditorTestCase {
    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }

    public void testSectionFallbacks() throws Exception {
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
        parametersPage.findTree().findTreeItemByContent(".*newKey.*").click();
        parametersPage.pressKeySequence("**.fooParam");
        parametersPage.pressEnter();
        
        //TODO assert result on the parameters page
    }
}
