package org.omnetpp.test.gui.sequencechart;

import org.omnetpp.sequencechart.widgets.SequenceChart;
import org.omnetpp.test.gui.access.SequenceChartAccess;
import org.omnetpp.test.gui.core.EventLogFileTestCase;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.BookmarkViewAccess;
import com.simulcraft.test.gui.access.EditorPartAccess;
import com.simulcraft.test.gui.util.WorkbenchUtils;

public class BookmarkTest
    extends EventLogFileTestCase
{
    public void testAddBookmark() throws Exception {
        ensureBookmarkView().assertEmptyBookmarksView();

        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInSequenceChartEditor();
        addBookmark(findEditorPart(), 1, "test");
        
        WorkbenchUtils.ensureViewActivated("General", "Bookmarks").findTree().assertNotEmpty();
    }

    public void testDeleteBookmark() throws Exception {
        ensureBookmarkView().assertEmptyBookmarksView();

        createFileWithTwoEvents();
        openFileFromProjectExplorerViewInSequenceChartEditor();
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
        openFileFromProjectExplorerViewInSequenceChartEditor();
        EditorPartAccess editorPart = findEditorPart();
        addBookmark(editorPart, 1, "test");
        editorPart.closeWithHotKey();
        
        ensureBookmarkView().gotoBookmark("test");
        findEditorPart();
    }

    private void addBookmark(EditorPartAccess editorPart, int eventNumber, String description) {
        SequenceChartAccess sequenceChart = (SequenceChartAccess)Access.createAccess(Access.findDescendantControl(editorPart.getComposite().getControl(), SequenceChart.class));
        sequenceChart.selectEventWithMouseClick(eventNumber);
        sequenceChart.selectEventWithMouseClick(eventNumber);
        sequenceChart.activateContextMenuWithMouseClick(eventNumber).findMenuItemByLabel("Toggle bookmark").activateWithMouseClick();
// TODO: add text description
//        ShellAccess shell = Access.findShellWithTitle(".*Bookmark.*");
//        shell.findTextAfterLabel(".*Bookmark.*").typeIn(description);
//        shell.findButtonWithLabel("OK").click();
    }
}
