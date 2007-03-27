package org.omnetpp.inifile.editor.views;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
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
public abstract class AbstractModuleView extends ViewPart {
	private ISelectionListener selectionChangedListener;
	private IPartListener partListener;
	private Label messageLabel;
	private INEDChangeListener nedChangeListener;
	
	public AbstractModuleView() {
	}


	@Override
	public void dispose() {
		unhookListeners();
		super.dispose();
	}

	/**
	 * Return the part control used to display the contents of the view. This will
	 * typically be a Tree or a Table.
	 */
	protected abstract Control getPartControl();

	@Override
	public void setFocus() {
		getPartControl().setFocus();
	}

	/**
	 * Display a message (such as "Nothing to show") instead of the contents.
	 */
	protected void displayMessage(String text) {
		if (messageLabel==null) {
			messageLabel = new Label(getPartControl().getParent(), SWT.WRAP);
			messageLabel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		}
		messageLabel.setText(text);
		setVisible(messageLabel, true);
		setVisible(getPartControl(), false);
	}

	/**
	 * Displays the part control.
	 */
	protected void hideMessage() {
		if (!getPartControl().isVisible()) {
			if (messageLabel != null) setVisible(messageLabel, false);
			setVisible(getPartControl(), true);
		}
	}

	protected void setVisible(Control control, boolean visible) {
		Assert.isTrue(control.getParent().getLayout() instanceof GridLayout); // if not, set it!
		GridData gridData = (GridData)control.getLayoutData();
		if (gridData == null) {
			gridData = new GridData();
			control.setLayoutData(gridData);
		}
		gridData.exclude = !visible;
		control.setVisible(visible);
		control.getParent().layout(true, true);
	}
	
	protected void hookListeners() {
		// Listen on selection changes
		selectionChangedListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				workbenchSelectionChanged(part, selection);
			}
		};
		getSite().getPage().addPostSelectionListener(selectionChangedListener);
		
		// Listens to workbench changes, and invokes activeEditorChanged() whenever the 
		// active editor changes. Listening on workbench changes is needed because 
		// editor don't always send selection changes when they get opened or closed.
		partListener = new IPartListener() {
			private IEditorPart activeEditor = null;
			
			public void partActivated(IWorkbenchPart part) {
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
		
	}
	
	protected void unhookListeners() {
		if (selectionChangedListener != null)
			getSite().getPage().removePostSelectionListener(selectionChangedListener);
		if (partListener != null)
			getSite().getPage().removePartListener(partListener);
		if (nedChangeListener != null)
			NEDResourcesPlugin.getNEDResources().getNEDResourceListenerList().remove(nedChangeListener);
	}

	protected void activeEditorChanged() {
		//System.out.println("************ "+activeEditor);
		// TODO Auto-generated method stub
		
	}

	protected void nedModelChanged() {
		// TODO Auto-generated method stub
		System.out.println("%%%%%%%%%%%%%% NED MODEL CHANGE");
	}
	
	public void workbenchSelectionChanged(IWorkbenchPart part, ISelection selection) {  //XXX remove params
			
		IWorkbenchPage activePage = getSite().getWorkbenchWindow().getActivePage();
		IEditorPart activeEditor = activePage==null ? null : activePage.getActiveEditor();
		if (activeEditor==null) {
			displayMessage("There is no active editor.");
			return;
		}

		ISelectionProvider selectionProvider = activeEditor.getSite().getSelectionProvider();
		selection = selectionProvider==null ? null : selectionProvider.getSelection();
		
		System.out.println("SELECTION: "+selection); //XXX

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
			displayMessage(activeEditor==null ? "No editor is open." : "Editor is not an inifile editor.");
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

}
