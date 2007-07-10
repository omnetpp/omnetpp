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

	public ExpandableCheckboxFieldEditor(Composite parent, ConfigKey entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, entry, inifile, formPage, labelText);
	}

	@Override
	protected FieldEditor createFieldEditor(boolean isExpanded) {
		return isExpanded ? 
				new CheckboxTableFieldEditor(this, entry, inifile, formPage, labelText) :
				new CheckboxFieldEditor(this, entry, inifile, formPage, labelText);
	}

}
