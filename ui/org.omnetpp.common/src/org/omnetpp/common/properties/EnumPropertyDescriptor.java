package org.omnetpp.common.properties;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.ArrayList;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ComboBoxCellEditor;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
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
		
		setLabelProvider(new LabelProvider() {
			public String getText(Object value) {
				String name = getName(value);
				return name != null ? name : "";
			}
		});
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
	
	public String getName(Object value) {
		if (value != null)
			for (int i = 0; i < values.length; ++i)
				if (value.equals(values[i]))
					return names[i];
		return null;
	}
	
	public Object getValue(String name) {
		if (name != null)
			for (int i = 0; i < names.length; ++i)
				if (name.equals(names[i]))
					return values[i];
		return null;
	}
	
	public CellEditor createPropertyEditor(Composite parent) {
        CellEditor editor = new EnumCellEditor(parent);
        if (getValidator() != null)
            editor.setValidator(getValidator());
        return editor;
    }
	
	class EnumCellEditor extends ComboBoxCellEditor {
		
		public EnumCellEditor(Composite parent) {
			super(parent, names, SWT.READ_ONLY);
		}

		@Override
		protected Object doGetValue() {
			int index = (Integer)super.doGetValue();
			return 0 <= index && index < values.length ? values[index] : null;
		}

		@Override
		protected void doSetValue(Object value) {
			if (value != null)
				for(int i = 0; i < values.length; ++i)
					if (value.equals(values[i])) {
						super.doSetValue(i);
						return;
					}
		}
	}
}

