package org.omnetpp.scave2.editors.ui;

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
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave2.actions.AddResultFileAction;
import org.omnetpp.scave2.actions.AddWildcardResultFileAction;
import org.omnetpp.scave2.actions.RemoveAction;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.treeproviders.CachedTreeContentProvider;
import org.omnetpp.scave2.editors.treeproviders.InputsLogicalViewContentProvider;
import org.omnetpp.scave2.editors.treeproviders.InputsLogicalViewLabelProvider;
import org.omnetpp.scave2.editors.treeproviders.InputsPhysicalViewContentProvider;
import org.omnetpp.scave2.editors.treeproviders.InputsPhysicalViewLabelProvider;

public class InputsPage extends ScaveEditorPage {

	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	private Section inputFilesSection = null;
	private Section dataSection = null;
	private SashForm sashform = null;

	public InputsPage(Composite parent, ScaveEditor scaveEditor) {
		super(parent, SWT.V_SCROLL, scaveEditor);
		initialize();
	}
	
	public TreeViewer getInputFilesTreeViewer() {
		InputFilesPanel panel = (InputFilesPanel)inputFilesSection.getClient();
		return panel.getTreeViewer();
	}
	
	public TreeViewer getPhysicalDataTreeViewer() {
		DataPanel panel = (DataPanel)dataSection.getClient();
		return panel.getPhysicalTreeViewer();
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
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout());
		createSashForm();
		createInputFilesSection();
		createDataSection();

		// configure viewers
        scaveEditor.configureTreeViewer(getInputFilesTreeViewer());
        
        getPhysicalDataTreeViewer().setContentProvider(new InputsPhysicalViewContentProvider());
        getPhysicalDataTreeViewer().setLabelProvider(new InputsPhysicalViewLabelProvider());

        getLogicalDataTreeViewer().setContentProvider(new InputsLogicalViewContentProvider());
        getLogicalDataTreeViewer().setLabelProvider(new InputsLogicalViewLabelProvider());

        scaveEditor.getResultFileManager().addListener(new IResultFilesChangeListener() {
			public void resultFileManagerChanged(ResultFileManager manager) {
				getPhysicalDataTreeViewer().setInput(manager); // force refresh
				getLogicalDataTreeViewer().setInput(manager);
			}
        });
        
        getPhysicalDataTreeViewer().addSelectionChangedListener(scaveEditor.getSelectionChangedListener());
        getLogicalDataTreeViewer().addSelectionChangedListener(scaveEditor.getSelectionChangedListener());
        
        // set up drag & drop of .sca and .vec files into the viewers
        setupResultFileDropTarget(getInputFilesTreeViewer().getControl());
        setupResultFileDropTarget(getPhysicalDataTreeViewer().getControl());
        setupResultFileDropTarget(getLogicalDataTreeViewer().getControl());

        // set contents
		Analysis analysis = scaveEditor.getAnalysis();
        getInputFilesTreeViewer().setInput(analysis.getInputs());

        ResultFileManagerEx manager = scaveEditor.getResultFileManager();
        getPhysicalDataTreeViewer().setInput(manager);
        getLogicalDataTreeViewer().setInput(manager);
	}

	/**
	 * This method initializes formToolkit	
	 * 	
	 * @return org.eclipse.ui.forms.widgets.FormToolkit	
	 */
	private FormToolkit getFormToolkit() {
		if (formToolkit == null) {
			formToolkit = new FormToolkit(Display.getCurrent());
		}
		return formToolkit;
	}

	private void createSashForm() {
		sashform = new SashForm(getBody(), SWT.VERTICAL | SWT.SMOOTH);
		sashform.setBackground(this.getBackground());
		sashform.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL |
											GridData.GRAB_VERTICAL |
											GridData.FILL_BOTH));
	}
	
	/**
	 * This method initializes inputFilesSection	
	 */
	private void createInputFilesSection() {
		GridData gridData = new GridData();
		gridData.grabExcessHorizontalSpace = true;
		gridData.grabExcessVerticalSpace = true;
		gridData.horizontalAlignment = GridData.FILL;
		gridData.verticalAlignment = GridData.FILL;
		inputFilesSection = getFormToolkit().createSection(sashform,
				Section.DESCRIPTION | ExpandableComposite.TITLE_BAR);
		inputFilesSection.setExpanded(true);
		inputFilesSection.setLayoutData(gridData);
		inputFilesSection.setText("Input files");
		inputFilesSection.setDescription("Add or drag & drop result files (*.sca or *.vec) that should be used in this analysis. Wildcards (*,?) can also be used to specify multiple files.");
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
				inputFilesPanel.getRemoveFileButton(), 
				inputFilesPanel.getTreeViewer(),
				new RemoveAction());
	}

	/**
	 * This method initializes dataSection	
	 *
	 */
	private void createDataSection() {
		GridData gridData1 = new GridData();
		gridData1.grabExcessHorizontalSpace = true;
		gridData1.grabExcessVerticalSpace = true;
		gridData1.horizontalAlignment = GridData.FILL;
		gridData1.verticalAlignment = GridData.FILL;
		dataSection = getFormToolkit().createSection(sashform, 
				Section.DESCRIPTION | ExpandableComposite.TITLE_BAR);
		dataSection.setExpanded(true);
		dataSection.setLayoutData(gridData1);
		dataSection.setText("Data");
		dataSection.setDescription("Here you can browse all data (vectors, scalars and histograms) that come from the result files.");
		dataSection.setClient(new DataPanel(dataSection, SWT.NONE));
	}
}
