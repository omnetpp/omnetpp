package org.omnetpp.test.gui.eventlogtable;

import org.omnetpp.eventlogtable.widgets.EventLogTable;
import org.omnetpp.test.gui.access.EventLogTableAccess;
import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.BookmarkViewAccess;
import com.simulcraft.test.gui.access.EditorPartAccess;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.util.WorkbenchUtils;

public class BookmarkTest
    extends EventLogFileTestCase
{
    public void testAddBookmark() throws Exception {
        ensureBookmarkView().assertEmptyBookmarksView();

        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInEventLogTableEditor();
        addBookmark(findEditorPart(), 1, "test");
        
        WorkbenchUtils.ensureViewActivated("General", "Bookmarks").findTree().assertNotEmpty();
    }

    public void testDeleteBookmark() throws Exception {
        ensureBookmarkView().assertEmptyBookmarksView();

        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInEventLogTableEditor();
        EditorPartAccess editorPart = findEditorPart();
        addBookmark(editorPart, 1, "test");

        BookmarkViewAccess bookmarkView = ensureBookmarkView();
        bookmarkView.deleteBookmark("test");
        bookmarkView.assertEmptyBookmarksView();
    }

    private BookmarkViewAccess ensureBookmarkView() {
        BookmarkViewAccess bookmarkView = (BookmarkViewAccess)WorkbenchUtils.ensureViewActivated("General", "Bookmarks");
        return bookmarkView;
    }

    public void testGotoBookmark() throws Exception {
        ensureBookmarkView().assertEmptyBookmarksView();

        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInEventLogTableEditor();
        EditorPartAccess editorPart = findEditorPart();
        addBookmark(editorPart, 1, "test");
        editorPart.closeWithHotKey();
        
        ensureBookmarkView().gotoBookmark("test");
        findEditorPart();
    }

    private void addBookmark(EditorPartAccess editorPart, int eventNumber, String description) {
        EventLogTableAccess eventLogTable = (EventLogTableAccess)Access.createAccess(Access.findDescendantControl(editorPart.getComposite().getControl(), EventLogTable.class));
        eventLogTable.activateContextMenuWithMouseClick(eventNumber).findMenuItemByLabel("Toggle bookmark").activateWithMouseClick();
        ShellAccess shell = Access.findShellWithTitle(".*Bookmark.*");
        shell.findTextAfterLabel(".*Bookmark.*").typeIn(description);
        shell.findButtonWithLabel("OK").click();
    }
}
