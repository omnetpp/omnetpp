package org.omnetpp.test.gui.inifileeditor.sectionspage;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TreeItemAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;

public class SectionDeleteTest extends InifileEditorTestCase {
    enum SelectMode {DEL_KEY, BUTTON, CONTEXTMENU };

    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }

    private void deleteSection(String section, SelectMode mode) {
        // go to Sections page, select the section, and press DEL or click the "Remove" button
        CompositeAccess sectionsPage = findInifileEditor().ensureActiveFormEditor().ensureActiveCategoryPage("Sections");
        TreeItemAccess treeItem = sectionsPage.findTree().findTreeItemByContent(section).reveal();
        switch (mode) {
            case DEL_KEY: treeItem.click(); sectionsPage.pressKey(SWT.DEL); break;
            case BUTTON: treeItem.click(); sectionsPage.findButtonWithLabel("Remove.*").activateWithMouseClick(); break;
            case CONTEXTMENU: treeItem.chooseFromContextMenu("Remove"); break;
        }
        // confirmation dialog
        WorkbenchWindowAccess.getActiveShell().findButtonWithLabel("Yes").activateWithMouseClick();
    }

    private void assertTextEditorContentMatches(String content) {
        findInifileEditor().ensureActiveTextEditor().assertContentIgnoringWhiteSpace(content);
    }

    private void basicDeleteSectionTest(SelectMode mode) throws Exception {
        prepareTest(
                "[Config Foo]\n" +
                "description = \"some foo\"\n" +
                "network = FooNetwork\n" +
                "[Config Bar]\n" +
                "extends = Foo\n");
        deleteSection("Config Foo .*", mode);
        assertTextEditorContentMatches(
                "[Config Bar]\n" +
                "extends = Foo\n");
    }

    public void testDeleteByButton() throws Exception {
        basicDeleteSectionTest(SelectMode.BUTTON);
    }

    public void testDeleteByKeyboard() throws Exception {
        basicDeleteSectionTest(SelectMode.DEL_KEY);
    }

    public void testDeleteByContextMenu() throws Exception {
        basicDeleteSectionTest(SelectMode.CONTEXTMENU);
    }


}
