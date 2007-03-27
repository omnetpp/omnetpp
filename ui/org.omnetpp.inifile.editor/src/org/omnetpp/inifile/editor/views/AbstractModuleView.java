package org.omnetpp.inifile.editor.views;

import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.InifileUtils.Type;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDTreeUtil;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
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
public abstract class AbstractModuleView extends ViewWithMessagePart {
	private ISelectionListener selectionChangedListener;
	private IPartListener partListener;
	private INEDChangeListener nedChangeListener;
	
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
		
		// Listen on NED changes as well. 
		//FIXME this only fires when TOPLEVEL elements are added/removed! not on submodule, parameter etc changes.
		nedChangeListener = new INEDChangeListener() {
			public void modelChanged(NEDModelEvent event) {
				nedModelChanged();
			}
		};
		NEDResourcesPlugin.getNEDResources().getNEDResourceListenerList().add(nedChangeListener);
		
		//XXX should listen on changes in the inifile as well.
		
	}
	
	protected void unhookListeners() {
		if (selectionChangedListener != null)
			getSite().getPage().removePostSelectionListener(selectionChangedListener);
		if (partListener != null)
			getSite().getPage().removePartListener(partListener);
		if (nedChangeListener != null)
			NEDResourcesPlugin.getNEDResources().getNEDResourceListenerList().remove(nedChangeListener);
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
		rebuildContent();
		Display.getDefault().asyncExec(new Runnable() {
			public void run() {
				rebuildContent();
			}
		});
	}

	public void rebuildContent() {
		if (isDisposed())
			return;
		
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
		
		//System.out.println("SELECTION: "+selection);

		if (selection instanceof IStructuredSelection && !selection.isEmpty()) {
			// The NED graphical editor publishes selection as an IStructuredSelection,
			// with editparts in it. NEDElement can be extracted from editparts
			// via IModelProvider.
			Object element = ((IStructuredSelection)selection).getFirstElement();
			if (element instanceof IModelProvider) {
				Object model = ((IModelProvider)element).getModel();
				if (model instanceof CompoundModuleNode) {
					CompoundModuleNode node = (CompoundModuleNode)model;
					String moduleTypeName = node.getName();
					buildContent(moduleTypeName, null);
					hideMessage();
				}
				else if (model instanceof SimpleModuleNode) {
					SimpleModuleNode node = (SimpleModuleNode)model;
					String moduleTypeName = node.getName();
					buildContent(moduleTypeName, null);
					hideMessage();
				}
				else if (model instanceof SubmoduleNode) {
					SubmoduleNode submodule = (SubmoduleNode)model;
					String submoduleName = InifileUtils.getSubmoduleFullName(submodule);
					String submoduleType = InifileUtils.getSubmoduleType(submodule);
					buildContent(submoduleName, submoduleType, null);
					hideMessage();
				}
			}
			
		}
		else if (activeEditor instanceof InifileEditor) {
			InifileEditor inifileEditor = (InifileEditor) activeEditor;
			IInifileDocument doc = inifileEditor.getEditorData().getInifileDocument();

			//XXX consider changing the return type of NEDResourcesPlugin.getNEDResources() to INEDTypeResolver

			String networkName = doc.getValue("General", "network");
			if (networkName == null) {
				displayMessage("Network not specified (no [General]/network= setting)");
				return;
			}
			buildContent(networkName, doc);
			hideMessage();
		}
		else {
			displayMessage("Please open an inifile or NED editor.");
		}
	}

	/**
	 * Delegates to the other buildContent() method. 
	 */
	protected void buildContent(String moduleTypeName, IInifileDocument doc) {
		buildContent(moduleTypeName, moduleTypeName, doc);
	}

	/**
	 * Update view to display content that corresponds to the given module, 
	 * with the specified inifile as configuration. 
	 */
	protected abstract void buildContent(String submoduleName, String submoduleType, IInifileDocument doc);

	/**
	 * Helper function: suggests an icon for a table or tree entry.
	 */
	protected static Image suggestImage(Type type, NEDElement element) {
		switch (type) {
		//FIXME better icons!
		case ERROR: return ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_ERROR); //XXX
		case UNASSIGNED_PAR: return ImageFactory.getImage(ImageFactory.MODEL_IMAGE_INVALID); //XXX
		case INI_PAR: return InifileEditorPlugin.getImageDescriptor("icons/inifile.png").createImage(); //XXX cache it
		case NED_PAR: return InifileEditorPlugin.getImageDescriptor("icons/moduletreeview.png").createImage(); //XXX cache it
		case INI_PAR_REDUNDANT: return ImageFactory.getImage(ImageFactory.DECORATOR_IMAGE_WARNING); //XXX
		case OTHER: if (element!=null) return NEDTreeUtil.getNedModelLabelProvider().getImage(element);
		}
		return null;
	}

}
