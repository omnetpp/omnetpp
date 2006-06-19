package org.omnetpp.scave2.editors.datatable;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

/**
 * A composite with UI elements to filter a data table.
 * This is a passive component, needs to be configured
 * to do anything useful.
 * @author andras
 */
public class FilteringPanel extends Composite {
	private CCombo runNameCombo;
	private CCombo moduleNameCombo;
	private CCombo scalarNameCombo;
	private Button filterButton;

	public FilteringPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}

	public CCombo getModuleNameCombo() {
		return moduleNameCombo;
	}

	public CCombo getRunNameCombo() {
		return runNameCombo;
	}

	public CCombo getNameCombo() {
		return scalarNameCombo;
	}

	public Button getFilterButton() {
		return filterButton;
	}

	private void initialize() {
		GridLayout gridLayout = new GridLayout();
		gridLayout.numColumns = 4;
		this.setLayout(gridLayout);
		Label runNameLabel = new Label(this, SWT.NONE);
		runNameLabel.setText("File name and run number:");
		Label moduleNameLabel = new Label(this, SWT.NONE);
		moduleNameLabel.setText("Module name:");
		Label scalarNameLabel = new Label(this, SWT.NONE);
		scalarNameLabel.setText("Scalar name:");
		new Label(this, SWT.NONE); // filler
		runNameCombo = new CCombo(this, SWT.BORDER);
		runNameCombo.setVisibleItemCount(10);
		runNameCombo.setToolTipText("Filter displayed items by file and run. Wildcards *,? are OK.");
		runNameCombo.setLayoutData(new GridData(SWT.FILL, SWT.TOP, true, false));
		moduleNameCombo = new CCombo(this, SWT.BORDER);
		moduleNameCombo.setVisibleItemCount(10);
		moduleNameCombo.setToolTipText("Filter displayed items by module name. Wildcards *,? are OK.");
		moduleNameCombo.setLayoutData(new GridData(SWT.FILL, SWT.TOP, true, false));
		scalarNameCombo = new CCombo(this, SWT.BORDER);
		scalarNameCombo.setVisibleItemCount(10);
		scalarNameCombo.setToolTipText("Filter displayed items by name. Wildcards *,? are OK.");
		scalarNameCombo.setLayoutData(new GridData(SWT.FILL, SWT.TOP, true, false));
		filterButton = new Button(this, SWT.NONE);
		filterButton.setText("Filter");
	}

}
