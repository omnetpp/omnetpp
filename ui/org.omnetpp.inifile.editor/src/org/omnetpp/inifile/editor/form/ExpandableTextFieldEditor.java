/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import org.eclipse.swt.widgets.Composite;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * An inifile field editor which displays TextFieldEditor, and lets the user 
 * expand it to a TextTableFieldEditor. 
 * 
 * @author Andras
 */
public class ExpandableTextFieldEditor extends ExpandableFieldEditor {

	public ExpandableTextFieldEditor(Composite parent, ConfigOption entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, entry, inifile, formPage, labelText);
	}

	@Override
	protected FieldEditor createFieldEditor(boolean isExpanded) {
		return isExpanded ? 
				new TextTableFieldEditor(this, entry, inifile, formPage, labelText) :
				new TextFieldEditor(this, entry, inifile, formPage, labelText);
	}

}
