/**
 *
 */
package org.omnetpp.scave.editors;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;

class ReadonlyPropertyDescriptor extends TextPropertyDescriptor implements INonsortablePropertyDescriptor {
    ReadonlyPropertyDescriptor(Object id, String displayName, String category) {
        super(id, displayName);
        setCategory(category);
    }

    @Override
    public CellEditor createPropertyEditor(Composite parent) {
        return null;
    }
}
