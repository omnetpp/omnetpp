package org.omnetpp.scave.editors.ui;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.forms.widgets.FormToolkit;

public class TreeWithButtonsPanel extends Composite {

	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	private TreeViewer treeviewer = null;
	private Composite buttonPanel = null;
	
	public TreeWithButtonsPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	public Composite getButtonPanel() {
		return buttonPanel;
	}
	
	public TreeViewer getTreeViewer() {
		return treeviewer;
	}
	
	private void initialize() {
		setBackground(getParent().getBackground());
		GridData gridData1 = new GridData();
		gridData1.grabExcessHorizontalSpace = true;
		gridData1.verticalAlignment = org.eclipse.swt.layout.GridData.FILL;
		gridData1.grabExcessVerticalSpace = true;
		gridData1.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridLayout gridLayout = new GridLayout();
		gridLayout.numColumns = 2;
		gridLayout.horizontalSpacing = 10;
		Tree tree = getFormToolkit().createTree(this, SWT.BORDER | SWT.MULTI);
		tree.setLayoutData(gridData1);
		treeviewer = new TreeViewer(tree);
		this.setLayout(gridLayout);
		createButtonPanel();
	}

	/**
	 * This method initializes formToolkit	
	 * 	
	 * @return org.eclipse.ui.forms.widgets.FormToolkit	
	 */
	protected FormToolkit getFormToolkit() {
		if (formToolkit == null)
			formToolkit = new FormToolkit2(Display.getCurrent());
		return formToolkit;
	}

	/**
	 * This method initializes buttonPanel	
	 *
	 */
	private void createButtonPanel() {
		FillLayout fillLayout = new FillLayout();
		fillLayout.type = org.eclipse.swt.SWT.VERTICAL;
		GridData gridData2 = new GridData();
		gridData2.verticalAlignment = org.eclipse.swt.layout.GridData.BEGINNING;
		buttonPanel = getFormToolkit().createComposite(this);
		buttonPanel.setLayoutData(gridData2);
		buttonPanel.setLayout(fillLayout);
		buttonPanel.setBackground(this.getBackground());
	}
}  //  @jve:decl-index=0:visual-constraint="10,10"
