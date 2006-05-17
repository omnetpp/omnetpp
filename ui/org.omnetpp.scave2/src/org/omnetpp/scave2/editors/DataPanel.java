package org.omnetpp.scave2.editors;

import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.graphics.Point;
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
	private Label label = null;
	private Label heading1 = null;
	private Label heading2 = null;
	private Tree PhysicalViewTree = null;
	private Tree LogicalViewTree = null;

	public DataPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}

	private void initialize() {
		this.setBounds(new Rectangle(0, 0, 500, 200));
		GridData gridData4 = new GridData();
		gridData4.grabExcessHorizontalSpace = true;
		gridData4.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridData gridData3 = new GridData();
		gridData3.grabExcessHorizontalSpace = true;
		gridData3.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridData gridData2 = new GridData();
		gridData2.grabExcessHorizontalSpace = false;
		gridData2.horizontalAlignment = org.eclipse.swt.layout.GridData.BEGINNING;
		GridData gridData1 = new GridData();
		gridData1.grabExcessHorizontalSpace = false;
		GridData gridData = new GridData();
		gridData.horizontalSpan = 2;
		gridData.grabExcessHorizontalSpace = true;
		GridLayout gridLayout = new GridLayout();
		gridLayout.numColumns = 2;
		gridLayout.makeColumnsEqualWidth = true;
		gridLayout.horizontalSpacing = 10;
		label = getFormToolkit().createLabel(this, "Here you can browse all data (vectors, scalars and histograms) that come from the input files.");
		label.setLayoutData(gridData);
		heading1 = getFormToolkit().createLabel(this, "Physical: by file and run");
		heading1.setLayoutData(gridData1);
		heading2 = getFormToolkit().createLabel(this, "Logical: by experiment, measurement, replication");
		heading2.setLayoutData(gridData2);
		PhysicalViewTree = getFormToolkit().createTree(this, SWT.BORDER);
		PhysicalViewTree.setLayoutData(gridData3);
		LogicalViewTree = getFormToolkit().createTree(this, SWT.BORDER);
		LogicalViewTree.setLayoutData(gridData4);
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
