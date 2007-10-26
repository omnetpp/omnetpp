package org.omnetpp.test.gui.sequencechart;

import org.omnetpp.test.gui.access.SequenceChartAccess;
import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.BookmarkViewAccess;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.util.WorkbenchUtils;

public class BookmarkTest
    extends EventLogFileTestCase
{
    public void testAddBookmark() throws Exception {
        ensureBookmarkView().assertEmptyBookmarksView();

        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInSequenceChartEditor();
        addBookmark(1, "test");
        
        WorkbenchUtils.ensureViewActivated("General", "Bookmarks").findTree().assertNotEmpty();
    }

    public void testDeleteBookmark() throws Exception {
        ensureBookmarkView().assertEmptyBookmarksView();

        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInSequenceChartEditor();
        addBookmark(1, "test");

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
        openFileFromProjectExplorerViewInSequenceChartEditor();
        addBookmark(1, "test");
        findEditorPart().closeWithHotKey();
        
        ensureBookmarkView().gotoBookmark("test");
        findEditorPart();
    }

    protected void addBookmark(int eventNumber, String description) {
        SequenceChartAccess sequenceChart = findSequenceChart();
        sequenceChart.selectEventWithMouseClick(eventNumber);
        sequenceChart.selectEventWithMouseClick(eventNumber);
        sequenceChart.activateContextMenuWithMouseClick(eventNumber).findMenuItemByLabel("Toggle bookmark").activateWithMouseClick();
        ShellAccess shell = Access.findShellWithTitle(".*Bookmark.*");
        shell.findTextAfterLabel(".*Bookmark.*").typeIn(description);
        shell.findButtonWithLabel("OK").click();
    }
}
