package org.omnetpp.ned.editor.graph.actions;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.PositionConstants;
import org.eclipse.gef.ui.actions.ActionRegistry;
import org.eclipse.gef.ui.actions.AlignmentRetargetAction;
import org.eclipse.gef.ui.actions.GEFActionConstants;
import org.eclipse.gef.ui.actions.MatchHeightRetargetAction;
import org.eclipse.gef.ui.actions.MatchWidthRetargetAction;
import org.eclipse.gef.ui.actions.RedoRetargetAction;
import org.eclipse.gef.ui.actions.UndoRetargetAction;
import org.eclipse.gef.ui.actions.ZoomInRetargetAction;
import org.eclipse.gef.ui.actions.ZoomOutRetargetAction;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.RetargetAction;
import org.eclipse.ui.part.EditorActionBarContributor;

/**
 * Adds Graphical ned editor actions to the action bar.
 * It is a copy of the original GEF ActionBarContributor because of the NPE issue if a non GEF editor
 * gets activated. once it is fixed in GEF their class can be derived from ActionBarContributor
 *
 * @author rhornig
 */
public class GNEDActionBarContributor extends EditorActionBarContributor {

    private ActionRegistry registry = new ActionRegistry();

    /**
     * Contains the {@link RetargetAction}s that are registered as global action handlers.  We
     * need to hold on to these so that we can remove them as PartListeners in dispose().
     */
    private List<RetargetAction> retargetActions = new ArrayList<RetargetAction>();
    private final List<String> globalActionKeys = new ArrayList<String>();

    /**
     * Adds the given action to the action registry.
     *
     * @param action the action to add
     */
    protected void addAction(IAction action) {
        getActionRegistry().registerAction(action);
    }

    /**
     * Indicates the existence of a global action identified by the specified key. This global
     * action is defined outside the scope of this contributor, such as the Workbench's undo
     * action, or an action provided by a workbench ActionSet. The list of global action keys
     * is used whenever the active editor is changed ({@link #setActiveEditor(IEditorPart)}).
     * Keys provided here will result in corresponding actions being obtained from the active
     * editor's <code>ActionRegistry</code>, and those actions will be registered with the
     * ActionBars for this contributor. The editor's action handler and the global action must
     * have the same key.
     *
     * @param key the key identifying the global action
     */
    protected void addGlobalActionKey(String key) {
        globalActionKeys.add(key);
    }

    /**
     * Adds the specified RetargetAction to this contributors <code>ActionRegistry</code>. The
     * RetargetAction is also added as a <code>IPartListener</code> of the contributor's page.
     * Also, the retarget action's ID is flagged as a global action key, by calling {@link
     * #addGlobalActionKey(String)}.
     *
     * @param action the retarget action being added
     */
    protected void addRetargetAction(RetargetAction action) {
        addAction(action);
        retargetActions.add(action);
        getPage().addPartListener(action);
        addGlobalActionKey(action.getId());
    }


    /**
     * Disposes the contributor. Removes all {@link RetargetAction}s that were {@link
     * org.eclipse.ui.IPartListener}s on the {@link org.eclipse.ui.IWorkbenchPage} and
     * disposes them. Also disposes the action registry.
     * <P>
     * Subclasses may extend this method to perform additional cleanup.
     * @see org.eclipse.ui.part.EditorActionBarContributor#dispose()
     */
    @Override
    public void dispose() {
        for (RetargetAction action : retargetActions ) {
            getPage().removePartListener(action);
            action.dispose();
        }
        registry.dispose();
        retargetActions = null;
        registry = null;
    }

    /**
     * Retrieves an action from the action registry using the given ID,
     * or null if not found.
     */
    protected IAction getAction(String id) {
        return getActionRegistry().getAction(id);
    }

    /**
     * Returns this contributor's ActionRegsitry.
     */
    protected ActionRegistry getActionRegistry() {
        return registry;
    }

