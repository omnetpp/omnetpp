package org.omnetpp.test.gui.inifileeditor.parameterspage;

import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.test.gui.access.InifileEditorAccess;
import org.omnetpp.test.gui.access.InifileFormEditorAccess;
import org.omnetpp.test.gui.inifileeditor.InifileEditorTestCase;

import com.simulcraft.test.gui.access.ComboAccess;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TreeAccess;

public class ParametersPageTest extends InifileEditorTestCase {
    //TODO finish

    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }

    private static GenericTreeNode[] toArray(GenericTreeNode... trees) {
        return trees;
    }

    private static GenericTreeNode nw(String containedWord, GenericTreeNode... children) {
        return n(".*\\b" + containedWord + "\\b.*", children);
    }

    private static GenericTreeNode n(String labelRegex, GenericTreeNode... children) {
        GenericTreeNode node = new GenericTreeNode(labelRegex);
        for (GenericTreeNode child : children)
            node.addChild(child);
        return node;
    }

    public void testSectionFallbacks() throws Exception {
        // check combo contents, and that tree always contains section fallback chain
        prepareTest(
                "[General]\n **.par1 = 100\n" +
        		"[Config Foo]\n **.par2 = 200\n" +
        		"[Config Bar]\n extends = Foo\n **.par3 = 300\n" +
        		"[Config Hap]\n extends = Foo\n **.par4 = 400\n");
        InifileEditorAccess inifileEditor = findInifileEditor();
        InifileFormEditorAccess formEditor = inifileEditor.ensureActiveFormEditor();
        CompositeAccess parametersPage = formEditor.activateCategoryPage("Parameters");
        ComboAccess combo = parametersPage.findComboAfterLabel("Config.*");
        TreeAccess parametersTree = parametersPage.findTree();
        
        combo.selectItem("General");
        parametersTree.assertContent(toArray(nw("General", nw("par1"))));
        combo.selectItem("Config Foo");
        parametersTree.assertContent(toArray(nw("Foo", nw("par2")), nw("General", nw("par1"))));
        combo.selectItem("Config Bar");
        parametersTree.assertContent(toArray(nw("Bar", nw("par3")), nw("Foo", nw("par2")), nw("General", nw("par1"))));
        combo.selectItem("Config Hap");
        parametersTree.assertContent(toArray(nw("Hap", nw("par4")), nw("Foo", nw("par2")), nw("General", nw("par1"))));
    }
}
