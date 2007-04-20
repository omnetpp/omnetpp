package org.omnetpp.inifile.editor.views;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;

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
import org.omnetpp.inifile.editor.editors.InifileSelectionItem;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.ParamResolution.ParamResolutionType;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned.model.pojo.SimpleModuleNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * Abstract base class for views that display information based on a single NED
 * module or network type, and possibly an inifile. Subclasses are expected to 
 * implement the buildContent() method, which will be invoked whenever the 
 * selection changes, or there is a change in NED or ini files.
 * 
 * @author Andras
 */
public abstract class AbstractModuleView extends ViewWithMessagePart implements IShowInTarget {
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
		NEDResourcesPlugin.getNEDResources().getNEDModelChangeListenerList().add(nedChangeListener);
	}
	
	protected void unhookListeners() {
		if (selectionChangedListener != null)
			getSite().getPage().removePostSelectionListener(selectionChangedListener);
        if (selectionChangedListener != null)
            getSite().getPage().removeSelectionListener(selectionChangedListener);
		if (partListener != null)
			getSite().getPage().removePartListener(partListener);
		if (nedChangeListener != null)
			NEDResourcesPlugin.getNEDResources().getNEDModelChangeListenerList().remove(nedChangeListener);
	}

	public void workbenchSelectionChanged() {
		System.out.println("*** SELECTIONCHANGE");
		scheduleRebuildContent();
	}

	protected void viewActivated() {
		System.out.println("*** VIEW ACTIVATED");
		scheduleRebuildContent();
	}

	protected void activeEditorChanged() {
		System.out.println("*** ACTIVE EDITOR CHANGED");
		scheduleRebuildContent();
	}

	protected void nedModelChanged() {
		System.out.println("*** NED MODEL CHANGE");
		scheduleRebuildContent();
	}

	public void scheduleRebuildContent() {
        rebuildContentJob.restartTimer();
	}

    /**
     * @return Tries to find a ned element up in the ancestor chanin which may have 
     * parameters. ie. simple and submodule, compoundmodule and channel
     */
    private NEDElement findFirstModuleOrSubmodule(NEDElement element) {
        while (element != null) {
            if (element instanceof CompoundModuleNode || element instanceof SimpleModuleNode ||
                    element instanceof SubmoduleNode || element instanceof ModuleInterfaceNode)
                return element;
            element = element.getParent();
        }
        return element;
    }
    
	public void rebuildContent() {
		if (isDisposed())
			return;
		
        System.out.println("*** CONTENT REBUILD");
		IEditorPart activeEditor = getActiveEditor();
		if (activeEditor==null) {
			displayMessage("There is no active editor.");
			return;
		}

		ISelection selection = getActiveEditorSelection();
		if (selection==null) {
			displayMessage("Nothing is selected.");
			return;
		}
		
		if (selection instanceof IStructuredSelection && !selection.isEmpty()) {
			Object element = ((IStructuredSelection)selection).getFirstElement();
			if (element instanceof IModelProvider) {
				//
				// The NED graphical editor publishes selection as an IStructuredSelection,
				// with editparts in it. NEDElement can be extracted from editparts
				// via IModelProvider.
				//
				NEDElement model = findFirstModuleOrSubmodule(((IModelProvider)element).getNEDModel());
				if (model != null ) {
					buildContent(model, null, null, null);
					hideMessage();
                } else
                    displayMessage("No NED element selected.");
			}
			else if (element instanceof InifileSelectionItem) {
				//
				// The inifile editor publishes selection in InifileSelectionItems.
				//
				InifileSelectionItem sel = (InifileSelectionItem) element;
				InifileAnalyzer analyzer = sel.getAnalyzer();
				IInifileDocument doc = sel.getDocument();

				if (sel.getSection()==null) {
					displayMessage("No section selected.");
					return;
				}
				String networkName = InifileUtils.lookupConfig(sel.getSection(), CFGID_NETWORK.getKey(), doc);
				if (networkName == null) {
					displayMessage("Network not specified (no network= setting in ["+sel.getSection()+"] or the sections it extends)");
					return;
				}
                INEDTypeInfo networkType = NEDResourcesPlugin.getNEDResources().getComponent(networkName);
                if (networkType == null) {
                    displayMessage("Unknown module type specified for network: "+networkName);
                    return;
                }
                buildContent(networkType.getNEDElement(), analyzer, sel.getSection(), sel.getKey());
				hideMessage();
			}
		}
		else {
			displayMessage("No NED element or INI file entry selected.");
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
	protected abstract void buildContent(NEDElement module, InifileAnalyzer ana, String section, String key);


	/* stuff for subclasses: icons */
	public static final Image ICON_ERROR = InifileEditorPlugin.getImage("icons/full/obj16/Error.png");
	public static final Image ICON_UNASSIGNEDPAR = InifileEditorPlugin.getImage("icons/full/obj16/UnassignedPar.png");
	public static final Image ICON_NEDPAR = InifileEditorPlugin.getImage("icons/full/obj16/NedPar.png");
	public static final Image ICON_NEDDEFAULTPAR = InifileEditorPlugin.getImage("icons/full/obj16/IniPar.png"); //XXX
	public static final Image ICON_INIPAR = InifileEditorPlugin.getImage("icons/full/obj16/IniPar.png");
	public static final Image ICON_INIPARREDUNDANT = InifileEditorPlugin.getImage("icons/full/obj16/IniParRedundant.png");
	
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

	protected static String[] getValueAndRemark(ParamResolution res, IInifileDocument doc) {
		// value in the NED file
		String nedValue = res.paramValueNode.getValue(); //XXX what if parsed expressions?
		if (StringUtils.isEmpty(nedValue)) 
			nedValue = null;
	
		// look up its value in the ini file
		String iniValue = null;
		if (doc != null && res.key != null)
			iniValue = doc.getValue(res.section, res.key);
	
		String value;
		String remark;
		switch (res.type) {
			case UNASSIGNED: value = null; remark = "unassigned"; break;
			case NED: value = nedValue; remark = "NED"; break;  
			case NED_DEFAULT: value = nedValue; remark = "NED default applied"; break;
			case INI: value = iniValue; remark = "ini"; break;
			case INI_OVERRIDE: value = iniValue; remark = "ini, overrides NED default: "+nedValue; break;
			case INI_NEDDEFAULT: value = nedValue; remark = "ini, sets same value as NED default"; break;
			default: throw new IllegalStateException("invalid param resolution type: "+res.type);
		}
		if (res.key!=null) remark += "; see ["+res.section+"] / " + res.key + "=" + iniValue;
		return new String[] {value, remark}; 
	}

	/* (non-Javadoc)
	 * @see org.eclipse.ui.part.IShowInTarget#show(org.eclipse.ui.part.ShowInContext)
	 */
	public boolean show(ShowInContext context) {
		return true;
	}
}
