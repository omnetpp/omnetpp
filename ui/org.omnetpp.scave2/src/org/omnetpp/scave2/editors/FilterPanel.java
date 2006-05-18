package org.omnetpp.scave2.editors;

import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;

/**
 * ScalarPanelComposite and VectorPanelComposite inherit from this class;
 * the purpose is that they can share the same controller, FilterPanel.
 */
public abstract class FilterPanel extends Composite {

	public FilterPanel(Composite parent, int style) {
		super(parent, style);
	}

	public abstract Button getFilterButton();

	public abstract CCombo getModuleNameCombo();

	public abstract CCombo getRunNameCombo();

	public abstract CCombo getNameCombo();

	public abstract Table getTable();

}