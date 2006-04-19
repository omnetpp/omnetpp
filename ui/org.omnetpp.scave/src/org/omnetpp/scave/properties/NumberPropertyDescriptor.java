package org.omnetpp.scave.properties;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ICellEditorValidator;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;

public class NumberPropertyDescriptor extends TextPropertyDescriptor {

	public NumberPropertyDescriptor(Object id, String displayName) {
		super(id, displayName);
	}

	@Override
	public CellEditor createPropertyEditor(Composite parent) {
		return new NumberCellEditor(parent);
	}
}

class NumberCellEditor extends TextCellEditor
{
	public NumberCellEditor(Composite parent) {
		super(parent);
		setValidator(NumberCellEditorValidator.instance());
	}
	
	@Override
	protected Object doGetValue() {
		return Double.parseDouble((String)super.doGetValue());
	}

	@Override
	protected void doSetValue(Object value) {
		super.doSetValue(value.toString());
	}
}

class NumberCellEditorValidator implements ICellEditorValidator
{
	private static NumberCellEditorValidator instance;
	
	public static NumberCellEditorValidator instance() {
		if (instance == null)
			instance = new NumberCellEditorValidator();
		return instance;
	}

	public String isValid(Object value) {
		if (value instanceof Double)
			return null;
		
		try {
			Double.parseDouble((String)value);
			return null;
		} catch (NumberFormatException e) {
			return "Not a number";
		}
	}
}
