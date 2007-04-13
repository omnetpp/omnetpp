package org.omnetpp.inifile.editor.form;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.window.ApplicationWindow;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.IWorkbenchWindow;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.inifile.editor.editors.InifileEditor;


/**
 * Common functionality for form-based pages of the inifile multi-page editor.
 * @author andras
 */
public class InifileFormEditor extends Composite {
	private static final String TREELABEL_SECTIONS = "Sections";
	private static final String TREELABEL_PARAMETERS = "Parameters";
	private static final String TREELABEL_CONFIGURATION = "Configuration";

	public static final Color BGCOLOR = null; // or: ColorFactory.asColor("white");

	protected InifileEditor inifileEditor = null;  // backreference to the containing editor
	private TreeViewer treeViewer;
	private Composite form;
	private FormPage formPage;
	
	public InifileFormEditor(Composite parent, InifileEditor inifileEditor) {
		super(parent, SWT.None);
		this.inifileEditor = inifileEditor;
		createControl();
	}

	private void createControl() {
		// create and layout a banner and a content area
		setBackground(BGCOLOR);
		setLayout(new GridLayout());
		SashForm sashForm = new SashForm(this, SWT.HORIZONTAL | SWT.SMOOTH);
		sashForm.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		sashForm.setBackground(BGCOLOR);
		
		treeViewer = createTreeViewer(sashForm);
		form = new Composite(sashForm, SWT.BORDER);  //XXX try to use ScrolledComposite
		form.setBackground(BGCOLOR);
		form.setLayout(new FillLayout());
		sashForm.setWeights(new int[] {1,4});
		
		buildTree();

		Display.getCurrent().asyncExec(new Runnable() {
			public void run() {
				showCategoryPage(GenericConfigPage.getCategoryNames()[0]);
			}
	});
}

	protected TreeViewer createTreeViewer(Composite parent) {
		final TreeViewer viewer = new TreeViewer(parent, SWT.BORDER);
		addListener(viewer);
		viewer.setLabelProvider(new LabelProvider());
		viewer.setContentProvider(new GenericTreeContentProvider());
		return viewer;
	}

	private void buildTree() {
		GenericTreeNode root = new GenericTreeNode("root");
		GenericTreeNode configNode = new GenericTreeNode(TREELABEL_CONFIGURATION); 
		root.addChild(configNode);

		String[] categories = GenericConfigPage.getCategoryNames();
		for (String c : categories)
			configNode.addChild(new GenericTreeNode(c));
		root.addChild(new GenericTreeNode(TREELABEL_SECTIONS));
		root.addChild(new GenericTreeNode(TREELABEL_PARAMETERS));
		treeViewer.setInput(root);
		treeViewer.expandAll();
	}
	
	private void addListener(final TreeViewer treeViewer) {
		((Tree) treeViewer.getControl()).addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(final SelectionEvent event) {
				ISelection selection = treeViewer.getSelection();
				if (selection.isEmpty())
					return;
				Object sel = ((IStructuredSelection) selection).getFirstElement();
				String selected = (String) ((GenericTreeNode)sel).getPayload();
				showCategoryPage(selected);
			}
		});
	}

	/**
	 * Shows the form page belonging to the given category (i.e. tree node),
	 * after committing changes on the current page.
	 */
	public void showCategoryPage(String category) {
		
		if (formPage != null) {
			formPage.commit();
			formPage.dispose();
		}

		if (category.equals(TREELABEL_CONFIGURATION))
			category = GenericConfigPage.getCategoryNames()[0];
			
		if (category.equals(TREELABEL_PARAMETERS))
			formPage = new ParametersPage(form, inifileEditor);
		else if (category.equals(TREELABEL_SECTIONS))
			formPage = new SectionsPage(form, inifileEditor);
		else
			formPage = new GenericConfigPage(form, category, inifileEditor);
		form.layout();
	}

	//XXX currently unused
	public void showStatusMessage(String message) {
		IWorkbenchWindow window = inifileEditor.getSite().getWorkbenchWindow();
		if (window instanceof ApplicationWindow)
			((ApplicationWindow)window).setStatus(message);
	}
	
	/**
	 * Notification: User switched to this page of the multipage editor.
	 */
	public void pageSelected() {
		if (formPage != null) 
			formPage.reread();
	}
	
	/**
	 * Notification: User switched away from this page of the multipage editor.
	 */
	public void pageDeselected() {
		if (formPage != null) 
			formPage.commit();
	}
}
