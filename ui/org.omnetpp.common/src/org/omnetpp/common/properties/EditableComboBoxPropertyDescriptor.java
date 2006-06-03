package org.omnetpp.common.properties;

import java.util.Collection;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;

public class EditableComboBoxPropertyDescriptor extends TextPropertyDescriptor {

    /** The list of possible values to display in the combo box. */
    private String[] values;

    /** The control of this property descriptor. */
    private EditableComboBoxCellEditor editor;


    /**
     *
     * @param id
     * @param displayName
     * @param valuesArray
     */
    public EditableComboBoxPropertyDescriptor(Object id, String displayName, String[] valuesArray) {
        super(id, displayName);
        values = valuesArray;
    }

    public EditableComboBoxPropertyDescriptor(Object id, String displayName) {
        super(id, displayName);
    }
    
    /**
     *
     */
    @Override
    public CellEditor createPropertyEditor(Composite parent) {
        editor = new EditableComboBoxCellEditor(parent, values);

        if (getValidator() != null) {
            editor.setValidator(getValidator());
        }

        return editor;
    }

    /**
    *
    * @param items
    */
   public void setItems(String[] valuesArray) {
       values = valuesArray;
   }

   public void setItems(Collection<String> coll) {
       if (coll == null) {
           values = null;
           return;
       }
       
       values = coll.toArray(new String[coll.size()]);
   }
}
