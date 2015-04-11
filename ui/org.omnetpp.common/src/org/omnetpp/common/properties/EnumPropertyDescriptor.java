/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.properties;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.LabelProvider;
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
            @Override
            public String getText(Object value) {
                String name = getName(value);
                return name != null ? name : "";
            }
        });
    }

    public void setEnumType(Class<?> enumType) {
        setEnumType(enumType, false);
    }

    public void setEnumType(Class<?> enumType, boolean optional)
    {
        List<Object> values = new ArrayList<Object>();
        List<String> names = new ArrayList<String>();

        if (optional) {
            values.add(null);
            names.add("");
        }

        if (enumType.isEnum()) { // Java 1.5 enum class
            Object[] enumValues = enumType.getEnumConstants();
            for (Object value : enumValues) {
                values.add(value);
                names.add(((Enum<?>)value).toString());
            }
        }
        else { // typesafe enum pattern class
            Field[] fields = enumType.getDeclaredFields();
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
        }
        this.values = values.toArray();
        this.names = names.toArray(new String[names.size()]);
    }

    public String getName(Object value) {
        for (int i = 0; i < values.length; ++i)
            if (value == null && values[i] == null ||
                value != null && value.equals(values[i]))
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

    @Override
    public CellEditor createPropertyEditor(Composite parent) {
        CellEditor editor = new EnumCellEditor(parent, names, values);
        if (getValidator() != null)
            editor.setValidator(getValidator());
        return editor;
    }
}

