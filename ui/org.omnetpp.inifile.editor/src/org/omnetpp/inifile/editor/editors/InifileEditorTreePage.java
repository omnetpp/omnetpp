package org.omnetpp.inifile.editor.editors;

import java.util.ArrayList;

import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.window.ApplicationWindow;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.ConfigurationRegistry;


/**
 * Common functionality for form-based pages of the inifile multi-page editor.
 * @author andras
 */
public class InifileEditorTreePage extends Composite {
	protected InifileEditor inifileEditor = null;  // backreference to the containing editor
	private TreeViewer treeViewer;
	private ScrolledForm form;
	
	public InifileEditorTreePage(Composite parent, InifileEditor inifileEditor) {
		super(parent, SWT.None);
		this.inifileEditor = inifileEditor;
		createControl();
	}

	private void createControl() {
		// create and layout a banner and a content area
		setBackground(ColorFactory.asColor("white"));
//		Composite bannerArea = new Composite(this, SWT.NONE);
		SashForm contentArea = new SashForm(this, SWT.HORIZONTAL | SWT.SMOOTH);
		setLayout(new GridLayout());
//		bannerArea.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		contentArea.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		contentArea.setBackground(ColorFactory.asColor("white"));
		
//		// setup banner area
//		Label title = new Label(bannerArea, SWT.NONE);
//		bannerArea.setLayout(new FillLayout());
//		title.setText("Configuration Editor");
//		title.setFont(new Font(null, "Arial", 12, SWT.BOLD));
//		title.setBackground(ColorFactory.asColor("white"));
//		title.setForeground(new Color(null, 0, 128, 255));
		
		treeViewer = createTreeViewer(contentArea);
		form = new ScrolledForm(contentArea, SWT.V_SCROLL | SWT.BORDER);
		form.setBackground(ColorFactory.asColor("white"));
		contentArea.setWeights(new int[] {1,3});
		
		buildTree();
	}

	private void buildTree() {
		GenericTreeNode root = new GenericTreeNode("root");
		String[] categories = ConfigurationRegistry.getCategories();
		for (String c : categories)
			root.addChild(new GenericTreeNode(c));
		treeViewer.setInput(root);
	}
	
	protected TreeViewer createTreeViewer(Composite parent) {
		final TreeViewer viewer = new TreeViewer(parent, SWT.BORDER);
		//XXX addListeners(viewer);
		viewer.setLabelProvider(new LabelProvider());
		viewer.setContentProvider(new GenericTreeContentProvider());
		return viewer;
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
