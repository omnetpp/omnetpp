package org.omnetpp.inifile.editor.form;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.ConfigKey;
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
		for (ConfigKey e : ConfigRegistry.getEntries()) {
			String label = "The \""+e.getKey()+"\" setting";
			if (e.getDataType()==ConfigKey.DataType.CFG_BOOL) {
				CheckboxFieldEditor control = new CheckboxFieldEditor(this, e, doc, this, label);
				control.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
			}
			else {
				TextFieldEditor control = new TextFieldEditor(this, e, doc, this, label);
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
	public List<ConfigKey> getSupportedKeys() {
		return new ArrayList<ConfigKey>(); // dummy impl.
	}
}
