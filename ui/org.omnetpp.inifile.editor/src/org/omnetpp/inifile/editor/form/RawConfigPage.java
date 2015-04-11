/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Form page for editing all config entries. Currently unused.
 *
 * @author Andras
 */
public class RawConfigPage extends FormPage {

    public RawConfigPage(Composite parent, InifileEditor inifileEditor) {
        super(parent, inifileEditor);
        setLayout(new GridLayout(1,false));

        // populate with field editors
        IInifileDocument doc = getInifileDocument();
        for (ConfigOption e : ConfigRegistry.getOptions()) {
            String label = "The \""+e.getName()+"\" setting";
            if (e.getDataType()==ConfigOption.DataType.CFG_BOOL) {
                CheckboxFieldEditor control = new CheckboxFieldEditor(this, e, doc, this, label, null);
                control.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
            }
            else {
                TextFieldEditor control = new TextFieldEditor(this, e, doc, this, label, null);
                control.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
            }
        }
    }

    @Override
    public String getPageCategory() {
        return "(raw)";
    }

    @Override
    public boolean setFocus() {
        return getChildren()[0].setFocus(); // refine if needed
    }

    @Override
    public void reread() {
        super.reread();
        for (Control c : getChildren())
            if (c instanceof FieldEditor)
                ((FieldEditor) c).reread();
    }

    @Override
    public List<ConfigOption> getSupportedKeys() {
        return new ArrayList<ConfigOption>(); // dummy impl.
    }
}
