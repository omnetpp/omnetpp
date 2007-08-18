package org.omnetpp.common.properties;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.common.color.ColorFactory;

/**
 * Descriptor for a property that has a color value which should be edited
 * with a color cell editor. This is a modified version of the platform's class,
 * providing a specialized text editor
 */
public class ColorPropertyDescriptor extends PropertyDescriptor {
    /**
     * Creates an property descriptor with the given id and display name.
     * 
     * @param id the id of the property
     * @param displayName the name to display for the property
     */
    public ColorPropertyDescriptor(Object id, String displayName) {
        super(id, displayName);
        setLabelProvider(new ColorFactory.ColorLabelProvider());
    }

    /**
     * The <code>ColorPropertyDescriptor</code> implementation of this 
     * <code>IPropertyDescriptor</code> method creates and returns a new
     * <code>ColorCellEditor</code>.
     * <p>
     * The editor is configured with the current validator if there is one.
     * </p>
     */
    public CellEditor createPropertyEditor(Composite parent) {
        CellEditor editor = new ColorCellEditorEx(parent);
        if (getValidator() != null) {
            editor.setValidator(getValidator());
        }
        return editor;
    }
}
