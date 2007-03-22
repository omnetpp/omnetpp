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
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.IWorkbenchWindow;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.ConfigurationRegistry;


/**
 * Common functionality for form-based pages of the inifile multi-page editor.
 * @author andras
 */
public class InifileEditorTreePage extends Composite {
	protected InifileEditor inifileEditor = null;  // backreference to the containing editor
	private TreeViewer treeViewer;
	private Composite form;
	private Color bgColor = new Color(null, 255, 255, 255);
	
	public InifileEditorTreePage(Composite parent, InifileEditor inifileEditor) {
		super(parent, SWT.None);
		this.inifileEditor = inifileEditor;
		createControl();
	}

	private void createControl() {
		// create and layout a banner and a content area
		setBackground(bgColor);
//		Composite bannerArea = new Composite(this, SWT.NONE);
		SashForm contentArea = new SashForm(this, SWT.HORIZONTAL | SWT.SMOOTH);
		setLayout(new GridLayout());
//		bannerArea.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		contentArea.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		contentArea.setBackground(bgColor);
		
//		// setup banner area
//		Label title = new Label(bannerArea, SWT.NONE);
//		bannerArea.setLayout(new FillLayout());
//		title.setText("Configuration Editor");
//		title.setFont(new Font(null, "Arial", 12, SWT.BOLD));
//		title.setBackground(ColorFactory.asColor("white"));
//		title.setForeground(new Color(null, 0, 128, 255));
		
		treeViewer = createTreeViewer(contentArea);
		form = new Composite(contentArea, SWT.V_SCROLL | SWT.BORDER);
		form.setBackground(bgColor);
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

	private void showCategoryPage(String category) {
		System.out.println("selected:" + category);
		for (Control c : form.getChildren())
			c.dispose();
		
		for (ConfigurationEntry e : ConfigurationRegistry.getEntries()) {
			if (e.getCategory().equals(category)) {
				Label label = new Label(form, SWT.NONE);
				//label.setLayoutData(new GridData(200,20));
				label.setBackground(bgColor);
				label.setText("["+e.getSection()+(e.isGlobal() ? "" : "] or [Run X")+"] "+e.getName());
				label.setToolTipText(StringUtils.breakLines(e.getDescription(),60));
				
				//System.out.println(label.getText());
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
	 * Notification about the selection of the page of the
	 * multipage editor.
	 */
	public void pageSelected() {
	}
}
