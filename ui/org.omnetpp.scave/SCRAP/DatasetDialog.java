/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.SCRAP;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterHints;

/**
 * Common base class for CreateDatasetDialog and AddToDatasetDialog.
 *
 * @author tomi
 */
public abstract class DatasetDialog extends Dialog {
	
	public static final int SHOW_SELECTION = 0x01;
	public static final int SHOW_FILTER = 0x02;
	
	private int style;
	
	private String dialogTitle;
	
	private boolean useFilter;
	private Filter filterParams;
	private FilterHints filterHints;
	private String[] runidFields;
	
	private DataItemsPanel panel;
	private RunSelectionPanel selectionPanel;
	
	public DatasetDialog(Shell parent, String title) {
		this(parent, SHOW_SELECTION | SHOW_FILTER, title);
	}

	public DatasetDialog(Shell parentShell, int style, String dialogTitle) {
		super(parentShell);
		this.style = style;
		this.dialogTitle = dialogTitle;
	}
	
	public void setFilterParams(Filter params) {
		filterParams = params;
	}
	
	public void setFilterHints(FilterHints hints) {
		filterHints = hints;
	}
	
	public boolean useFilter() {
		return useFilter;
	}
	
	public void setUseFilter(boolean flag) {
		useFilter = flag;
	}
	
	public Filter getFilterParams() {
		return filterParams;
	}
	
	public String[] getRunIdFields() {
		return runidFields;
	}
	
	@Override
	protected void configureShell(Shell shell) {
		super.configureShell(shell);
		shell.setText(dialogTitle);
	}

	protected Composite createDataItemsPanel(Composite parent ) {
		if (style == (SHOW_SELECTION | SHOW_FILTER)) {
			panel = new DataItemsPanel(parent, SWT.NONE);
			panel.setLayoutData(new GridData(GridData.FILL_BOTH));
			panel.setUseFilter(useFilter);
			if (filterParams != null) {
				panel.setFilterParams(filterParams);
			}
			if (filterHints != null) {
				panel.setFilterHints(filterHints);
			}
			return panel;
		}
		else if (style == SHOW_SELECTION) {
			Composite p = new Composite(parent, SWT.NONE);
			p.setLayout(new GridLayout());
			Label label = new Label(p, SWT.NONE);
			label.setText("Add selected data items");
			GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
			gridData.horizontalIndent = 15;
			selectionPanel = new RunSelectionPanel(p, SWT.NONE);
			selectionPanel.setLayoutData(gridData);
			return p;
		}
		return null;
	}

	@Override
	protected void okPressed() {
		applyChanges();
		super.okPressed();
	}
	
	protected void applyChanges() {
		if (panel != null) {
			useFilter = panel.useFilter();
			filterParams = panel.getFilterParams();
			runidFields = panel.getRunIdFields();
		}
		else if (selectionPanel != null) {
			useFilter = false;
			runidFields = selectionPanel.getRunIdFields();
		}
	}
}
