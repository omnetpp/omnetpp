package org.omnetpp.inifile.editor.form;

import org.eclipse.swt.widgets.Composite;
import org.omnetpp.inifile.editor.model.ConfigKey;
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

	public ExpandableCheckboxFieldEditor(Composite parent, ConfigKey entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, entry, inifile, formPage, labelText);
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
		    return new CheckboxFieldEditor(this, entry, inifile, formPage, labelText);
		else {
		    CheckboxTableFieldEditor editor = new CheckboxTableFieldEditor(this, entry, inifile, formPage, labelText);
		    if (sectionColumnTitle != null)
		        editor.setSectionColumnTitle(sectionColumnTitle);
            if (objectColumnTitle != null)
                editor.setSectionColumnTitle(objectColumnTitle);
            return editor;
		}
	}

}
