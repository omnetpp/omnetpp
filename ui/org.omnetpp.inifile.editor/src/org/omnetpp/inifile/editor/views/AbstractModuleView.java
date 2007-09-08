package org.omnetpp.inifile.editor.views;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;

import org.eclipse.core.resources.IProject;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.IShowInTarget;
import org.eclipse.ui.part.ShowInContext;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.actions.ActionExt;
import org.omnetpp.inifile.editor.editors.InifileSelectionItem;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamResolution.ParamResolutionType;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.CompoundModuleElement;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
import org.omnetpp.ned.model.pojo.SimpleModuleElement;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

/**
 * Abstract base class for views that display information based on a single NED
 * module or network type, and possibly an inifile. Subclasses are expected to
 * implement the buildContent() method, which will be invoked whenever the
 * selection changes, or there is a change in NED or ini files.
 *
 * @author Andras
 */
public abstract class AbstractModuleView extends ViewWithMessagePart implements IShowInTarget {
	private IEditorPart pinnedToEditor = null;
	private ISelection pinnedToEditorSelection = null;
	private IAction pinAction;
	
	private ISelectionListener selectionChangedListener;
	private IPartListener partListener;
	private INEDChangeListener nedChangeListener;
    private DelayedJob rebuildContentJob = new DelayedJob(200) {
        public void run() {
            rebuildContent();
        }
    };

	public AbstractModuleView() {
	}

