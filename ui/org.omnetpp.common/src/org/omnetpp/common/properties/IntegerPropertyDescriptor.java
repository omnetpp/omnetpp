package org.omnetpp.common.properties;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ICellEditorValidator;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.StringUtils;

public class IntegerPropertyDescriptor extends TextPropertyDescriptor {

	public IntegerPropertyDescriptor(Object id, String displayName) {
		super(id, displayName);
	}

	@Override
	public CellEditor createPropertyEditor(Composite parent) {
		return new IntegerCellEditor(parent);
	}
}

class IntegerCellEditor extends TextCellEditor
{
	public IntegerCellEditor(Composite parent) {
		super(parent);
		setValidator(IntegerCellEditorValidator.instance());
	}
	
	@Override
	protected Object doGetValue() {
		return Converter.stringToInteger((String)super.doGetValue());
	}

	@Override
	protected void doSetValue(Object value) {
		super.doSetValue(StringUtils.defaultString(Converter.integerToString((Integer)value)));
	}
}

class IntegerCellEditorValidator implements ICellEditorValidator
{
	private static IntegerCellEditorValidator instance;
	
	public static IntegerCellEditorValidator instance() {
		if (instance == null)
			instance = new IntegerCellEditorValidator();
		return instance;
	}

	public String isValid(Object value) {
		if (value instanceof Integer)
			return null;
		
		if (value != null && !(value instanceof String))
			return "Unexpected type: " + value.getClass().getName();
		
		String strValue = (String)value;
		if (StringUtils.isEmpty(strValue))
			return null;
		
		try {
			Integer.parseInt(strValue);
			return null;
		} catch (NumberFormatException e) {
			return "Not a number";
		}
	}
}
