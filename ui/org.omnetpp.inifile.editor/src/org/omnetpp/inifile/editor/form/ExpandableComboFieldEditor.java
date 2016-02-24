/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import java.util.List;
import java.util.Map;

import org.eclipse.swt.widgets.Composite;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * An inifile field editor which displays ComboFieldEditor, and lets the user
 * expand it to a ComboTableFieldEditor.
 *
 * @author Andras
 */
public class ExpandableComboFieldEditor extends ExpandableFieldEditor {
    protected boolean enableTypein;
    protected List<String> comboContents;

    public ExpandableComboFieldEditor(Composite parent, ConfigOption entry, IInifileDocument inifile, FormPage formPage, String labelText, Map<String,Object> hints, boolean enableTypein) {
        super(parent, entry, inifile, formPage, labelText, hints);
        this.enableTypein = enableTypein;
    }

    @Override
    protected FieldEditor createFieldEditor(boolean isExpanded) {
        FieldEditor result = isExpanded ?
                new ComboTableFieldEditor(this, entry, inifile, formPage, labelText, hints) : // currently we have no ComboTableFieldEditor
                new ComboFieldEditor(this, entry, inifile, formPage, labelText, hints, enableTypein);
        result.setComboContents(comboContents);
        return result;
    }

    public void setComboContents(List<String> list) {
        comboContents = list;
        getInnerFieldEditor().setComboContents(list);
    }

}
