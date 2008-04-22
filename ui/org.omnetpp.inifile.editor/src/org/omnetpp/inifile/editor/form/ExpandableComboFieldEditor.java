package org.omnetpp.inifile.editor.form;

import java.util.List;

import org.eclipse.swt.widgets.Composite;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * An inifile field editor which displays ComboFieldEditor, and lets the user 
 * expand it to a TextTableFieldEditor. 
 * 
 * @author Andras
 */
public class ExpandableComboFieldEditor extends ExpandableFieldEditor {
    protected List<String> comboContents; 

	public ExpandableComboFieldEditor(Composite parent, ConfigKey entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, entry, inifile, formPage, labelText);
	}

	@Override
	protected FieldEditor createFieldEditor(boolean isExpanded) {
		FieldEditor result = isExpanded ? 
				new TextTableFieldEditor(this, entry, inifile, formPage, labelText) : // currently we have no ComboTableFieldEditor
				new ComboFieldEditor(this, entry, inifile, formPage, labelText);
		result.setComboContents(comboContents);
		return result;
	}

	public void setComboContents(List<String> list) {
	    comboContents = list;
	    getInnerFieldEditor().setComboContents(list);
	}

}
