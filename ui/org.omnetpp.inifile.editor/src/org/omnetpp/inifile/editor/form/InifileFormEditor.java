package org.omnetpp.inifile.editor.form;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.StructuredSelection;
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
import org.omnetpp.inifile.editor.TestSupport;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.KeyType;


/**
 * Represents the form page of the inifile multi-page editor.
 * 
 * IMPORTANT: to check whether all config keys are covered by
 * the form editor, set DUMP_FORGOTTEN_CONFIG_KEYS to true, 
 * launch the IDE, and open an ini file in the editor.
 * Then check the console window.
 * 
 * @author andras
 */
public class InifileFormEditor extends Composite {
	private static final boolean DUMP_FORGOTTEN_CONFIG_KEYS = false;

	public static final String SECTIONS_PAGE = "Sections";
	public static final String PARAMETERS_PAGE = "Parameters";
	public static final String CONFIGURATION_PAGE = "Configuration";

	public static final Color BGCOLOR = null;

	protected InifileEditor inifileEditor = null;  // backreference to the containing editor
	private TreeViewer treeViewer;
	private Composite formContainer;
	private FormPage formPage;

	public InifileFormEditor(Composite parent, InifileEditor inifileEditor) {
		super(parent, SWT.None);
		this.inifileEditor = inifileEditor;
		createControl();
	}

	public FormPage getActiveCategoryPage() {
		return formPage;
	}

	protected void createControl() {
		// create and layout a banner and a content area
		setBackground(BGCOLOR);
		setLayout(new GridLayout());
		SashForm sashForm = new SashForm(this, SWT.HORIZONTAL | SWT.SMOOTH);
		sashForm.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		sashForm.setBackground(BGCOLOR);

		treeViewer = createTreeViewer(sashForm);
		formContainer = new Composite(sashForm, SWT.BORDER);
		formContainer.setBackground(BGCOLOR);
		formContainer.setLayout(new FillLayout());
		sashForm.setWeights(new int[] {1,4});

        if (TestSupport.enableGuiTest)
            treeViewer.getTree().setData(TestSupport.WIDGET_ID, TestSupport.CATEGORY_TREE);

		buildTree();
	}

	protected TreeViewer createTreeViewer(Composite parent) {
		final TreeViewer viewer = new TreeViewer(parent, SWT.BORDER);
		addListener(viewer);
		viewer.setLabelProvider(new LabelProvider());
		viewer.setContentProvider(new GenericTreeContentProvider());
		return viewer;
	}

	protected void buildTree() {
		GenericTreeNode root = new GenericTreeNode("root");

		root.addChild(new GenericTreeNode(SECTIONS_PAGE));

		root.addChild(new GenericTreeNode(PARAMETERS_PAGE));

		GenericTreeNode configNode = new GenericTreeNode(CONFIGURATION_PAGE); 
		root.addChild(configNode);
		String[] categories = GenericConfigPage.getCategoryNames();
		for (String c : categories)
			configNode.addChild(new GenericTreeNode(c));

		treeViewer.setInput(root);
		treeViewer.expandAll();

		// at this point InifileDocument is not yet set up, so we have to defer showing the form page
		Display.getCurrent().asyncExec(new Runnable() {
			public void run() {
			    if (isDisposed()) 
			        return;
				if (DUMP_FORGOTTEN_CONFIG_KEYS)
					dumpForgottenConfigKeys(); // maintenance code
				IInifileDocument doc = inifileEditor.getEditorData().getInifileDocument();
				String initialPage = doc.getSectionNames().length >= 2 ? SECTIONS_PAGE : PARAMETERS_PAGE;
				showCategoryPage(initialPage);
			}
		});

	}

