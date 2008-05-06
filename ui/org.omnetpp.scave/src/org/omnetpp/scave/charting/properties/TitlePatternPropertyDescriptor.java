package org.omnetpp.scave.charting.properties;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.scave.editors.ui.ResultItemNamePatternField;

public class TitlePatternPropertyDescriptor extends PropertyDescriptor
{
	
	public TitlePatternPropertyDescriptor(Object id, String displayName) {
		super(id, displayName);
	}

	public CellEditor createPropertyEditor(Composite parent) {
		return new TitlePatternCellEditor(parent);
	}
}

class TitlePatternCellEditor extends TextCellEditor
{
	public TitlePatternCellEditor(Composite parent) {
		super(parent);
	}

	@Override
	protected Control createControl(Composite parent) {
		Control control =  super.createControl(parent);
		new ResultItemNamePatternField(text);
		return control;
	}

	@Override
	protected void doSetValue(Object value) {
		if (value == null)
			value = "";
		super.doSetValue(value);
	}
}

