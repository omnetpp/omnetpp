/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.properties;

import org.eclipse.jface.resource.StringConverter;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.DialogCellEditor;
import org.eclipse.jface.viewers.ICellEditorValidator;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.FontDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.views.properties.PropertyDescriptor;

public class FontPropertyDescriptor extends PropertyDescriptor {

    public FontPropertyDescriptor(Object id, String displayName) {
        super(id, displayName);
    }

    public CellEditor createPropertyEditor(Composite parent) {
        CellEditor editor = new FontDialogCellEditor(parent);
        ICellEditorValidator validator = getValidator();
        if (validator == null)
            validator = new ICellEditorValidator() {
                public String isValid(Object value) {

                    if (value instanceof FontData)
                        return null;
                    else
                        return "FontData expected";
                }
        };
        editor.setValidator(validator);
        return editor;
    }

    public ILabelProvider getLabelProvider() {
        return new LabelProvider() {
            public String getText(Object object) {
                return StringConverter.asString((FontData)object);
            }
        };
    }

    static class FontDialogCellEditor extends DialogCellEditor {

        protected FontDialogCellEditor(Composite parent) {
            super(parent);
        }

        protected Object openDialogBox(Control cellEditorWindow) {
            FontDialog fontDialog = new FontDialog( cellEditorWindow.getShell());

            FontData value = (FontData)getValue();

            if (value != null)
                fontDialog.setFontList(new FontData[] {value});
            FontData fd = fontDialog.open();

            if (fd != null) {
                value = fd;
            }
            return value;
        }

        @Override
        protected void updateContents(Object value) {
            Label label = getDefaultLabel();
            if (label != null) {
                String text = ""; //$NON-NLS-1$
                if (value != null && value instanceof FontData)
                    text = StringConverter.asString((FontData)value);
                label.setText(text);
            }
        }

        @Override
        protected void doSetValue(Object value) {
            super.doSetValue(value);
            setValueValid(isCorrect(value));
        }
    }
}