	protected void addListener(final TreeViewer treeViewer) {
		((Tree) treeViewer.getControl()).addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(final SelectionEvent event) {
				ISelection selection = treeViewer.getSelection();
				if (selection.isEmpty())
					return;
				Object sel = ((IStructuredSelection) selection).getFirstElement();
				String selected = (String) ((GenericTreeNode)sel).getPayload();
				showCategoryPage(selected);
			}

			@Override
			public void widgetDefaultSelected(SelectionEvent e) {
				formPage.setFocus();
			}
		});
	}

	/**
	 * Shows the form page belonging to the given category (i.e. tree node),
	 * after committing changes on the current page.
	 */
	public FormPage showCategoryPage(String category) {
		// root tree node is a shortcut to "General" 
		if (category.equals(CONFIGURATION_PAGE))
			category = GenericConfigPage.getCategoryNames()[0];

		// adjust tree selection (needed if we are invoked programmatically)
		Object sel = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
		String selectedCategory = sel==null ? null : (String) ((GenericTreeNode)sel).getPayload();
		if (!category.equals(selectedCategory))
			treeViewer.setSelection(new StructuredSelection(new GenericTreeNode(category)));

		// nothing to do if already showing
		if (formPage != null && formPage.getPageCategory().equals(category))
			return formPage; 

		// dispose old page
		if (formPage != null)
			formPage.dispose();

		// create new page
		if (category.equals(PARAMETERS_PAGE))
			formPage = new ParametersPage(formContainer, inifileEditor);
		else if (category.equals(SECTIONS_PAGE))
			formPage = new SectionsPage(formContainer, inifileEditor);
		else
			formPage = new GenericConfigPage(formContainer, category, inifileEditor);
		formContainer.layout();
		return formPage;
	}

	//XXX currently unused
	public void showStatusMessage(String message) {
		IWorkbenchWindow window = inifileEditor.getSite().getWorkbenchWindow();
		if (window instanceof ApplicationWindow)
			((ApplicationWindow)window).setStatus(message);
	}

	/**
	 * Notification: User switched to this page of the multipage editor (i.e. from text
	 * to form view).
	 */
	public void pageSelected() {
		if (formPage != null) 
			formPage.reread();
	}

	/**
	 * Notification: User switched away from this page of the multipage editor 
	 * (i.e. from form to text view).
	 */
	public void pageDeselected() {
		// nothing to do
	}

	public void gotoSection(String section) {
		showCategoryPage(SECTIONS_PAGE);
		formPage.gotoSection(section);
	}

	public void gotoEntry(String section, String key) {
		KeyType keyType = InifileAnalyzer.getKeyType(key);
		if (keyType==KeyType.PARAM)
			showCategoryPage(PARAMETERS_PAGE);
		else if (keyType==KeyType.PER_OBJECT_CONFIG)
			showCategoryPage(PARAMETERS_PAGE); //XXX for lack of anything better for now
		else 
			showCategoryPage(CONFIGURATION_PAGE); //XXX could make more effort to position the right field editor...
		formPage.gotoEntry(section, key);
	}

	/**
	 * Utility method, for source code maintenance. Prints the names
	 * of config keys that do not appear on any form page.
	 */
	public void dumpForgottenConfigKeys() {
		System.out.println("Checking if all config keys are covered by the forms...");

		// collect keys supported by the editor forms
		Set<ConfigKey> supportedKeys = new HashSet<ConfigKey>();
		
		List<String> categories = new ArrayList<String>();
		categories.add(PARAMETERS_PAGE);
		categories.add(SECTIONS_PAGE);
		categories.addAll(Arrays.asList(GenericConfigPage.getCategoryNames()));

		for (String category : categories) {
			FormPage page = showCategoryPage(category);
			for (ConfigKey key : page.getSupportedKeys())
				supportedKeys.add(key);
		}
		
		// see which keys are not supported anywhere
		for (ConfigKey key : ConfigRegistry.getEntries())
			if (!supportedKeys.contains(key))
				System.out.println(" - forgotten key: "+key.getKey());
        for (ConfigKey key : ConfigRegistry.getPerObjectEntries())
            if (!supportedKeys.contains(key))
                System.out.println(" - forgotten key: **."+key.getKey());

		System.out.println("Checking done.");
	}

}
