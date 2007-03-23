package org.omnetpp.inifile.editor.editors;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

public class CheckboxFieldEditor extends FieldEditor {
	private Button checkbox;
	private Label label;

	public CheckboxFieldEditor(Composite parent, int style, ConfigurationEntry entry, IInifileDocument inifile) {
		super(parent, style, entry, inifile);

		Assert.isTrue(entry.getType()==ConfigurationEntry.Type.CFG_BOOL);
		checkbox = new Button(this, SWT.CHECK);
		label = new Label(this, SWT.NONE);
		setLayout(new GridLayout(2, false));
		checkbox.setLayoutData(new GridData());
		label.setLayoutData(new GridData());
		label.setText("["+entry.getSection()+(entry.isGlobal() ? "" : "] or [Run X")+"] "+entry.getName());
		label.setToolTipText(StringUtils.breakLines(entry.getDescription(),60));

		reread();

		checkbox.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				setAsString(checkbox.getSelection() ? "true" : "false");
			}
		});
		
	}

	public void reread() {
		String text = getAsString();
		if (text==null) return; //XXX for now -- should set default, probably
		//XXX create some InifileUtils for parsing?
		boolean value;
	    if (text.equals("yes") || text.equals("true") || text.equals("on") || text.equals("1"))
	    	value = true;
	    else if (text.equals("no") || text.equals("false") || text.equals("off") || text.equals("0"))
	    	value = false;
	    else
	    	value = false; //XXX something invalid, we take it as false; probably should warn the user or something
	    checkbox.setSelection(value);
	}
}
