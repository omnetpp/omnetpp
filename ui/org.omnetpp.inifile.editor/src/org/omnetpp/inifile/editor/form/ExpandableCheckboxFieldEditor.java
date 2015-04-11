/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import java.util.Map;

import org.eclipse.swt.widgets.Composite;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * An inifile field editor which displays CheckboxFieldEditor, and lets the user
 * expand it to a CheckboxTableFieldEditor.
 *
 * @author Andras
 */
public class ExpandableCheckboxFieldEditor extends ExpandableFieldEditor {
    protected String sectionColumnTitle;
    protected String objectColumnTitle;

    public ExpandableCheckboxFieldEditor(Composite parent, ConfigOption entry, IInifileDocument inifile, FormPage formPage, String labelText, Map<String,Object> hints) {
        super(parent, entry, inifile, formPage, labelText, hints);
    }

    public void setSectionColumnTitle(String text) {
        this.sectionColumnTitle = text;
        if (fieldEditor instanceof CheckboxTableFieldEditor)
            ((CheckboxTableFieldEditor)fieldEditor).setSectionColumnTitle(text);
    }

    public void setObjectColumnTitle(String text) {
        this.objectColumnTitle = text;
        if (fieldEditor instanceof CheckboxTableFieldEditor)
            ((CheckboxTableFieldEditor)fieldEditor).setObjectColumnTitle(text);
    }

    @Override
    protected FieldEditor createFieldEditor(boolean isExpanded) {
        if (!isExpanded)
            return new CheckboxFieldEditor(this, entry, inifile, formPage, labelText, hints);
        else {
            CheckboxTableFieldEditor editor = new CheckboxTableFieldEditor(this, entry, inifile, formPage, labelText, hints);
            if (sectionColumnTitle != null)
                editor.setSectionColumnTitle(sectionColumnTitle);
            if (objectColumnTitle != null)
                editor.setSectionColumnTitle(objectColumnTitle);
            return editor;
        }
    }

}
