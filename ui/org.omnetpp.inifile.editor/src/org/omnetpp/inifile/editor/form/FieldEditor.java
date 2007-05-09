package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import org.eclipse.core.resources.IMarker;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.ui.ITooltipTextProvider;
import org.omnetpp.common.ui.TooltipSupport;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.InifileUtils;

/**
 * Base class for inifile field editors
 * @author Andras
 */
public abstract class FieldEditor extends Composite {
	// colors and icons for subclasses
	public static final Color BGCOLOR = InifileFormEditor.BGCOLOR;
	public static final Image ICON_ERROR_SMALL = InifileEditorPlugin.getImage("icons/full/ovr16/error2.gif");
	public static final Image ICON_WARNING_SMALL = InifileEditorPlugin.getImage("icons/full/ovr16/warning.gif");
	public static final Image ICON_INFO_SMALL = InifileEditorPlugin.getImage("icons/full/ovr16/warning.gif"); //XXX
	public static final Image ICON_ERROR = InifileEditorPlugin.getImage("icons/full/obj16/Error.png");
	public static final Image ICON_WARNING = InifileEditorPlugin.getImage("icons/full/obj16/Warning.png");
	public static final Image ICON_INFO = InifileEditorPlugin.getImage("icons/full/obj16/Warning.png"); //XXX
	
	protected ConfigurationEntry entry;
	protected IInifileDocument inifile;
	protected FormPage formPage; //XXX unfortunately we need it, to be able to call setEditorSelection()
	
	protected TooltipSupport tooltipSupport;

	public FieldEditor(Composite parent, int style, ConfigurationEntry entry, IInifileDocument inifile, FormPage formPage) {
		super(parent, style);
		this.entry = entry;
		this.inifile = inifile;
		this.formPage = formPage;
		setBackground(BGCOLOR);

		tooltipSupport = new TooltipSupport(new ITooltipTextProvider() {
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
		return InifileHoverUtils.getConfigTooltip(entry, inifile);
//		String tooltip = entry.getDescription();
//		tooltip += "\n\nConfigures: [General]"+(entry.isGlobal() ? "" : " or [Config X]")+" / "+entry.getKey()+"=...";
//		IInifileDocument.LineInfo line = inifile.getEntryLineDetails(GENERAL, entry.getKey()); 
//		tooltip += "\n\n"+(line==null ? "Currently set to default." : "Defined at: "+line.getFile().getFullPath().toString()+" line "+line.getLineNumber());
//		tooltip = StringUtils.breakLines(tooltip, 80);
//		return tooltip;
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
				formPage.setEditorSelection(GENERAL, entry.getKey());
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

	/** 
	 * Utility function: sets up a mouse listener so when a composite or label is clicked,
	 * another control (editfield etc) gets the focus.
	 */
	protected static void addFocusTransfer(final Control widgetClicked, final Control widgetToFocus) {
		widgetClicked.addMouseListener(new MouseAdapter() {
			public void mouseDown(MouseEvent e) {
				widgetToFocus.setFocus();
			}
		});
	}
	
	protected static Image getProblemImage(IMarker[] markers, boolean small) {
		int severity = InifileUtils.getMaximumSeverity(markers);
		switch (severity) {
			case IMarker.SEVERITY_ERROR: return small ? ICON_ERROR_SMALL : ICON_ERROR;
			case IMarker.SEVERITY_WARNING: return small ? ICON_WARNING_SMALL : ICON_WARNING;
			case IMarker.SEVERITY_INFO: return small ? ICON_INFO_SMALL : ICON_INFO;
			default: return null;
		}
	}
	protected static String getProblemsText(IMarker[] markers) {
		if (markers.length==0) 
			return null;
		String text = "";
		for (IMarker marker : markers)
			text += marker.getAttribute(IMarker.MESSAGE, "") + "\n";
		return text.trim();
	}
	
}
