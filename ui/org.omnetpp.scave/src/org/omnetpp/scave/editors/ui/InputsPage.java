package org.omnetpp.scave.editors.ui;

import static org.omnetpp.scave.TestSupport.FILE_RUN_VIEW_TREE_ID;
import static org.omnetpp.scave.TestSupport.INPUT_FILES_TREE;
import static org.omnetpp.scave.TestSupport.LOGICAL_VIEW_TREE_ID;
import static org.omnetpp.scave.TestSupport.RUN_FILE_VIEW_TREE_ID;
import static org.omnetpp.scave.TestSupport.WIDGET_ID;
import static org.omnetpp.scave.TestSupport.enableGuiTest;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.forms.widgets.ExpandableComposite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.Section;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.CustomSashForm;
import org.omnetpp.scave.actions.AddResultFileAction;
import org.omnetpp.scave.actions.AddWildcardResultFileAction;
import org.omnetpp.scave.actions.EditAction;
import org.omnetpp.scave.actions.RemoveAction;
import org.omnetpp.scave.actions.SetFilterAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.treeproviders.InputsFileRunViewContentProvider;
import org.omnetpp.scave.editors.treeproviders.InputsLogicalViewContentProvider;
import org.omnetpp.scave.editors.treeproviders.InputsRunFileViewContentProvider;
import org.omnetpp.scave.editors.treeproviders.InputsViewLabelProvider;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;

public class InputsPage extends ScaveEditorPage {

	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	private Section inputFilesSection = null;
	private Section dataSection = null;
	private SashForm sashform = null;
	
	private Runnable scheduledUpdate;

	public InputsPage(Composite parent, ScaveEditor scaveEditor) {
		super(parent, SWT.V_SCROLL, scaveEditor);
		initialize();
	}
	
	public TreeViewer getInputFilesTreeViewer() {
		InputFilesPanel panel = (InputFilesPanel)inputFilesSection.getClient();
		return panel.getTreeViewer();
	}
	
	public TreeViewer getFileRunTreeViewer() {
		DataPanel panel = (DataPanel)dataSection.getClient();
		return panel.getFileRunTreeViewer();
	}
	
	public TreeViewer getRunFileTreeViewer() {
		DataPanel panel = (DataPanel)dataSection.getClient();
		return panel.getRunFileTreeViewer();
	}
	
	public TreeViewer getLogicalDataTreeViewer() {
		DataPanel panel = (DataPanel)dataSection.getClient();
		return panel.getLogicalTreeViewer();
	}
	
	private void initialize() {
		// set up UI
		setPageTitle("Inputs"); 
		setFormTitle("Inputs");
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.WHITE);
		getBody().setLayout(new GridLayout());
		createSashForm();
		createInputFilesSection();
		createDataSection();

		// configure viewers
        scaveEditor.configureTreeViewer(getInputFilesTreeViewer());
        
        getFileRunTreeViewer().setContentProvider(new InputsFileRunViewContentProvider());
        getFileRunTreeViewer().setLabelProvider(new InputsViewLabelProvider());

        getRunFileTreeViewer().setContentProvider(new InputsRunFileViewContentProvider());
        getRunFileTreeViewer().setLabelProvider(new InputsViewLabelProvider());
        
        getLogicalDataTreeViewer().setContentProvider(new InputsLogicalViewContentProvider());
        getLogicalDataTreeViewer().setLabelProvider(new InputsViewLabelProvider());

        scaveEditor.getResultFileManager().addChangeListener(new IResultFilesChangeListener() {
			public void resultFileManagerChanged(final ResultFileManager manager) {
				if (scheduledUpdate == null) {
					scheduledUpdate = new Runnable() {
						public void run() {
							scheduledUpdate = null;
							if (!isDisposed()) {
								getFileRunTreeViewer().setInput(manager); // force refresh
								getRunFileTreeViewer().setInput(manager);
								getLogicalDataTreeViewer().setInput(manager);
							}
						}
					};
					getDisplay().asyncExec(scheduledUpdate);
				}
			}
        });
        
        getFileRunTreeViewer().addSelectionChangedListener(scaveEditor.getSelectionChangedListener());
        getRunFileTreeViewer().addSelectionChangedListener(scaveEditor.getSelectionChangedListener());
        getLogicalDataTreeViewer().addSelectionChangedListener(scaveEditor.getSelectionChangedListener());
        
        // set up drag & drop of .sca and .vec files into the viewers
        setupResultFileDropTarget(getInputFilesTreeViewer().getControl());
        setupResultFileDropTarget(getFileRunTreeViewer().getControl());
        setupResultFileDropTarget(getRunFileTreeViewer().getControl());
        setupResultFileDropTarget(getLogicalDataTreeViewer().getControl());

