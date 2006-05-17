package org.omnetpp.scave2.editors;

import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.graphics.Rectangle;

public class TreeWithButtonsPanel extends Composite {

	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	private Label label = null;
	private Tree tree = null;
	private Composite buttonPanel = null;
	public TreeWithButtonsPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	public Label getLabel() {
		return label;
	}

	public Composite getButtonPanel() {
		return buttonPanel;
	}
	
	public Tree getTree() {
		return tree;
	}
	
	private void initialize() {
		this.setBounds(new Rectangle(0, 0, 500, 200));
		GridData gridData1 = new GridData();
		gridData1.grabExcessHorizontalSpace = true;
		gridData1.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridData gridData = new GridData();
		gridData.horizontalSpan = 2;
		gridData.grabExcessHorizontalSpace = true;
		GridLayout gridLayout = new GridLayout();
		gridLayout.numColumns = 2;
		gridLayout.horizontalSpacing = 10;
		label = getFormToolkit().createLabel(this, "Label");
		label.setLayoutData(gridData);
		label.setBackground(this.getBackground());
		tree = getFormToolkit().createTree(this, SWT.BORDER);
		tree.setLayoutData(gridData1);
		this.setLayout(gridLayout);
		createButtonPanel();
	}

	/**
	 * This method initializes formToolkit	
	 * 	
	 * @return org.eclipse.ui.forms.widgets.FormToolkit	
	 */
	protected FormToolkit getFormToolkit() {
		if (formToolkit == null) {
			formToolkit = new FormToolkit(Display.getCurrent());
		}
		return formToolkit;
	}

	/**
	 * This method initializes buttonPanel	
	 *
	 */
	private void createButtonPanel() {
		GridData gridData2 = new GridData();
		gridData2.verticalAlignment = org.eclipse.swt.layout.GridData.BEGINNING;
		RowLayout rowLayout = new RowLayout();
		rowLayout.type = org.eclipse.swt.SWT.VERTICAL;
		rowLayout.wrap = false;
		buttonPanel = getFormToolkit().createComposite(this);
		buttonPanel.setLayout(rowLayout);
		buttonPanel.setLayoutData(gridData2);
		buttonPanel.setBackground(this.getBackground());
	}
}  //  @jve:decl-index=0:visual-constraint="10,10"
