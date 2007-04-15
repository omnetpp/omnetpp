package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.ui.ITooltipProvider;
import org.omnetpp.common.ui.TooltipSupport;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;

/**
 * Base class for inifile field editors
 * @author Andras
 */
public abstract class FieldEditor extends Composite {
	public static final Color BGCOLOR = InifileFormEditor.BGCOLOR;

	protected ConfigurationEntry entry;
	protected IInifileDocument inifile;
	
	protected TooltipSupport tooltipSupport;

	public FieldEditor(Composite parent, int style, ConfigurationEntry entry, IInifileDocument inifile) {
		super(parent, style);
		this.entry = entry;
		this.inifile = inifile;
		setBackground(BGCOLOR);

		tooltipSupport = new TooltipSupport(new ITooltipProvider() {
			public String getTooltipFor(Control control, int x, int y) {
				return getTooltipText();
			}
		});
	}

	protected String getValueFromFile(String section) {
		String key = entry.getKey();
		return inifile.getValue(section, key);
	}

	protected void setValueInFile(String section, String value) {
		String key = entry.getKey();
		if (!inifile.containsKey(section, key))
			InifileUtils.addEntry(inifile, section, key, value, null);
		else
			inifile.setValue(section, key, value);
	}

	protected void removeFromFile(String section) {
		String key = entry.getKey();
		inifile.removeKey(section, key);
	}
	
	protected Label createLabel(ConfigurationEntry entry, String labelText) {
		Label label = new Label(this, SWT.NONE);
		label.setBackground(BGCOLOR);
		label.setText(labelText);
		tooltipSupport.adapt(label);
		return label;
	}

	protected String getTooltipText() {
		String tooltip = entry.getDescription();
		tooltip += "\n\nConfigures: [General]"+(entry.isGlobal() ? "" : " or [Config X]")+" / "+entry.getKey()+"=...";
		IInifileDocument.LineInfo line = inifile.getEntryLineDetails(GENERAL, entry.getKey()); 
		tooltip += "\n\n"+(line==null ? "Currently set to default." : "Defined at: "+line.getFile().getFullPath().toString()+" line "+line.getLineNumber());
		tooltip = StringUtils.breakLines(tooltip, 80);
		return tooltip;
	}

	protected Button createResetButton() {
		Button resetButton = new Button(this, SWT.PUSH);
		resetButton.setText("Reset");
		resetButton.setToolTipText("Remove corresponding entry from ini file");
		resetButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				resetToDefault();
			}
		});
		return resetButton;
	}

	protected void addFocusListenerTo(Control control) {
		control.addFocusListener(new FocusListener() {
			public void focusGained(FocusEvent e) {
				//XXX position the cursor on the edited entry, and fire InifileEditor selection change
			}

			public void focusLost(FocusEvent e) {
				commit();
			}
		});
	}

	public abstract void reread();
	
	public abstract void commit();
	
	public void resetToDefault() {
		removeFromFile(GENERAL);  //XXX into subclass?
		reread();
	}
	
}
