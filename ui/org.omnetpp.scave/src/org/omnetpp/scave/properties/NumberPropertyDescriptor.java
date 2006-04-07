package org.omnetpp.scave.properties;

import org.eclipse.jface.viewers.ICellEditorValidator;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;

public class NumberPropertyDescriptor extends TextPropertyDescriptor {

	public NumberPropertyDescriptor(Object id, String displayName) {
		super(id, displayName);
		this.setValidator(NumberCellEditorValidator.instance());
	}
}

class NumberCellEditorValidator implements ICellEditorValidator
{
	private static NumberCellEditorValidator instance;
	
	public static NumberCellEditorValidator instance()
	{
		if (instance == null)
			instance = new NumberCellEditorValidator();
		return instance;
	}

	public String isValid(Object value) {
		try
		{
			Double.parseDouble((String)value);
			return null;
		}
		catch (NumberFormatException e)
		{
			return "Not a number";
		}
	}
}
