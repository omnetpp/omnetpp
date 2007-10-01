package org.omnetpp.test.gui.inifileeditor;

import org.omnetpp.test.gui.access.InifileEditorAccess;
import org.omnetpp.test.gui.access.InifileFormEditorAccess;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TreeAccess;

public class SectionDialogTest extends InifileEditorTestCase {
    //TODO finish

    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }

    public void testCreateSection() throws Exception {
        // TODO: create one (!) section, and check the result in text mode
        prepareTest("");
        InifileEditorAccess inifileEditor = findInifileEditor();
        InifileFormEditorAccess formEditor = inifileEditor.ensureActiveFormEditor();
        CompositeAccess sectionsPage = formEditor.activateCategoryPage("Sections");
        
        formEditor.createSectionByDialog("Foo", "some foo", null, null);
        formEditor.createSectionByDialog("General", null, null, null);
        formEditor.createSectionByDialog("Bar", "some bar", null, "BarNetwork");
        formEditor.createSectionByDialog("Maz", "some maz", "Bar", null);

        //TODO test contents
        TreeAccess sectionsTree = sectionsPage.findTree();
        //sectionsTree.assertContent(trees);
        //...
        
        formEditor.editSectionByDialog("Bar", false, "Bar2", null, null, null);
        formEditor.editSectionByDialog("Bar2", true, "Bar3", null, null, null);
    }

}
