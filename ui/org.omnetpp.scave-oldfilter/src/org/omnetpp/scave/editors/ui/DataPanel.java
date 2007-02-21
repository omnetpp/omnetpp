package org.omnetpp.scave.editors.ui;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.forms.widgets.FormToolkit;


/**
 * This class has been created with the Visual Editor. DO NOT HAND-EDIT!
 * 
 * @author Andras
 */
public class DataPanel extends SashForm {

	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	private TreeViewer fileRunView;
	private TreeViewer runFileView;
	private TreeViewer logicalView;

	public DataPanel(Composite parent, int style) {
		super(parent, style | SWT.SMOOTH);
		getFormToolkit().adapt(this);
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

		fileRunView = createLabelAndTree(this, "Physical: by file and run");
		runFileView = createLabelAndTree(this, "Physical: by run and file");
		logicalView = createLabelAndTree(this, "Logical: by experiment, measurement, replication");
	}

	private TreeViewer createLabelAndTree(Composite parent, String text) {
		Composite composite = new Composite(parent, SWT.BACKGROUND);
		composite.setLayout(new GridLayout(1, false));

		Label label = getFormToolkit().createLabel(composite, text);
		label.setLayoutData(new GridData(GridData.BEGINNING, GridData.BEGINNING, false, false));
		Tree tree = getFormToolkit().createTree(composite, SWT.BORDER);
		tree.setLayoutData(new GridData(GridData.FILL, GridData.FILL, true, true));
		return new TreeViewer(tree);
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
