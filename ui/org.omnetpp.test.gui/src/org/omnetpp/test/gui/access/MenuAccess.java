package org.omnetpp.test.gui.access;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.test.gui.core.InUIThread;

public class MenuAccess extends WidgetAccess<Menu> {

	public MenuAccess(Menu menu) {
		super(menu);
	}

	public Menu getMenu() {
		return widget;
	}

	/**
	 * Activates the menu item with the given label. If it opens a submenu,
	 * return it, otherwise returns null.
	 */
	@InUIThread
	public MenuAccess activateMenuItemWithMouse(String label) {
		System.out.println("Activating menu item: " + label);
		return findMenuItemByLabel(label).activateWithMouseClick();
	}

	@InUIThread
	public MenuItemAccess findMenuItemByLabel(final String label) {
		try {
			return new MenuItemAccess((MenuItem)theOnlyWidget(collectMenuItems(widget, new IPredicate() {
				public boolean matches(Object object) {
					String menuItemLabel = ((MenuItem)object).getText().replace("&", "");
					return menuItemLabel.matches(label);
				}
			})));
		} catch (RuntimeException e) {
			closeMenus();
			throw e;
		}
	}

	@InUIThread
	public List<MenuItem> collectMenuItems(Menu menu, IPredicate predicate) {
		ArrayList<MenuItem> resultMenuItems = new ArrayList<MenuItem>();
		for (MenuItem menuItem : menu.getItems())
			if (predicate.matches(menuItem))
				resultMenuItems.add(menuItem);
		return resultMenuItems;
	}

	@InUIThread
	public static void closeMenus() {
		// we need to manually close the menus whenever an error occurs during 
		// menu processing (e.g. assert failed: menu item not found or 
		// disabled), otherwise the event loop will sit there forever.
		// However, there seems to be no clean way of doing it; here we try
		// to click at the caption bar (i.e. the window manager decoration)
		// of the workbench window if that's the active shell. We don't do 
		// anything if some other shell is active, as it might have unwanted
		// side effects. Issue: menu might actually be exactly there where
		// we want to click, try avoid hitting it...
		//
		Shell workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
		if (workbenchWindow != null && workbenchWindow == Display.getCurrent().getActiveShell()) {
			Rectangle bounds = workbenchWindow.getBounds(); // note: this includes "trimmings" as well
			if (bounds.width > 200) {
				int x = bounds.x + bounds.width - 150; // click on the right side, to avoid menus which are usually on the left
				int y = bounds.y + 1;  // top edge of caption bar ("trimmings")
				new ShellAccess(workbenchWindow).click(Access.LEFT_MOUSE_BUTTON, x, y);
			}
		}
	}
	
// the following code searches in the menus recursively -- retained just in case it might be needed somewhere...
//	@InUIThread
//	public MenuAccess activateMenuItemWithMouse_Recursive(String label) {
//		System.out.println("Activating menu item: " + label);
//		return findMenuItemByLabelRecursive(label).activateWithMouseClick();
//	}
//
//	@InUIThread
//	public MenuItemAccess findMenuItemByLabelRecursive(final String label) {
//		return new MenuItemAccess((MenuItem)theOnlyWidget(collectMenuItemsRecursive(widget, new IPredicate() {
//			public boolean matches(Object object) {
//				String menuItemLabel = ((MenuItem)object).getText().replace("&", "");
//				return menuItemLabel.matches(label);
//			}
//		})));
//	}
//
//	@InUIThread
//	public List<MenuItem> collectMenuItemsRecursive(Menu menu, IPredicate predicate) {
//		ArrayList<MenuItem> resultMenuItems = new ArrayList<MenuItem>();
//		collectMenuItemsRecursive(menu, predicate, resultMenuItems);
//		return resultMenuItems;
//	}
//
//	protected void collectMenuItemsRecursive(Menu menu, IPredicate predicate, List<MenuItem> resultMenuItems) {
//		for (MenuItem menuItem : menu.getItems()) {
//			if (debug)
//				System.out.println("Trying to collect menu item: " + menuItem.getText());
//
//			if (menuItem.getMenu() != null)
//				collectMenuItemsRecursive(menuItem.getMenu(), predicate, resultMenuItems);
//
//			if (predicate.matches(menuItem)) {
//				System.out.println("--> matches: " + menuItem.getText());
//				resultMenuItems.add(menuItem);
//			}
//		}
//	}

}
