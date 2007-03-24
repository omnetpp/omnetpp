package org.omnetpp.inifile.editor.form;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.ConfigurationRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Makeshift form page, just throws in all field editors.
 * TODO design proper forms instead, and throw this out
 * 
 * @author Andras
 */
public class GenericConfigPage extends FormPage {
	
	public GenericConfigPage(Composite parent, String category, InifileEditor inifileEditor) {
		super(parent, inifileEditor);
		setLayout(new GridLayout(1,false));
		
		// populate with field editors
		IInifileDocument doc = getInifileDocument();
		for (ConfigurationEntry e : ConfigurationRegistry.getEntries()) {
			if (e.getCategory().equals(category)) {
				String label = "The \""+e.getName()+"\" setting";
				if (e.getType()==ConfigurationEntry.Type.CFG_BOOL) {
					CheckboxFieldEditor control = new CheckboxFieldEditor(this, SWT.NONE, e, doc, label);
					control.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
				}
				else {
					TextFieldEditor control = new TextFieldEditor(this, SWT.NONE, e, doc, label);
					control.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
				}
			}
		}
	}
	
	@Override
	public void reread() {
		for (Control c : getChildren())  //XXX temp code
			if (c instanceof FieldEditor)
				((FieldEditor) c).reread();
	}

	@Override
	public void commit() {
		for (Control c : getChildren())  //XXX temp code
			if (c instanceof FieldEditor)
				((FieldEditor) c).commit();
	}
}
