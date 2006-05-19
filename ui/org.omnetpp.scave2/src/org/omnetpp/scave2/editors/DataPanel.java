package org.omnetpp.scave2.editors;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Rectangle;

public class DataPanel extends Composite {

	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	private Label heading1 = null;
	private Label heading2 = null;
	private TreeViewer physicalView;
	private TreeViewer logicalView;

	public DataPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	public TreeViewer getPhysicalTreeViewer() {
		return physicalView;
	}
	
	public TreeViewer getLogicalTreeViewer() {
		return logicalView;
	}

	private void initialize() {
		this.setBounds(new Rectangle(0, 0, 500, 200));
		GridData gridData4 = new GridData();
		gridData4.grabExcessHorizontalSpace = true;
		gridData4.grabExcessVerticalSpace = true;
		gridData4.verticalAlignment = org.eclipse.swt.layout.GridData.FILL;
		gridData4.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridData gridData3 = new GridData();
		gridData3.grabExcessHorizontalSpace = true;
		gridData3.grabExcessVerticalSpace = true;
		gridData3.verticalAlignment = org.eclipse.swt.layout.GridData.FILL;
		gridData3.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridData gridData2 = new GridData();
		gridData2.grabExcessHorizontalSpace = false;
		gridData2.horizontalAlignment = org.eclipse.swt.layout.GridData.BEGINNING;
		GridData gridData1 = new GridData();
		gridData1.grabExcessHorizontalSpace = false;
		GridLayout gridLayout = new GridLayout();
		gridLayout.numColumns = 2;
		gridLayout.makeColumnsEqualWidth = true;
		gridLayout.horizontalSpacing = 10;
		heading1 = getFormToolkit().createLabel(this, "Physical: by file and run");
		heading1.setLayoutData(gridData1);
		heading2 = getFormToolkit().createLabel(this, "Logical: by experiment, measurement, replication");
		heading2.setLayoutData(gridData2);
		Tree physicalViewTree = getFormToolkit().createTree(this, SWT.BORDER);
		physicalViewTree.setLayoutData(gridData3);
		physicalView = new TreeViewer(physicalViewTree);
		Tree logicalViewTree = getFormToolkit().createTree(this, SWT.BORDER);
		logicalViewTree.setLayoutData(gridData4);
		logicalView = new TreeViewer(logicalViewTree);
		this.setLayout(gridLayout);
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
