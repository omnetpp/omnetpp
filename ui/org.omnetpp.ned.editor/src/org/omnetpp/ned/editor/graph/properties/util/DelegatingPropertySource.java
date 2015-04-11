/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.DialogCellEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;

/**
 * A property source that delegates all functions to another, except that provides its own
 * descriptor, and as a value provides the other property source
 *
 * @author rhornig
 */
public class DelegatingPropertySource implements IPropertySource2 {
        public enum Prop { ID };
        protected IPropertyDescriptor[] descriptors;
        protected IPropertySource2 delegateTo;

        public DelegatingPropertySource(final IPropertySource2 delegateTo, String name, String descr) {
            this.delegateTo = delegateTo;

            PropertyDescriptor propDesc = new PropertyDescriptor(this, name) {
                // if the property source we delegate to provides cell editor
                // we should use it as the current cell editor
                @Override
                public CellEditor createPropertyEditor(Composite parent) {
                    if (!(delegateTo instanceof IDialogCellEditorProvider))
                        return null;
                    DialogCellEditor cellEditor = ((IDialogCellEditorProvider)delegateTo).getCellEditor();
                    if (cellEditor != null ) {
                        cellEditor.create(parent);
                        if (getValidator() != null)
                            cellEditor.setValidator(getValidator());
                    }
                    return cellEditor;
                }
            };
            propDesc.setCategory(MergedPropertySource.BASE_CATEGORY);
            propDesc.setDescription(descr);

            descriptors = new IPropertyDescriptor[] { propDesc };
        }

        public Object getEditableValue() {
            return delegateTo.getEditableValue();
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            return descriptors;
        }

        public Object getPropertyValue(Object propName) {
            if (this.equals(propName))
                return delegateTo;

            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
        }

        public boolean isPropertySet(Object propName) {
            return true;
        }

        public void resetPropertyValue(Object propName) {
        }

        public boolean isPropertyResettable(Object propName) {
            return false;
        }
}
