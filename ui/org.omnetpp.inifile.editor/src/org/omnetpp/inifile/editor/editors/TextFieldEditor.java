package org.omnetpp.inifile.editor.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

public class TextFieldEditor extends FieldEditor {
	private Text textField;
	private Label label;

	public TextFieldEditor(Composite parent, int style, ConfigurationEntry entry, IInifileDocument inifile) {
		super(parent, style, entry, inifile);

		label = new Label(this, SWT.NONE);
		label.setText("["+entry.getSection()+(entry.isGlobal() ? "" : "] or [Run X")+"] "+entry.getName());
		label.setToolTipText(StringUtils.breakLines(entry.getDescription(),60));
		label.setBackground(BGCOLOR);
		textField = new Text(this, SWT.SINGLE | SWT.BORDER);
		//textField.setBackground(BGCOLOR);

		setLayout(new GridLayout(2, false));
		label.setLayoutData(new GridData());
		textField.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, true, false));

		reread();

		textField.addSelectionListener(new SelectionAdapter() {
			public void widgetDefaultSelected(SelectionEvent e) {
				setAsString(textField.getText());  //XXX rather, on focusOut or something like that?
			}
		});
		
	}

	public void reread() {
		String text = getAsString();
		if (text==null) return; //XXX for now -- should set default, probably
		textField.setText(text);
	}
}
