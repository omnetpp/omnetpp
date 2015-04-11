/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.eclipse.ui.IViewPart;


public class BookmarkViewAccess
    extends ViewPartAccess
{
    public BookmarkViewAccess(IViewPart bookmarkView) {
        super(bookmarkView);
    }

    public void gotoBookmark(String description) {
        findTree().findTreeItemByContent(".*" + description + ".*").reveal().doubleClick();
    }

    public void deleteBookmark(String description) {
        TreeItemAccess treeItem = findTree().findTreeItemByContent(".*" + description + ".*").reveal();
        treeItem.activateContextMenuWithMouseClick(new Point(1, 1)).findMenuItemByLabel(".*Delete.*").activateWithMouseClick();
    }

    public void assertEmptyBookmarksView() {
        findTree().assertEmpty();
    }
}
