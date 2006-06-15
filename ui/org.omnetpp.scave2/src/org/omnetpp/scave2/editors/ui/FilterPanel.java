package org.omnetpp.scave2.editors.ui;

import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.scave2.model.FilterParams;

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

	public abstract TableViewer getTableViewer();
	
	public FilterParams getFilterParams() {
		return new FilterParams(
				getRunNameCombo().getText(),
				getModuleNameCombo().getText(),
				getNameCombo().getText());
	}
}