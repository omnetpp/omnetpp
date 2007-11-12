package org.omnetpp.scave.editors.ui;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.forms.widgets.FormToolkit;


/**
 * Bottom part of the InputsPage.
 * 
 * @author Andras
 */
public class DataPanel extends Composite {

	private FormToolkit formToolkit = null;
	private TreeViewer fileRunView;
	private TreeViewer runFileView;
	private TreeViewer logicalView;

	public DataPanel(Composite parent, int style) {
		super(parent, style | SWT.NONE);
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
		this.setLayout(new FillLayout());
		
		CTabFolder tabfolder = new CTabFolder(this, SWT.TOP | SWT.BORDER);
		getFormToolkit().adapt(tabfolder);

		// create the tabs
		fileRunView = createLabelAndTree(tabfolder, "Physical: by file and run");
		runFileView = createLabelAndTree(tabfolder, "Physical: by run and file");
		logicalView = createLabelAndTree(tabfolder, "Logical: by experiment, measurement, replication");
		
		tabfolder.setSelection(0);
	}

	private TreeViewer createLabelAndTree(CTabFolder parent, String text) {
		Tree tree = getFormToolkit().createTree(parent, SWT.NONE);
		
		CTabItem tabitem = new CTabItem(parent, SWT.NONE);
		tabitem.setText(text);
		tabitem.setControl(tree);
		
		return new TreeViewer(tree);
	}
	
	/**
	 * This method initializes formToolkit	
	 */
	private FormToolkit getFormToolkit() {
		if (formToolkit == null)
			formToolkit = new FormToolkit2(Display.getCurrent());
		return formToolkit;
	}

}
