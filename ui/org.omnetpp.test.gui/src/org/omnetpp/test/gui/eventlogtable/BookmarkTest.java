package org.omnetpp.test.gui.eventlogtable;

import org.omnetpp.eventlogtable.widgets.EventLogTable;
import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.EditorPartAccess;
import com.simulcraft.test.gui.access.TreeItemAccess;
import com.simulcraft.test.gui.access.ViewPartAccess;
import com.simulcraft.test.gui.util.WorkbenchUtils;

public class BookmarkTest
    extends EventLogFileTestCase
{
    public void testAddBookmark() throws Exception {
        openFileFromProjectExplorerView();
        addBookmark(findEditorPart(), 1000, "test");
    }

    public void testDeleteBookmark() throws Exception {
        assertEmptyBookmarksView();

        EditorPartAccess editorPart = findEditorPart();
        addBookmark(editorPart, 1000, "test");

        deleteBookmark();
        assertEmptyBookmarksView();
    }

    public void testGotoBookmark() throws Exception {
        assertEmptyBookmarksView();

        EditorPartAccess editorPart = findEditorPart();
        addBookmark(editorPart, 1000, "test");
        editorPart.closeWithHotKey();
        
        gotoBookmark("test");
        findEditorPart();
    }
    
    private void addBookmark(EditorPartAccess editorPart, int eventNumber, String description) {
        Access.findDescendantControl(editorPart.getComposite().getComposite(), EventLogTable.class);
    }

    private void gotoBookmark(String description) {
        ViewPartAccess viewPart = WorkbenchUtils.ensureViewActivated("General", "Bookmarks");
        viewPart.findTree().findTreeItemByContent(description).reveal().doubleClick();
    }

    private void deleteBookmark() {
        ViewPartAccess viewPart = WorkbenchUtils.ensureViewActivated("General", "Bookmarks");
        TreeItemAccess treeItem = viewPart.findTree().findTreeItemByContent("Event Number").reveal();
        treeItem.activateContextMenuWithMouseClick().findMenuItemByLabel("Delete").activateWithMouseClick();
    }

    private void assertEmptyBookmarksView() {
        ViewPartAccess viewPart = WorkbenchUtils.ensureViewActivated("General", "Bookmarks");
        viewPart.findTree().assertEmpty();
    }
}
