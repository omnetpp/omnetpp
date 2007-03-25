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
 * Form page for editing fields for a given section. Just throws in 
 * editors for all fields that are in the given section. This, of course,
 * requires section names to be listed as nodes in the form editor's tree.
 * 
 * Currently unused.
 * 
 * @author Andras
 */
public class SectionBasedConfigPage extends FormPage {
	
	public SectionBasedConfigPage(Composite parent, String sectionName, InifileEditor inifileEditor) {
		super(parent, inifileEditor);
		setLayout(new GridLayout(1,false));
		
		// populate with field editors
		IInifileDocument doc = getInifileDocument();
		for (ConfigurationEntry e : ConfigurationRegistry.getEntries()) {
			if (e.getSection().equals(sectionName)) {
				String label = "The \""+e.getName()+"\" setting";
				if (e.getType()==ConfigurationEntry.Type.CFG_BOOL) {
					CheckboxFieldEditor control = new CheckboxFieldEditor(this, e, doc, label);
					control.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
				}
				else {
					TextFieldEditor control = new TextFieldEditor(this, e, doc, label);
					control.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
				}
			}
		}

//		//XXX temp
//		for (String cat : new String[] {
//				ConfigurationRegistry.CAT_GENERAL, 
//				ConfigurationRegistry.CAT_PARAMETERS, 
//				ConfigurationRegistry.CAT_RANDOMNUMBERS,
//				ConfigurationRegistry.CAT_DEBUGGING,
//				ConfigurationRegistry.CAT_EXECUTION,
//				ConfigurationRegistry.CAT_ADVANCED,
//				ConfigurationRegistry.CAT_CMDENV,
//				ConfigurationRegistry.CAT_TKENV,
//				ConfigurationRegistry.CAT_PARSIM,
//				ConfigurationRegistry.CAT_OUTPUTFILES,
//				ConfigurationRegistry.CAT_OUTPUTVECTORS,
//		}) { 
//			System.out.println("if (category.equals(\""+cat+"\")) {");
//			for (ConfigurationEntry e : ConfigurationRegistry.getEntries()) {
//				if (e.getCategory().equals(cat)) {
//					String id = e.getName();
//					id = id.replaceAll("-", "_");
//					id = id.toUpperCase();
//					id = "CFGID_" + id;
//					String clazz = (e.getType()==ConfigurationEntry.Type.CFG_BOOL) ? "CheckboxFieldEditor" : "TextFieldEditor"; 
//					System.out.println("\taddField(new "+clazz+"(this, SWT.NONE, "+id+", doc, \""+e.getName()+"\"));");
//				}
//			}
//			System.out.print("}\nelse ");
//		}
	
	}
	
	@Override
	public void reread() {
		for (Control c : getChildren())
			if (c instanceof FieldEditor)
				((FieldEditor) c).reread();
	}

	@Override
	public void commit() {
		for (Control c : getChildren())
			if (c instanceof FieldEditor)
				((FieldEditor) c).commit();
	}
}
