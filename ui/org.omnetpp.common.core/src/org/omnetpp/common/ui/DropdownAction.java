/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IContributionItem;
import org.eclipse.jface.action.IMenuCreator;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.ui.PlatformUI;

/**
 * An action that displays other actions in a drop-down menu, and can be made
 * to cycle through them. The latter is useful e.g. to cycle though display modes.
 * A DropDownAction is most useful if it's placed on a toolbar.
 * If sub-actions are defined with the Action.AS_RADIO_BUTTON style, the current action
 * will always be checked. If labelPrefix is non-null, the action's text will take
 * over the text of the current action, prefixed with the labelPrefix string.
 *
 * @author andras
 */
public class DropdownAction extends Action {
    private boolean cyclethrough = false;
    private int menuIndex = -1;
    private MenuManager menuManager;
    private String labelPrefix = null;

    public DropdownAction(String text, String labelPrefix, ImageDescriptor image, boolean cyclethrough, IAction[] actions) {
        this(text, labelPrefix, image, cyclethrough);
        setActions(actions);
    }

    public DropdownAction(String text, String labelPrefix, ImageDescriptor image, boolean cyclethrough) {
        super(text, Action.AS_DROP_DOWN_MENU);
        menuManager = new MenuManager();
        setImageDescriptor(image);
        setCyclethrough(cyclethrough);
        setLabelPrefix(labelPrefix);
    }

    /**
     * Actions added to this manager will appear in the drop-down menu of the action.
     * To add/remove or update actions, add an IMenuListener to this manager, and do it
     * from its menuAboutToShow() method.
     */
    public MenuManager getMenuManager() {
        return menuManager;
    }

    /**
     * Shortcut: actions can be bulk-added via this method, no need to
     * add them one by one via menuManager. A null element will add a separator.
     */
    public void setActions(IAction[] actions) {
        menuManager.removeAll();
        for (IAction action : actions) {
            if (action != null)
                menuManager.add(action);
            else
                menuManager.add(new Separator());
        }
        menuManager.update(true);
    }

    public boolean getCyclethrough() {
        return cyclethrough;
    }

    public void setCyclethrough(boolean cyclethrough) {
        this.cyclethrough = cyclethrough;
    }

    public String getLabelPrefix() {
        return labelPrefix;
    }

    public void setLabelPrefix(String labelPrefix) {
        this.labelPrefix = labelPrefix;
    }

    @Override
    public void run() {
        if (menuManager.isEmpty())
            return;
        if (menuIndex == -1) {
            bumpMenuIndex();
            update();
        }
        IAction action = getCurrentActionIfValid();
        if (action == null)
            MessageDialog.openInformation(PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell(), "Confirm", "The selected action is disabled.");
        else {
            action.run();
            action.setChecked(true);
        }
        if (getCyclethrough()) {
            bumpMenuIndex();
            update();
        }
    }

    protected IAction getCurrentActionIfValid() {
        IContributionItem[] items = menuManager.getItems();
        if (menuIndex >= 0 && menuIndex < items.length) {
            IContributionItem item = items[menuIndex];
            if (item != null && item instanceof ActionContributionItem) {
                IAction action = ((ActionContributionItem)item).getAction();
                if (action != null && action.isEnabled())
                    return action;
            }
        }
        return null;
    }

    protected void bumpMenuIndex() {
        IContributionItem[] items = menuManager.getItems();
        menuIndex = (menuIndex+1) % items.length;
        for (int i = 0; i < items.length; i++) {
            if (getCurrentActionIfValid() != null)
                break;
            menuIndex = (menuIndex+1) % items.length;
        }
    }

    protected void update() {
        // if there's a prefix given, take over the current action's label prefixed with it
        IAction action = getCurrentActionIfValid();
        if (action != null && labelPrefix != null)
            setText(labelPrefix + action.getText());
    }

    @Override
    public IMenuCreator getMenuCreator() {
        return new IMenuCreator() {
            private Menu contextMenu;

            @Override
            public Menu getMenu(Menu parent) {
                throw new IllegalArgumentException();
            }

            @Override
            public Menu getMenu(Control parent) {
                Assert.isTrue(contextMenu == null);
                return contextMenu = menuManager.createContextMenu(parent);
            }

            @Override
            public void dispose() {
                if (contextMenu != null)
                    contextMenu.dispose();
            }
        };
    }
};
