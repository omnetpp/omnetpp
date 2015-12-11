/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import static org.eclipse.gef.ui.actions.GEFActionConstants.ALIGN_BOTTOM;
import static org.eclipse.gef.ui.actions.GEFActionConstants.ALIGN_CENTER;
import static org.eclipse.gef.ui.actions.GEFActionConstants.ALIGN_LEFT;
import static org.eclipse.gef.ui.actions.GEFActionConstants.ALIGN_MIDDLE;
import static org.eclipse.gef.ui.actions.GEFActionConstants.ALIGN_RIGHT;
import static org.eclipse.gef.ui.actions.GEFActionConstants.ALIGN_TOP;
import static org.eclipse.gef.ui.actions.GEFActionConstants.DIRECT_EDIT;
import static org.eclipse.gef.ui.actions.GEFActionConstants.GROUP_COPY;
import static org.eclipse.gef.ui.actions.GEFActionConstants.GROUP_EDIT;
import static org.eclipse.gef.ui.actions.GEFActionConstants.GROUP_FIND;
import static org.eclipse.gef.ui.actions.GEFActionConstants.GROUP_PRINT;
import static org.eclipse.gef.ui.actions.GEFActionConstants.GROUP_REST;
import static org.eclipse.gef.ui.actions.GEFActionConstants.GROUP_SAVE;
import static org.eclipse.gef.ui.actions.GEFActionConstants.GROUP_UNDO;
import static org.eclipse.gef.ui.actions.GEFActionConstants.GROUP_VIEW;
import static org.eclipse.gef.ui.actions.GEFActionConstants.MATCH_HEIGHT;
import static org.eclipse.gef.ui.actions.GEFActionConstants.MATCH_WIDTH;
import static org.eclipse.ui.IWorkbenchActionConstants.GROUP_ADD;
import static org.eclipse.ui.IWorkbenchActionConstants.MB_ADDITIONS;

import org.eclipse.gef.ContextMenuProvider;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.ui.actions.ActionRegistry;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.osgi.util.NLS;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.ContributionItemFactory;
import org.eclipse.ui.keys.IBindingService;
import org.eclipse.ui.services.IServiceLocator;

/**
 * Adds actions to the context menu in the graphical editor area
 *
 * @author rhornig
 */
public class GNedContextMenuProvider extends ContextMenuProvider {

    private ActionRegistry actionRegistry;

    private IServiceLocator serviceLocator;

    public GNedContextMenuProvider(EditPartViewer viewer, ActionRegistry registry, IServiceLocator serviceLocator) {
        super(viewer);
        setActionRegistry(registry);
        this.serviceLocator = serviceLocator;
    }

    /*
     * (non-Javadoc)
     *
     * @see org.eclipse.gef.ContextMenuProvider#menuAboutToShow(org.eclipse.jface.action.IMenuManager)
     */
    @Override
    public void buildContextMenu(IMenuManager manager) {
//      GEFActionConstants.addStandardActionGroups(manager);
        manager.add(new Separator(GROUP_EDIT));
        manager.add(new Separator(GROUP_FIND));
        manager.add(new Separator(GROUP_VIEW));
        manager.add(new Separator(GROUP_UNDO));
        manager.add(new Separator(GROUP_COPY));
        manager.add(new Separator(GROUP_PRINT));
        manager.add(new Separator(GROUP_ADD));
        manager.add(new Separator(GROUP_REST));
        manager.add(new Separator(MB_ADDITIONS));
        manager.add(new Separator(GROUP_SAVE));

        IWorkbenchWindow wwin = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        IAction action;
        ActionRegistry ar = getActionRegistry();

        action = ar.getAction(PropertiesAction.ID);
        manager.appendToGroup(GROUP_EDIT, action);

        action = ar.getAction(ParametersDialogAction.ID);
        manager.appendToGroup(GROUP_EDIT, action);

        action = ar.getAction(DIRECT_EDIT);
        if (action.isEnabled())
            manager.appendToGroup(GROUP_EDIT, action);

        // add convert menu ONLY if its meaningful
        action = ar.getAction(ConvertToNewFormatAction.ID);
        if (action.isEnabled())
            manager.appendToGroup(GROUP_EDIT, action);

        action = ar.getAction(ActionFactory.CUT.getId());
        manager.appendToGroup(GROUP_COPY, action);

        action = ar.getAction(ActionFactory.COPY.getId());
        manager.appendToGroup(GROUP_COPY, action);

        action = ar.getAction(ActionFactory.PASTE.getId());
        manager.appendToGroup(GROUP_COPY, action);

        action = ar.getAction(ActionFactory.DELETE.getId());
        manager.appendToGroup(GROUP_COPY, action);

        action = ar.getAction(OpenTypeAction.ID);
        manager.appendToGroup(GROUP_FIND, action);

        MenuManager showInSubMenu= new MenuManager(getShowInMenuLabel());
        showInSubMenu.add(ContributionItemFactory.VIEWS_SHOW_IN.create(wwin));
        manager.appendToGroup(GROUP_FIND, showInSubMenu);

        action = ar.getAction(TogglePinAction.ID);
        if (action.isEnabled())
            manager.appendToGroup(GROUP_REST, action);

        // Alignment Actions
        MenuManager submenu = new MenuManager("&Align");
        submenu.add(ar.getAction(ALIGN_LEFT));
        submenu.add(ar.getAction(ALIGN_CENTER));
        submenu.add(ar.getAction(ALIGN_RIGHT));
        submenu.add(new Separator());
        submenu.add(ar.getAction(ALIGN_TOP));
        submenu.add(ar.getAction(ALIGN_MIDDLE));
        submenu.add(ar.getAction(ALIGN_BOTTOM));
        submenu.add(new Separator());
        submenu.add(ar.getAction(MATCH_WIDTH));
        submenu.add(ar.getAction(MATCH_HEIGHT));
        manager.appendToGroup(GROUP_REST, submenu);

        action = ar.getAction(ToggleSnapToGeometryAction.ID);
        manager.appendToGroup(GROUP_REST, action);

        action = ar.getAction(RelayoutAction.ID);
        if (action.isEnabled())
            manager.appendToGroup(GROUP_REST, action);

        action = ar.getAction(ZoomInAction.ID);
        if (action.isEnabled())
            manager.appendToGroup(GROUP_REST, action);

        action = ar.getAction(ZoomOutAction.ID);
        if (action.isEnabled())
            manager.appendToGroup(GROUP_REST, action);

        action = ar.getAction(ScaleUpIconsAction.ID);
        if (action.isEnabled())
            manager.appendToGroup(GROUP_REST, action);

        action = ar.getAction(ScaleDownIconsAction.ID);
        if (action.isEnabled())
            manager.appendToGroup(GROUP_REST, action);

        action = ar.getAction(ExportImageAction.ID);
        if (action != null && action.isEnabled())
            manager.appendToGroup(GROUP_SAVE, action);
    }

    private String getShowInMenuLabel() {
        String keyBinding= null;

        IBindingService bindingService= (IBindingService)PlatformUI.getWorkbench().getAdapter(IBindingService.class);
        if (bindingService != null)
            keyBinding= bindingService.getBestActiveBindingFormattedFor("org.eclipse.ui.navigate.showInQuickMenu");

        if (keyBinding == null)
            keyBinding= "";

        return NLS.bind("Show In \t{0}",keyBinding);
    }

    private ActionRegistry getActionRegistry() {
        return actionRegistry;
    }

    private void setActionRegistry(ActionRegistry registry) {
        actionRegistry = registry;
    }

}