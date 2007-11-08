package com.simulcraft.test.gui.access;

import java.util.ArrayList;
import java.util.List;

import junit.framework.Assert;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Decorations;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.core.UIStep;


public class MenuAccess extends WidgetAccess {

	public MenuAccess(Menu menu) {
		super(menu);
	}

    @Override
	public Menu getWidget() {
		return (Menu)widget;
	}

	/**
	 * Activates the menu item with the given label. If it opens a submenu,
	 * return it, otherwise returns null.
	 */
	@UIStep
	public MenuAccess activateMenuItemWithMouse(String label) {
		log(debug, "Activating menu item: " + label);
		return findMenuItemByLabel(label).activateWithMouseClick();
	}

	@UIStep
	public MenuItemAccess findMenuItemByLabel(final String label) {
		return new MenuItemAccess((MenuItem)findMenuItem(getWidget(), new IPredicate() {
			public boolean matches(Object object) {
				String menuItemLabel = ((MenuItem)object).getText().replace("&", "");
				return menuItemLabel.matches(label);
			}
			
			@Override
			public String toString() {
			    return "menu item with label: "+label;
			}
		}));
	}

	@UIStep
    public MenuItem findMenuItem(Menu menu, IPredicate predicate) {
        return (MenuItem)theOnlyWidget(collectMenuItems(getWidget(), predicate), predicate);
	}

	@UIStep
	public List<MenuItem> collectMenuItems(Menu menu, IPredicate predicate) {
		ArrayList<MenuItem> resultMenuItems = new ArrayList<MenuItem>();
		for (MenuItem menuItem : menu.getItems())
			if (predicate.matches(menuItem))
				resultMenuItems.add(menuItem);
		return resultMenuItems;
	}
	
    @UIStep
    public void assertMenuItemsEnabled(String[] labels) {
        for (String label : labels) 
            findMenuItemByLabel(label).assertEnabled();        
    }

    @UIStep
    public void assertMenuItemsDisabled(String[] labels) {
        for (String label : labels) 
            findMenuItemByLabel(label).assertDisabled();        
    }

    @UIStep
    public void assertVisible() {
        Assert.assertTrue("menu not visible", getWidget().isVisible());
    }

    @UIStep
    public static MenuAccess findPopupMenu() {
        ArrayList<Menu> menus = new ArrayList<Menu>();

        for (Shell shell : Display.getCurrent().getShells())
            collectMenus(shell, new IPredicate() {
                public boolean matches(Object object) {
                    Menu menu = (Menu)object;
                    return menu.isVisible() && menu.getShell().getMenuBar() != menu;
                }
            }, menus);
        
        Menu menu = (Menu)theOnlyObject(menus, null);

        if (debug)
            Access.dumpMenu(menu);

        return new MenuAccess(menu);
    }

    private static void collectMenus(Composite composite, IPredicate predicate, List<Menu> collectedMenus) {
        if (composite instanceof Decorations) {
            Menu[] menus = (Menu[])ReflectionUtils.getFieldValue(composite, "menus");
            
            if (menus != null)
                for (Menu menu : menus)
                    if (menu != null && predicate.matches(menu))
                        collectedMenus.add(menu);
        }

        for (Control control : composite.getChildren())
            if (control instanceof Composite)
                collectMenus(((Composite)control), predicate, collectedMenus);
    }
}
