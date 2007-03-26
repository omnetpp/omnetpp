package org.omnetpp.inifile.editor.views;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.SimpleModuleNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * View.
 * 
 * @author Andras
 */
public abstract class AbstractModuleView extends ViewPart {
	private ISelectionListener selectionChangedListener;
	private Label messageLabel;
	
	public AbstractModuleView() {
	}


	@Override
	public void dispose() {
		unhookSelectionChangedListener();
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
	
	protected void hookSelectionChangedListener() {
		selectionChangedListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				workbenchSelectionChanged(part, selection);
			}
		};
		getSite().getPage().addPostSelectionListener(selectionChangedListener);
	}
	
	private void unhookSelectionChangedListener() {
		if (selectionChangedListener != null)
			getSite().getPage().removePostSelectionListener(selectionChangedListener);
	}
	
	public void workbenchSelectionChanged(IWorkbenchPart part, ISelection selection) {
		if (!(part instanceof IEditorPart))
			return; // ignore selection from views
			
		IEditorPart activeEditor = (IEditorPart)part;
		//IEditorPart activeEditor = getSite().getWorkbenchWindow().getActivePage().getActiveEditor(); //XXX this may crash on startup (NPE)!!!

		System.out.println("SELECTION: "+selection); //XXX
		ISelection editorSel = activeEditor.getSite().getSelectionProvider().getSelection();
		System.out.println("   EDITOR: "+editorSel); //XXX

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
