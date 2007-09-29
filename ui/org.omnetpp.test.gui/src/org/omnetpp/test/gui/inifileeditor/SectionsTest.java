package org.omnetpp.test.gui.inifileeditor;

import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.test.gui.access.InifileEditorAccess;
import org.omnetpp.test.gui.access.InifileFormEditorAccess;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TreeAccess;

public class SectionsTest extends InifileEditorTestCase {
    //TODO finish

    private void prepareTest(String content) throws Exception {
        createFileWithContent(content);
        openFileFromProjectExplorerView();
    }
    
    private static GenericTreeNode[] toArray(GenericTreeNode... trees) {
        return trees;
    }

    private static GenericTreeNode n(String labelRegex, GenericTreeNode... children) {
        GenericTreeNode node = new GenericTreeNode(labelRegex);
        for (GenericTreeNode child : children)
            node.addChild(child);
        return node;
    }

    private void checkSectionsTreeViewContent(String inifileContent, GenericTreeNode[] trees) throws Exception {
        prepareTest(inifileContent);
        
        InifileEditorAccess inifileEditor = findInifileEditor();
        InifileFormEditorAccess formEditor = inifileEditor.ensureActiveFormEditor();
        CompositeAccess sectionsPage = formEditor.activateCategoryPage("Sections");
        TreeAccess sectionsTree = sectionsPage.findTree();
        sectionsTree.assertContent(trees);
    }

    public void testEmptyInifile() throws Exception {
        checkSectionsTreeViewContent("", 
                toArray(
                        n("General")));  // still displays "General" with a hollow icon
    }

    public void testGeneralSectionOnly() throws Exception {
        checkSectionsTreeViewContent(
                "[General]\n", 
                toArray(
                        n("General")));
    }

    public void testGeneralSectionWithNetworkOnly() throws Exception {
        checkSectionsTreeViewContent(
                "[General]\n" +
                "network = TestNetwork\n", 
                toArray(
                        n("General.*TestNetwork.*")));
    }

    public void testOneSectionOnly() throws Exception {
        checkSectionsTreeViewContent(
                "[Config TestSection]\n" +
                "network = TestNetwork\n", 
                toArray(
                        n("General", 
                                n(".*TestSection.*TestNetwork.*"))));
    }

}
