package org.omnetpp.scave2.editors.ui;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.graphics.Rectangle;


public class DataPanel extends Composite {

	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	private Label heading1 = null;
	private Label heading2 = null;
	private TreeViewer fileRunView;
	private TreeViewer runFileView;
	private TreeViewer logicalView;

	public DataPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	public TreeViewer getFileRunTreeViewer() {
		return fileRunView;
	}
	
	public TreeViewer getRunFileTreeViewer() {
		return runFileView;
	}
	
	public TreeViewer getLogicalTreeViewer() {
		return logicalView;
	}

	private void initialize() {
		this.setBounds(new Rectangle(0, 0, 500, 200));
		GridLayout gridLayout = new GridLayout();
		gridLayout.numColumns = 2;
		gridLayout.makeColumnsEqualWidth = true;
		gridLayout.horizontalSpacing = 10;
		this.setLayout(gridLayout);

		GridData gridData1 = new GridData();
		gridData1.grabExcessHorizontalSpace = false;
		heading1 = getFormToolkit().createLabel(this, "Physical: by file and run");
		heading1.setLayoutData(gridData1);
		GridData gridData2 = new GridData();
		gridData2.grabExcessHorizontalSpace = false;
		gridData2.horizontalAlignment = GridData.BEGINNING;
		heading2 = getFormToolkit().createLabel(this, "Logical: by experiment, measurement, replication");
		heading2.setLayoutData(gridData2);
		createPhysicalViews();
		createLogicalView();
	}
	
	private void createPhysicalViews() {
		GridData gridData3 = new GridData();
		gridData3.grabExcessHorizontalSpace = true;
		gridData3.grabExcessVerticalSpace = true;
		gridData3.verticalAlignment = GridData.FILL;
		gridData3.horizontalAlignment = GridData.FILL;
		SashForm sashForm = new SashForm(this, SWT.HORIZONTAL);
		sashForm.setLayoutData(gridData3);
		
		Tree fileRunTree = getFormToolkit().createTree(sashForm, SWT.BORDER);
		fileRunView = new TreeViewer(fileRunTree);
		Tree runFileTree = getFormToolkit().createTree(sashForm, SWT.BORDER);
		runFileView = new TreeViewer(runFileTree);
		
		//sashForm.setMaximizedControl(fileRunTree);
		sashForm.setWeights(new int[] {100, 0});
	}
	
	private void createLogicalView() {
		GridData gridData = new GridData();
		gridData.grabExcessHorizontalSpace = true;
		gridData.grabExcessVerticalSpace = true;
		gridData.verticalAlignment = GridData.FILL;
		gridData.horizontalAlignment = GridData.FILL;
		Tree logicalViewTree = getFormToolkit().createTree(this, SWT.BORDER);
		logicalViewTree.setLayoutData(gridData);
		logicalView = new TreeViewer(logicalViewTree);
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

}
