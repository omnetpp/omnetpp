package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.TreeItem;

import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.core.NotInUIThread;

public class TreeItemAccess extends ClickableWidgetAccess
{
	public TreeItemAccess(TreeItem treeItem) {
		super(treeItem);
	}

	public TreeItem getTreeItem() {
		return (TreeItem)widget;
	}

	@InUIThread
	public TreeAccess getTree() {
	    return (TreeAccess) createAccess(getTreeItem().getParent());
	}
	
	@InUIThread @Override
	public void click() {
        Assert.assertTrue("point to click is scrolled out", getTree().getAbsoluteBounds().contains(getAbsolutePointToClick()));
	    super.click();
	}

	@InUIThread
	public TreeItemAccess reveal() {
		getTreeItem().getParent().setTopItem(getTreeItem()); // scroll there
		return this;
	}
	
	@Override
	protected Point getAbsolutePointToClick() {
		return getTreeItem().getParent().toDisplay(getCenter(getTreeItem().getBounds()));
	}

	@Override
	protected Menu getContextMenu() {
		return (Menu)getTreeItem().getParent().getMenu();
	}

    @InUIThread
    public void ensureChecked() {
        if (!getTreeItem().getChecked()) {
            click();
            pressKey(' ');
        }
    }

    @InUIThread
    public void ensureExpanded() {
        if (!getTreeItem().getExpanded()) {
            reveal();
            click();
            pressKey(SWT.ARROW_RIGHT);
        }
    }
    
    @InUIThread
    public void clickColumn(int index) {
        Point point = getTreeItem().getParent().toDisplay(getCenter(getTreeItem().getTextBounds(index)));
        Assert.assertTrue("point to click is scrolled out", getTree().getAbsoluteBounds().contains(point));
        clickAbsolute(LEFT_MOUSE_BUTTON, point);
    }

    @NotInUIThread
    public TextAccess activateCellEditor() {
        Control oldFocusControl = getFocusControl().getControl();
        click();
        Control focusControl = getFocusControl().getControl();
        Assert.assertTrue("cell editor could not be activated", focusControl instanceof Text && oldFocusControl != focusControl);
        System.out.println(oldFocusControl.getClass().getSimpleName());
        return (TextAccess)createAccess(focusControl);
    }

    @NotInUIThread
    public TextAccess activateCellEditor(int index) {
        Control oldFocusControl = getFocusControl().getControl();
        clickColumn(index);
        Control focusControl = getFocusControl().getControl();
        Assert.assertTrue("cell editor could not be activated", focusControl instanceof Text && oldFocusControl != focusControl);
        System.out.println(oldFocusControl.getClass().getSimpleName());
        return (TextAccess)createAccess(focusControl);
    }
    
    @NotInUIThread
    public void clickAndTypeOver(String content) {
        TextAccess cellEditor = activateCellEditor();
        cellEditor.typeOver(content);
        cellEditor.pressEnter();
        //assertTextContent(Pattern.quote(content));  //XXX this would assume text gets stored "as is"
    }

    @NotInUIThread
    public void clickAndTypeOver(int index, String content) {
        TextAccess cellEditor = activateCellEditor(index);
        cellEditor.typeOver(content);
        cellEditor.pressEnter();
        //assertTextContent(index, Pattern.quote(content));  //XXX this would assume text gets stored "as is"
    }

    @InUIThread
    public void assertTextContent(String content) {
        Assert.assertTrue(getTreeItem().getText().matches(content));
    }

    @InUIThread
    public void assertTextContent(int index, String content) {
        Assert.assertTrue(getTreeItem().getText(index).matches(content));
    }
}