        // set contents
		Analysis analysis = scaveEditor.getAnalysis();
        getInputFilesTreeViewer().setInput(analysis.getInputs());

        ResultFileManagerEx manager = scaveEditor.getResultFileManager();
        getFileRunTreeViewer().setInput(manager);
        getRunFileTreeViewer().setInput(manager);
        getLogicalDataTreeViewer().setInput(manager);
        
        if (enableGuiTest) {
        	getInputFilesTreeViewer().getTree().setData(WIDGET_ID, INPUT_FILES_TREE);
        	getFileRunTreeViewer().getTree().setData(WIDGET_ID, FILE_RUN_VIEW_TREE_ID);
        	getRunFileTreeViewer().getTree().setData(WIDGET_ID, RUN_FILE_VIEW_TREE_ID);
        	getLogicalDataTreeViewer().getTree().setData(WIDGET_ID, LOGICAL_VIEW_TREE_ID);
        }
	}

	/**
	 * This method initializes formToolkit	
	 * 	
	 * @return org.eclipse.ui.forms.widgets.FormToolkit	
	 */
	private FormToolkit getFormToolkit() {
		if (formToolkit == null)
			formToolkit = new FormToolkit2(Display.getCurrent());
		return formToolkit;
	}

	private void createSashForm() {
		sashform = new CustomSashForm(getBody(), SWT.VERTICAL | SWT.SMOOTH);
		getFormToolkit().adapt(sashform);
		//sashform.setBackground(this.getBackground());
		sashform.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL |
											GridData.GRAB_VERTICAL |
											GridData.FILL_BOTH));
	}
	
	/**
	 * This method initializes inputFilesSection	
	 */
	private void createInputFilesSection() {
		inputFilesSection = getFormToolkit().createSection(sashform,
				Section.DESCRIPTION | ExpandableComposite.TITLE_BAR);
		inputFilesSection.setLayoutData(new GridData(GridData.FILL, GridData.FILL, true, true));
		inputFilesSection.setText("Input files");
		inputFilesSection.setDescription("Add or drag & drop result files (*.sca or *.vec) that should be used in this analysis. Wildcards (*,?) can also be used to specify multiple files.");
		inputFilesSection.setExpanded(true);
		InputFilesPanel inputFilesPanel = new InputFilesPanel(inputFilesSection, SWT.NONE);
		inputFilesSection.setClient(inputFilesPanel);

		// buttons
		configureViewerButton(
				inputFilesPanel.getAddFileButton(), 
				inputFilesPanel.getTreeViewer(),
				new AddResultFileAction());
		configureViewerButton(
				inputFilesPanel.getAddWildcardButton(), 
				inputFilesPanel.getTreeViewer(),
				new AddWildcardResultFileAction());
		configureViewerButton(
				inputFilesPanel.getEditButton(),
				inputFilesPanel.getTreeViewer(),
				new EditAction());
		configureViewerButton(
				inputFilesPanel.getRemoveFileButton(), 
				inputFilesPanel.getTreeViewer(),
				new RemoveAction());
	}

	/**
	 * This method initializes dataSection	
	 */
	private void createDataSection() {
		dataSection = getFormToolkit().createSection(sashform, 
				Section.DESCRIPTION | ExpandableComposite.TITLE_BAR);
		dataSection.setLayoutData(new GridData(GridData.FILL, GridData.FILL, true, true));
		dataSection.setText("Data");
		dataSection.setDescription("Here you can browse the result files and their contents.");
		//dataSection.setExpanded(true); XXX SWT bug: must be after setText() if present, otherwise text won't appear!
		DataPanel dataPanel = new DataPanel(dataSection, SWT.NONE);
		dataSection.setClient(dataPanel);

		// double-clicks
		configureViewerDefaultAction(
				dataPanel.getFileRunTreeViewer(),
				new SetFilterAction());
		configureViewerDefaultAction(
				dataPanel.getRunFileTreeViewer(),
				new SetFilterAction());
		configureViewerDefaultAction(
				dataPanel.getLogicalTreeViewer(),
				new SetFilterAction());
	}

	@Override
	public boolean gotoObject(Object object) {
		if (object instanceof InputFile) {
			TreeViewer viewer = getInputFilesTreeViewer();
			viewer.reveal(object);
			return true;
		}
		else if (object instanceof Inputs) {
			return true;
		}
			
		return false;
	}

	@Override
	public void selectionChanged(ISelection selection) {
		setViewerSelectionNoNotify(getInputFilesTreeViewer(), selection);
	}
}
