/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph;

import org.eclipse.gef.ContextMenuProvider;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.ui.actions.ActionRegistry;
import org.eclipse.gef.ui.actions.GEFActionConstants;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.ui.actions.ActionFactory;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;

public class ModuleEditorContextMenuProvider extends ContextMenuProvider {

    private ActionRegistry actionRegistry;

    public ModuleEditorContextMenuProvider(EditPartViewer viewer, ActionRegistry registry) {
        super(viewer);
        setActionRegistry(registry);
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.eclipse.gef.ContextMenuProvider#menuAboutToShow(org.eclipse.jface.action.IMenuManager)
     */
    public void buildContextMenu(IMenuManager manager) {
        GEFActionConstants.addStandardActionGroups(manager);

        IAction action;

        action = getActionRegistry().getAction(ActionFactory.UNDO.getId());
        manager.appendToGroup(GEFActionConstants.GROUP_UNDO, action);

        action = getActionRegistry().getAction(ActionFactory.REDO.getId());
        manager.appendToGroup(GEFActionConstants.GROUP_UNDO, action);

        action = getActionRegistry().getAction(ActionFactory.PASTE.getId());
        if (action.isEnabled()) manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);

        action = getActionRegistry().getAction(ActionFactory.DELETE.getId());
        if (action.isEnabled()) manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);

        action = getActionRegistry().getAction(GEFActionConstants.DIRECT_EDIT);
        if (action.isEnabled()) manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);


        // Alignment Actions
        MenuManager submenu = new MenuManager(MessageFactory.AlignmentAction_AlignSubmenu_ActionLabelText);

        action = getActionRegistry().getAction(GEFActionConstants.ALIGN_LEFT);
        if (action.isEnabled()) submenu.add(action);

        action = getActionRegistry().getAction(GEFActionConstants.ALIGN_CENTER);
        if (action.isEnabled()) submenu.add(action);

        action = getActionRegistry().getAction(GEFActionConstants.ALIGN_RIGHT);
        if (action.isEnabled()) submenu.add(action);

        submenu.add(new Separator());

        action = getActionRegistry().getAction(GEFActionConstants.ALIGN_TOP);
        if (action.isEnabled()) submenu.add(action);

        action = getActionRegistry().getAction(GEFActionConstants.ALIGN_MIDDLE);
        if (action.isEnabled()) submenu.add(action);

        action = getActionRegistry().getAction(GEFActionConstants.ALIGN_BOTTOM);
        if (action.isEnabled()) submenu.add(action);

        if (!submenu.isEmpty()) manager.appendToGroup(GEFActionConstants.GROUP_REST, submenu);

        action = getActionRegistry().getAction(ActionFactory.SAVE.getId());
        manager.appendToGroup(GEFActionConstants.GROUP_SAVE, action);

    }

    private ActionRegistry getActionRegistry() {
        return actionRegistry;
    }

    private void setActionRegistry(ActionRegistry registry) {
        actionRegistry = registry;
    }

}