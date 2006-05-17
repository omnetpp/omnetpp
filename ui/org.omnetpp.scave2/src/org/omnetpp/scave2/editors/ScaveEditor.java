package org.omnetpp.scave2.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.ui.forms.events.ExpansionAdapter;
import org.eclipse.ui.forms.events.ExpansionEvent;
import org.eclipse.ui.forms.widgets.ColumnLayout;
import org.eclipse.ui.forms.widgets.ExpandableComposite;
import org.eclipse.ui.forms.widgets.FormText;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;
import org.eclipse.ui.part.MultiPageEditorPart;

public class ScaveEditor extends MultiPageEditorPart {
	
	/**
	 * The constructor.
	 */
	public ScaveEditor() {
	}
	
	protected void createPages() {
		
        FillLayout layout = new FillLayout();
        getContainer().setLayout(layout);

        setPartName("wirelessLan.scave");
        createOverviewPage();
        createDatasetPage("queue lengths");
        createDatasetPage("average EED");
        createDatasetPage("frame counts");
        createChartPage("packet loss");
        createChartPage("delay");
	}
	
	private void createOverviewPage() {
		OverviewPage page = new OverviewPage(getContainer(), SWT.NONE);
		setFormTitle(page, "Overview");
		int index = addPage(page);
		setPageText(index, "Overview");
	}
	
	private void createDatasetPage(String name) {
		DatasetPage page = new DatasetPage(getContainer(), SWT.NONE);
		setFormTitle(page, "Dataset: " + name);
		int index = addPage(page);
		setPageText(index, "Dataset: " + name);
	}
	
	private void createChartPage(String name) {
		ChartSheetPage page = new ChartSheetPage(getContainer(), SWT.NONE);
		setFormTitle(page, "Charts: " + name);
		int index = addPage(page);
		setPageText(index, "Charts: " + name);
	}
	
	private void setFormTitle(ScrolledForm form, String title) {
		form.setFont(new Font(null, "Arial", 12, SWT.BOLD));
		form.setForeground(new Color(null, 0, 128, 255));
		form.setText(title);
	}
	
	@Override
	public void doSave(IProgressMonitor monitor) {
		// TODO Auto-generated method stub

	}
	@Override
	public void doSaveAs() {
		// TODO Auto-generated method stub

	}

	@Override
	public boolean isDirty() {
		// TODO Auto-generated method stub
		return false;
	}
	@Override
	public boolean isSaveAsAllowed() {
		// TODO Auto-generated method stub
		return false;
	}
}


