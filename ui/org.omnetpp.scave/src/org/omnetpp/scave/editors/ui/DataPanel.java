package org.omnetpp.scave.editors.ui;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.forms.FormColors;
import org.eclipse.ui.forms.IFormColors;
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

		// make the selected tab stand out (by default, everything is white)
		Color blue = getFormToolkit().getColors().getColor(IFormColors.TB_BORDER);
		Color lighterBlue = new Color(null, FormColors.blend(blue.getRGB(), new RGB(255,255,255), 30));
		tabfolder.setSelectionBackground(new Color[]{blue, lighterBlue}, new int[] {25}, true);

		// other color variations tried:
		//Color white = getDisplay().getSystemColor(SWT.COLOR_WHITE);
		//tabfolder.setSelectionBackground(lighterBlue);
		//tabfolder.setSelectionBackground(new Color[]{lighterBlue, white}, new int[] {25}, true);
		//Color gray = getDisplay().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND);
		//tabfolder.setBackground(gray);
		//tabfolder.setSelectionBackground(gray);

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
