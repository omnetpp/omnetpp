package org.omnetpp.scave.properties;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.ArrayList;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ComboBoxCellEditor;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.views.properties.ComboBoxLabelProvider;
import org.eclipse.ui.views.properties.PropertyDescriptor;

public class EnumPropertyDescriptor extends PropertyDescriptor {

	private final static Object[] EmptyObjectArray = new Object[0];
	private final static String[] EmptyStringArray = new String[0];
	
	private String[] names;
	private Object[] values;
	
	public EnumPropertyDescriptor(Object id, String displayName) {
		super(id, displayName);
		names = EmptyStringArray;
		values = EmptyObjectArray;
	}
	
	public void setEnumType(Class enumType)
	{
		Field[] fields = enumType.getDeclaredFields();
		ArrayList<Object> values = new ArrayList<Object>();
		ArrayList<String> names = new ArrayList<String>();
		
		for (Field field : fields)
		{
			int modifiers = field.getModifiers();
			if (Modifier.isFinal(modifiers) && Modifier.isStatic(modifiers) &&
				Modifier.isPublic(modifiers))
			{
				try {
					values.add(field.get(null));
					names.add(field.getName());
				} catch (IllegalArgumentException e) {
				} catch (IllegalAccessException e) {}
			}
		}
		
		this.values = values.toArray();
		this.names = names.toArray(new String[names.size()]);
	}
	
	public CellEditor createPropertyEditor(Composite parent) {
        CellEditor editor = new ComboBoxCellEditor(parent, names, SWT.READ_ONLY);
        if (getValidator() != null)
            editor.setValidator(getValidator());
        return editor;
    }

    public ILabelProvider getLabelProvider() {
        if (isLabelProviderSet())
            return super.getLabelProvider();
        else
            return new ComboBoxLabelProvider(names);
    }	
	
	public Object getValue(int index)
	{
		return values[index];
	}
	
	public int getIndex(Object value)
	{
		for (int i = 0; i < values.length; ++i)
			if (values[i].equals(value))
				return i;
		return -1;
	}
}
