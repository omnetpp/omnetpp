package org.omnetpp.test.gui.inifileeditor.sectionspage;

import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

import com.simulcraft.test.gui.access.TreeAccess;

public class SectionDragDropTest extends InifileEditorTestCase {

    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }

    private void dragSection(String section, String targetSection) {
        TreeAccess sectionsTree = findInifileEditor().ensureActiveFormEditor().ensureActiveCategoryPage("Sections").findTree();
        sectionsTree.findTreeItemByContent(section).dragTo(sectionsTree.findTreeItemByContent(targetSection));
    }

    private void assertTextEditorContentMatches(String content) {
        findInifileEditor().ensureActiveTextEditor().assertContentIgnoringWhiteSpace(content);
    }

    public void testDrag() throws Exception {
        // check that drag takes place, and dragged subtree doesn't fall apart
        prepareTest(
                "[Config Foo]\n" +
                "[Config Baz]\nextends = Foo\n" +
                "[Config Bar]\n");
        dragSection("Config Foo", "Config Bar");
        assertTextEditorContentMatches(
                "[Config Foo]\nextends = Bar\n" +
                "[Config Baz]\nextends = Foo\n" +
                "[Config Bar]\n");
    }

    public void testDragToGeneral() throws Exception {
        prepareTest(
                "[Config Foo]\n" +
                "[Config Bar]\nextends = Foo\n");
        dragSection("Config Bar", "General");
        assertTextEditorContentMatches(
                "[Config Foo]\n" +
                "[Config Bar]\n");
    }

    public void testDragGeneral() throws Exception {
        // dragging General does not make sense (file doesn't get changed)
        prepareTest(
                "[General]\n" +
                "[Config Foo]\n");
        dragSection("General", "Config Foo");
        assertTextEditorContentMatches(
                "[General]\n" +
                "[Config Foo]\n");
    }

    public void testDragToCreateCycle() throws Exception {
        // drags that generate a cycle are not allowed (file doesn't get changed)
        prepareTest(
                "[Config Foo]\n" +
                "[Config Bar]\nextends = Foo\n");
        dragSection("Config Foo", "Config Bar");
        assertTextEditorContentMatches(
                "[Config Foo]\n" +
                "[Config Bar]\nextends = Foo\n");
    }

}