    /* (non-Javadoc)
     * @see EditorActionBarContributor#init(IActionBars)
     */
    @Override
    public void init(IActionBars bars) {
        buildActions();
        declareGlobalActionKeys();
        super.init(bars);
    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.IEditorActionBarContributor#setActiveEditor(IEditorPart)
     */
    @Override
    public void setActiveEditor(IEditorPart editor) {
        ActionRegistry registry = (ActionRegistry)editor.getAdapter(ActionRegistry.class);
        IActionBars bars = getActionBars();
        for (int i = 0; i < globalActionKeys.size(); i++) {
            String id = globalActionKeys.get(i);
            // XXX fix for the NPE if non GEF editor is activated (in that case the global
            // actions should be removed)
            IAction action = registry == null ? null : registry.getAction(id);
            // end fix
            bars.setGlobalActionHandler(id, action);
        }
    }

    /* (non-Javadoc)
     * @see org.eclipse.gef.ui.actions.ActionBarContributor#buildActions()
     */
    protected void buildActions() {
        addRetargetAction(new UndoRetargetAction());
        addRetargetAction(new RedoRetargetAction());

        RetargetAction retAction;
        addRetargetAction(retAction = new RetargetAction(TogglePinAction.ID, TogglePinAction.MENUNAME, IAction.AS_CHECK_BOX));
        retAction.setToolTipText(TogglePinAction.TOOLTIP);
        retAction.setImageDescriptor(TogglePinAction.IMAGE);
        addRetargetAction(retAction = new RetargetAction(ReLayoutAction.ID, ReLayoutAction.MENUNAME));
        retAction.setToolTipText(ReLayoutAction.TOOLTIP);
        retAction.setImageDescriptor(ReLayoutAction.IMAGE);

        addRetargetAction(new AlignmentRetargetAction(PositionConstants.LEFT));
        addRetargetAction(new AlignmentRetargetAction(PositionConstants.CENTER));
        addRetargetAction(new AlignmentRetargetAction(PositionConstants.RIGHT));
        addRetargetAction(new AlignmentRetargetAction(PositionConstants.TOP));
        addRetargetAction(new AlignmentRetargetAction(PositionConstants.MIDDLE));
        addRetargetAction(new AlignmentRetargetAction(PositionConstants.BOTTOM));

        addRetargetAction(new ZoomInRetargetAction());
        addRetargetAction(new ZoomOutRetargetAction());

        addRetargetAction(new MatchWidthRetargetAction());
        addRetargetAction(new MatchHeightRetargetAction());

        addRetargetAction(retAction = new RetargetAction(GNEDToggleSnapToGeometryAction.ID, GNEDToggleSnapToGeometryAction.MENUNAME,
                IAction.AS_CHECK_BOX));
        retAction.setToolTipText(GNEDToggleSnapToGeometryAction.TOOLTIP);
        retAction.setImageDescriptor(GNEDToggleSnapToGeometryAction.IMAGE);

    }

    /* (non-Javadoc)
     * @see org.eclipse.gef.ui.actions.ActionBarContributor#declareGlobalActionKeys()
     */
    protected void declareGlobalActionKeys() {
        addGlobalActionKey(ActionFactory.PRINT.getId());
        addGlobalActionKey(ActionFactory.SELECT_ALL.getId());
        addGlobalActionKey(ActionFactory.DELETE.getId());
        addGlobalActionKey(ActionFactory.CUT.getId());
        addGlobalActionKey(ActionFactory.COPY.getId());
        addGlobalActionKey(ActionFactory.PASTE.getId());
    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.part.EditorActionBarContributor#contributeToToolBar(IToolBarManager)
     */
    @Override
    public void contributeToToolBar(IToolBarManager tbm) {
        tbm.add(getAction(ActionFactory.UNDO.getId()));
        tbm.add(getAction(ActionFactory.REDO.getId()));

        tbm.add(new Separator());
    	tbm.add(getAction(TogglePinAction.ID));
    	tbm.add(getAction(ReLayoutAction.ID));

    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.part.EditorActionBarContributor#contributeToMenu(IMenuManager)
     */
    @Override
    public void contributeToMenu(IMenuManager menubar) {

        MenuManager viewMenu = new MenuManager("View");
        viewMenu.add(getAction(GEFActionConstants.ZOOM_IN));
        viewMenu.add(getAction(GEFActionConstants.ZOOM_OUT));
        viewMenu.add(new Separator());
        viewMenu.add(getAction(GNEDToggleSnapToGeometryAction.ID));
        viewMenu.add(new Separator());
        viewMenu.add(getAction(GEFActionConstants.ALIGN_LEFT));
        viewMenu.add(getAction(GEFActionConstants.ALIGN_CENTER));
        viewMenu.add(getAction(GEFActionConstants.ALIGN_RIGHT));
        viewMenu.add(new Separator());
        viewMenu.add(getAction(GEFActionConstants.ALIGN_TOP));
        viewMenu.add(getAction(GEFActionConstants.ALIGN_MIDDLE));
        viewMenu.add(getAction(GEFActionConstants.ALIGN_BOTTOM));
        viewMenu.add(new Separator());
        viewMenu.add(getAction(GEFActionConstants.MATCH_WIDTH));
        viewMenu.add(getAction(GEFActionConstants.MATCH_HEIGHT));

        // adds a VIEW menu to the main toolbar
        menubar.insertAfter(IWorkbenchActionConstants.M_EDIT, viewMenu);
    }

}
