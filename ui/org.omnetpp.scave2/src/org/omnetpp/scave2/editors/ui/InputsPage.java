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
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave2.actions.AddResultFileAction;
import org.omnetpp.scave2.actions.AddWildcardResultFileAction;
import org.omnetpp.scave2.actions.RemoveAction;
import org.omnetpp.scave2.editors.ScaveEditor;

public class InputsPage extends ScrolledForm {

	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	private Section inputFilesSection = null;
	private Section dataSection = null;
	private ScaveEditor scaveEditor = null;  // the containing editor
	private SashForm sashform = null;

	public InputsPage(Composite parent, int style, ScaveEditor scaveEditor) {
		super(parent, style | SWT.V_SCROLL);
		this.scaveEditor = scaveEditor;
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
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout());
		createSashForm();
		createInputFilesSection();
		createDataSection();
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
		inputFilesSection.setDescription("Add or drag & drop result files (*.sca or *.vec) that should be used in this analysis.");
		InputFilesPanel inputFilesPanel = new InputFilesPanel(inputFilesSection, SWT.NONE);
		inputFilesSection.setClient(inputFilesPanel);

		// buttons
		scaveEditor.configureViewerButton(
				inputFilesPanel.getAddFileButton(), 
				inputFilesPanel.getTreeViewer(),
				new AddResultFileAction());
		scaveEditor.configureViewerButton(
				inputFilesPanel.getAddWildcardButton(), 
				inputFilesPanel.getTreeViewer(),
				new AddWildcardResultFileAction());
		scaveEditor.configureViewerButton(
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
