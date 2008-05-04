package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.gef.ContextMenuProvider;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.ui.actions.ActionRegistry;
import org.eclipse.gef.ui.actions.GEFActionConstants;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.osgi.util.NLS;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.ContributionItemFactory;
import org.eclipse.ui.keys.IBindingService;
import org.omnetpp.common.image.ImageFactory;

/**
 * Adds actions to the context menu in the graphical editor area
 *
 * @author rhornig
 */
public class GNEDContextMenuProvider extends ContextMenuProvider {

    private ActionRegistry actionRegistry;

    public GNEDContextMenuProvider(EditPartViewer viewer, ActionRegistry registry) {
        super(viewer);
        setActionRegistry(registry);
    }

    /*
     * (non-Javadoc)
     *
     * @see org.eclipse.gef.ContextMenuProvider#menuAboutToShow(org.eclipse.jface.action.IMenuManager)
     */
    @Override
    public void buildContextMenu(IMenuManager manager) {
        GEFActionConstants.addStandardActionGroups(manager);

        IWorkbenchWindow wwin = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        IAction action;

        // add convert menu ONLY if its meaningful
        action = getActionRegistry().getAction(ConvertToNewFormatAction.ID);
        if (action.isEnabled())
            manager.appendToGroup(GEFActionConstants.GROUP_REST, action);

        action = getActionRegistry().getAction(ActionFactory.UNDO.getId());
        manager.appendToGroup(GEFActionConstants.GROUP_UNDO, action);

        action = getActionRegistry().getAction(ActionFactory.REDO.getId());
        manager.appendToGroup(GEFActionConstants.GROUP_UNDO, action);

        action = getActionRegistry().getAction(ActionFactory.CUT.getId());
        manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);
        action = getActionRegistry().getAction(ActionFactory.COPY.getId());
        manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);
        action = getActionRegistry().getAction(ActionFactory.PASTE.getId());
        manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);

        manager.appendToGroup(GEFActionConstants.GROUP_EDIT, new Separator());

        action = getActionRegistry().getAction(ChooseIconAction.ID);
        if (action.isEnabled()) manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);

        action = getActionRegistry().getAction(ParametersDialogAction.ID);
        if (action.isEnabled()) manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);

        action = getActionRegistry().getAction(TogglePinAction.ID);
        if (action.isEnabled()) manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);

        action = getActionRegistry().getAction(ReLayoutAction.ID);
        if (action.isEnabled()) manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);

        // TODO allow extending the context menu via an extensions point
//        action = getActionRegistry().getAction(ExportImageAction.ID);
//        if (action.isEnabled()) manager.appendToGroup(GEFActionConstants.GROUP_SAVE, action);

        action = getActionRegistry().getAction(GEFActionConstants.DIRECT_EDIT);
        action.setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_RENAME));
        if (action.isEnabled()) manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);

        action = getActionRegistry().getAction(ActionFactory.DELETE.getId());
        if (action.isEnabled()) manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);

        // Alignment Actions
        MenuManager submenu = new MenuManager("&Align");

        action = getActionRegistry().getAction(GEFActionConstants.ALIGN_LEFT);
        submenu.add(action);

        action = getActionRegistry().getAction(GEFActionConstants.ALIGN_CENTER);
        submenu.add(action);

        action = getActionRegistry().getAction(GEFActionConstants.ALIGN_RIGHT);
        submenu.add(action);

        submenu.add(new Separator());

        action = getActionRegistry().getAction(GEFActionConstants.ALIGN_TOP);
        submenu.add(action);

        action = getActionRegistry().getAction(GEFActionConstants.ALIGN_MIDDLE);
        submenu.add(action);

        action = getActionRegistry().getAction(GEFActionConstants.ALIGN_BOTTOM);
        submenu.add(action);

        submenu.add(new Separator());

        action = getActionRegistry().getAction(GEFActionConstants.MATCH_WIDTH);
        submenu.add(action);

        action = getActionRegistry().getAction(GEFActionConstants.MATCH_HEIGHT);
        submenu.add(action);

        if (!submenu.isEmpty()) manager.appendToGroup(GEFActionConstants.GROUP_EDIT, submenu);

        action = getActionRegistry().getAction(GNEDToggleSnapToGeometryAction.ID);
        manager.appendToGroup(GEFActionConstants.GROUP_EDIT, action);

        action = getActionRegistry().getAction(GEFActionConstants.ZOOM_IN);
        manager.appendToGroup(GEFActionConstants.GROUP_VIEW, action);

        action = getActionRegistry().getAction(GEFActionConstants.ZOOM_OUT);
        manager.appendToGroup(GEFActionConstants.GROUP_VIEW, action);

        MenuManager showInSubMenu= new MenuManager(getShowInMenuLabel());
        showInSubMenu.add(ContributionItemFactory.VIEWS_SHOW_IN.create(wwin));
        manager.appendToGroup(GEFActionConstants.GROUP_VIEW, showInSubMenu);

        action = getActionRegistry().getAction(IPageLayout.ID_PROP_SHEET);
        if (action.isEnabled()) manager.appendToGroup(GEFActionConstants.GROUP_VIEW, action);

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