	@Override
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);
		hookListeners();
		scheduleRebuildContent();
	}

	@Override
	public void dispose() {
		unhookListeners();
		super.dispose();
	}

	protected void hookListeners() {
		// Listen on selection changes
		selectionChangedListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != AbstractModuleView.this) // ignore our own selection changes
					workbenchSelectionChanged();
			}
		};
		getSite().getPage().addPostSelectionListener(selectionChangedListener);
        getSite().getPage().addSelectionListener(selectionChangedListener);

		// Listens to workbench changes, and invokes activeEditorChanged() whenever the
		// active editor changes. Listening on workbench changes is needed because
		// editor don't always send selection changes when they get opened or closed.
		//
		// NOTE: the view only gets workbench events while it is visible. So we also
		// need to update our contents on getting activated.
		//
		final IEditorPart initialEditor = getActiveEditor();
		partListener = new IPartListener() {
			private IEditorPart activeEditor = initialEditor;

			public void partActivated(IWorkbenchPart part) {
				if (part == AbstractModuleView.this) {
					viewActivated();
				}
				if (part instanceof IEditorPart) {
					activeEditor = (IEditorPart) part;
					activeEditorChanged();
				}
			}

			public void partBroughtToTop(IWorkbenchPart part) {
			}

			public void partClosed(IWorkbenchPart part) {
				if (part == pinnedToEditor) {
					// unpin on closing of the editor we're pinned to
					unpin();
					activeEditorChanged();
				}
				if (part == activeEditor) {
					activeEditor = null;
					activeEditorChanged();
				}
			}

			public void partDeactivated(IWorkbenchPart part) {
			}

			public void partOpened(IWorkbenchPart part) {
			}
		};
		getSite().getPage().addPartListener(partListener);

		// Listen on NED changes as well (note: inifile changes arrive as selection changes)
		nedChangeListener = new INEDChangeListener() {
			public void modelChanged(NEDModelEvent event) {
				nedModelChanged();
			}
		};
		NEDResourcesPlugin.getNEDResources().addNEDModelChangeListener(nedChangeListener);
	}

	protected void unhookListeners() {
		if (selectionChangedListener != null)
			getSite().getPage().removePostSelectionListener(selectionChangedListener);
        if (selectionChangedListener != null)
            getSite().getPage().removeSelectionListener(selectionChangedListener);
		if (partListener != null)
			getSite().getPage().removePartListener(partListener);
		if (nedChangeListener != null)
			NEDResourcesPlugin.getNEDResources().addNEDModelChangeListener(nedChangeListener);
	}

	public void workbenchSelectionChanged() {
		//System.out.println("*** SELECTIONCHANGE");
		scheduleRebuildContent();
	}

	protected void viewActivated() {
		//System.out.println("*** VIEW ACTIVATED");
		scheduleRebuildContent();
	}

	protected void activeEditorChanged() {
		//System.out.println("*** ACTIVE EDITOR CHANGED");
		scheduleRebuildContent();
	}

	protected void nedModelChanged() {
		//System.out.println("*** NED MODEL CHANGE");
		scheduleRebuildContent();
	}

	public void scheduleRebuildContent() {
        rebuildContentJob.restartTimer();
	}

	protected IAction getOrCreatePinAction() {
		if (pinAction == null) {
			pinAction = new ActionExt("Pin", IAction.AS_CHECK_BOX, InifileEditorPlugin.getImageDescriptor("icons/full/elcl16/pin.gif")) {
				@Override
				public void run() {
					if (isChecked())
						pin();
					else
						unpin();
				}
			};
		} 
		return pinAction;
	}	

	/**
	 * Returns the editor the view is pinned to, or the active editor if the view is not pinned.
	 */
	protected IEditorPart getAssociatedEditor() {
		return pinnedToEditor != null ? pinnedToEditor : getActiveEditor();
	}

	/**
	 * Returns the pinned selection if the view is pinned, or the active editor's selection 
	 * if the view is not pinned.
	 */
	protected ISelection getAssociatedEditorSelection() {
		return pinnedToEditorSelection != null ? pinnedToEditorSelection : getActiveEditorSelection();
	}
	
	/**
	 * Pin the view to the current editor's current selection.
	 */
	public void pin() {
		pinnedToEditor = getActiveEditor();
		pinnedToEditorSelection = getActiveEditorSelection();
		pinAction.setChecked(true);
        rebuildContent();  // to update label: "Pinned to: ..."
	}

	/**
	 * Unpin the view: let it follow the active editor selection.
	 */
	public void unpin() {
		pinnedToEditor = null;
		pinnedToEditorSelection = null;
		pinAction.setChecked(false);
        rebuildContent();
	}

	/**
	 * Returns the editor this view is pinned to (see Pin action), or null if view is not pinned.
	 */
	public IEditorPart getPinnedToEditor() {
		return pinnedToEditor;
	}

	/**
	 * Returns the selection this view is pinned to (see Pin action), or null if view is not pinned.
	 */
	public ISelection getPinnedToEditorSelection() {
		return pinnedToEditorSelection;
	}

	/**
     * Tries to find a NED element among the parents which may have
     * parameters (simple module, compound module, channel, submodule).
     */
    private INEDElement findFirstModuleOrSubmodule(INEDElement element) {
        while (element != null) {
            if (element instanceof CompoundModuleElement || element instanceof SimpleModuleElement ||
                    element instanceof SubmoduleElement || element instanceof ModuleInterfaceElement)
                return element;
            element = element.getParent();
        }
        return null;
    }

	public void rebuildContent() {
		if (isDisposed())
			return;

		// Note: we make no attempt to filter out selection changes while view is pinned
		// to a different editor (i.e. not the active editor), because we might miss updates.
		//XXX check.
		
        //System.out.println("*** CONTENT REBUILD");
		IEditorPart activeEditor = getAssociatedEditor();
		if (activeEditor==null) {
			showMessage("There is no active editor.");
			return;
		}

		ISelection selection = getAssociatedEditorSelection();
		if (selection==null) {
			showMessage("Nothing is selected.");
			return;
		}

		if (selection instanceof IStructuredSelection && !selection.isEmpty()) {
			Object element = ((IStructuredSelection)selection).getFirstElement();
			if (element instanceof IModelProvider) {
				//
				// The NED graphical editor publishes selection as an IStructuredSelection,
				// with editparts in it. INEDElement can be extracted from editparts
				// via IModelProvider.
				//
				INEDElement model = findFirstModuleOrSubmodule(((IModelProvider)element).getNEDModel());
				if (model != null ) {
					hideMessage();
					buildContent(model, null, null, null);
                } else
                    showMessage("No NED element selected.");
			}
			else if (element instanceof InifileSelectionItem) {
				//
				// The inifile editor publishes selection in InifileSelectionItems.
				//
				InifileSelectionItem sel = (InifileSelectionItem) element;
				InifileAnalyzer analyzer = sel.getAnalyzer();
				IInifileDocument doc = sel.getDocument();

				if (sel.getSection()==null) {
					showMessage("No section selected.");
					return;
				}
				String networkName = InifileUtils.lookupConfig(sel.getSection(), CFGID_NETWORK.getKey(), doc);
				if (StringUtils.isEmpty(networkName)) {
					showMessage("Network not specified (no network= setting in ["+sel.getSection()+"] or the sections it extends)");
					return;
				}
				IProject contextProject = doc.getDocumentFile().getProject();
				INEDTypeInfo networkType = NEDResourcesPlugin.getNEDResources().getToplevelNedType(networkName, contextProject);
                if (networkType == null) {
                    showMessage("No such NED network: "+networkName);
                    return;
                }

                hideMessage();
                buildContent(networkType.getNEDElement(), analyzer, sel.getSection(), sel.getKey());
			}
		}
		else {
			showMessage("No NED element or INI file entry selected.");
		}
	}

	/**
	 * Update view to display content that corresponds to the given module,
	 * with the specified inifile as configuration.
	 * @param module can be a toplevel type (Network, CompoundModule, SimpleModule), or Submodule
	 * @param ana Ini file analyzer
	 * @param key selected section
	 * @param section selected key
	 */
	protected abstract void buildContent(INEDElement module, InifileAnalyzer ana, String section, String key);


	/* stuff for subclasses: icons */
	public static final Image ICON_ERROR = InifileEditorPlugin.getCachedImage("icons/full/obj16/Error.png");
	public static final Image ICON_UNASSIGNEDPAR = InifileEditorPlugin.getCachedImage("icons/full/obj16/UnassignedPar.png");
	public static final Image ICON_NEDPAR = InifileEditorPlugin.getCachedImage("icons/full/obj16/NedPar.png");
	public static final Image ICON_NEDDEFAULTPAR = InifileEditorPlugin.getCachedImage("icons/full/obj16/IniPar.png"); //XXX
	public static final Image ICON_INIPAR = InifileEditorPlugin.getCachedImage("icons/full/obj16/IniPar.png");
	public static final Image ICON_INIPARREDUNDANT = InifileEditorPlugin.getCachedImage("icons/full/obj16/IniParRedundant.png");

	/**
	 * Helper function: suggests an icon for a table or tree entry.
	 */
	protected static Image suggestImage(ParamResolutionType type) {
		switch (type) {
			case UNASSIGNED: return ICON_UNASSIGNEDPAR;
			case NED: return ICON_NEDPAR;
			case NED_DEFAULT: return ICON_NEDDEFAULTPAR;
			case INI: return ICON_INIPAR;
			case INI_OVERRIDE:  return ICON_INIPAR;
			case INI_NEDDEFAULT: return ICON_INIPARREDUNDANT;
		}
		return null;
	}


	/* (non-Javadoc)
	 * @see org.eclipse.ui.part.IShowInTarget#show(org.eclipse.ui.part.ShowInContext)
	 */
	public boolean show(ShowInContext context) {
		return true;
	}
}
