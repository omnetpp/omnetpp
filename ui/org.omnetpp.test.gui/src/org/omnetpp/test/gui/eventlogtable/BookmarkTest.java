package org.omnetpp.test.gui.eventlogtable;

import org.eclipse.swt.graphics.Point;
import org.omnetpp.eventlogtable.widgets.EventLogTable;
import org.omnetpp.test.gui.access.EventLogTableAccess;
import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.EditorPartAccess;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.TreeItemAccess;
import com.simulcraft.test.gui.access.ViewPartAccess;
import com.simulcraft.test.gui.util.WorkbenchUtils;

public class BookmarkTest
    extends EventLogFileTestCase
{
    public void testAddBookmark() throws Exception {
        assertEmptyBookmarksView();

        createFileWithTwoEvents();
        openFileFromProjectExplorerView();
        addBookmark(findEditorPart(), 1, "test");
        
        WorkbenchUtils.ensureViewActivated("General", "Bookmarks").findTree().assertNotEmpty();
    }

    public void testDeleteBookmark() throws Exception {
        assertEmptyBookmarksView();

        createFileWithTwoEvents();
        openFileFromProjectExplorerView();
        EditorPartAccess editorPart = findEditorPart();
        addBookmark(editorPart, 1, "test");

        deleteBookmark("test");
        assertEmptyBookmarksView();
    }

    public void testGotoBookmark() throws Exception {
        assertEmptyBookmarksView();

        createFileWithTwoEvents();
        openFileFromProjectExplorerView();
        EditorPartAccess editorPart = findEditorPart();
        addBookmark(editorPart, 1, "test");
        editorPart.closeWithHotKey();
        
        gotoBookmark("test");
        findEditorPart();
    }

    private void addBookmark(EditorPartAccess editorPart, int eventNumber, String description) {
        EventLogTableAccess eventLogTable = (EventLogTableAccess)Access.createAccess(Access.findDescendantControl(editorPart.getComposite().getControl(), EventLogTable.class));
        eventLogTable.activateContextMenuWithMouseClick(eventNumber).findMenuItemByLabel("Toggle bookmark").activateWithMouseClick();
        ShellAccess shell = Access.findShellWithTitle(".*Bookmark.*");
        shell.findTextAfterLabel(".*Bookmark.*").typeIn(description);
        shell.findButtonWithLabel("OK").click();
    }

    // TODO: factor out
    private void gotoBookmark(String description) {
        ViewPartAccess viewPart = WorkbenchUtils.ensureViewActivated("General", "Bookmarks");
        viewPart.findTree().findTreeItemByContent(".*" + description + ".*").reveal().doubleClick();
    }

    // TODO: factor out
    private void deleteBookmark(String description) {
        ViewPartAccess viewPart = WorkbenchUtils.ensureViewActivated("General", "Bookmarks");
        TreeItemAccess treeItem = viewPart.findTree().findTreeItemByContent(".*" + description + ".*").reveal();
        treeItem.activateContextMenuWithMouseClick(new Point(1, 1)).findMenuItemByLabel(".*Delete.*").activateWithMouseClick();
    }

    // TODO: factor out
    private void assertEmptyBookmarksView() {
        ViewPartAccess viewPart = WorkbenchUtils.ensureViewActivated("General", "Bookmarks");
        viewPart.findTree().assertEmpty();
    }

    // TODO: factor out
    protected void openFileFromProjectExplorerView() {
        WorkbenchUtils.findInProjectExplorerView(filePath).reveal().activateContextMenuWithMouseClick().activateMenuItemWithMouse(".*Open With.*").activateMenuItemWithMouse(".*Event Log Table.*");
    }
}
