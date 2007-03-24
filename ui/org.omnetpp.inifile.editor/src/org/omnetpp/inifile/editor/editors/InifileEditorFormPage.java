package org.omnetpp.inifile.editor.editors;

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
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.IWorkbenchWindow;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.ConfigurationRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;


/**
 * Common functionality for form-based pages of the inifile multi-page editor.
 * @author andras
 */
public class InifileEditorFormPage extends Composite {
	public static final Color BGCOLOR = null; // or: ColorFactory.asColor("white");

	protected InifileEditor inifileEditor = null;  // backreference to the containing editor
	private TreeViewer treeViewer;
	private Composite form;
	
	public InifileEditorFormPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, SWT.None);
		this.inifileEditor = inifileEditor;
		createControl();
	}

	private void createControl() {
		// create and layout a banner and a content area
		setBackground(BGCOLOR);
		SashForm contentArea = new SashForm(this, SWT.HORIZONTAL | SWT.SMOOTH);
		setLayout(new GridLayout());
		contentArea.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		contentArea.setBackground(BGCOLOR);
		
		treeViewer = createTreeViewer(contentArea);
		form = new Composite(contentArea, SWT.V_SCROLL | SWT.BORDER);
		form.setBackground(BGCOLOR);
		form.setLayout(new GridLayout());
		contentArea.setWeights(new int[] {1,3});
		
		buildTree();
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
		String[] categories = ConfigurationRegistry.getCategories();
		for (String c : categories)
			root.addChild(new GenericTreeNode(c));
		treeViewer.setInput(root);
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
	 * Writes the edits on the current page back into the text editor.
	 */
	public void commitCurrentPage() {
		System.out.println("form page: committing current page");
		for (Control c : form.getChildren())  //XXX temp code
			if (c instanceof FieldEditor)
				((FieldEditor) c).commit();
	}
	
	/**
	 * Shows the form page belonging to the given category (i.e. tree node),
	 * after committing changes on the current page.
	 */
	public void showCategoryPage(String category) {
		System.out.println("selected:" + category);
		
		commitCurrentPage();

		//XXX temp: remove old widgets
		for (Control c : form.getChildren())
			c.dispose();
		
		IInifileDocument doc = inifileEditor.getEditorData().getInifileDocument();
		for (ConfigurationEntry e : ConfigurationRegistry.getEntries()) {
			if (e.getCategory().equals(category)) {
				//String label = "["+e.getSection()+(e.isGlobal() ? "" : "] or [Run X")+"] "+e.getName();
				String label = "The \""+e.getName()+"\" setting";
				if (e.getType()==ConfigurationEntry.Type.CFG_BOOL) {
					CheckboxFieldEditor control = new CheckboxFieldEditor(form, SWT.NONE, e, doc, label);
					control.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
				}
				else {
					TextFieldEditor control = new TextFieldEditor(form, SWT.NONE, e, doc, label);
					control.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
				}
			}
		}
		form.layout();
	}

	public void showStatusMessage(String message) {
		IWorkbenchWindow window = inifileEditor.getSite().getWorkbenchWindow();
		if (window instanceof ApplicationWindow)
			((ApplicationWindow)window).setStatus(message);
	}
	
	/**
	 * Notification: User switched to this page of the multipage editor.
	 */
	public void pageSelected() {
		commitCurrentPage();
	}
	
	/**
	 * Notification: User switched away from this page of the multipage editor.
	 */
	public void pageDeselected() {
		commitCurrentPage();
	}
}
