package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.dialogs.MessageDialog;
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
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.ConfigKey;
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
	public static final Image ICON_ERROR_SMALL = InifileEditorPlugin.getCachedImage("icons/full/ovr16/error2.gif");
	public static final Image ICON_WARNING_SMALL = InifileEditorPlugin.getCachedImage("icons/full/ovr16/warning.gif");
	public static final Image ICON_INFO_SMALL = InifileEditorPlugin.getCachedImage("icons/full/ovr16/info.gif");
	public static final Image ICON_ERROR = InifileEditorPlugin.getCachedImage("icons/full/obj16/Error.png");
	public static final Image ICON_WARNING = InifileEditorPlugin.getCachedImage("icons/full/obj16/Warning.png");
	public static final Image ICON_INFO = InifileEditorPlugin.getCachedImage("icons/full/obj16/Info.gif");
	
	protected ConfigKey entry;
	protected IInifileDocument inifile;
	protected FormPage formPage; // to access hoverSupport, and to be able to call setEditorSelection()
	
	protected IHoverTextProvider hoverTextProvider = new IHoverTextProvider() {
		public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
			return getTooltipText();
		}
	};

	public FieldEditor(Composite parent, int style, ConfigKey entry, IInifileDocument inifile, FormPage formPage) {
		super(parent, style);
		this.entry = entry;
		this.inifile = inifile;
		this.formPage = formPage;
		setBackground(BGCOLOR);

	}

	protected String getValueFromFile(String section) {
		String key = entry.getKey();
		return inifile.getValue(section, key);
	}

	protected void setValueInFile(String section, String value) {
		try {
			String key = entry.getKey();
			if (!inifile.containsKey(section, key))
				InifileUtils.addEntry(inifile, section, key, value, "");
			else
				inifile.setValue(section, key, value);
		} 
		catch (RuntimeException e) {
			showErrorDialog(e);
		}
	}

	protected void removeFromFile(String section) {
		try {
			String key = entry.getKey();
			inifile.removeKey(section, key);
		}
		catch (RuntimeException e) {
			showErrorDialog(e);
		}
	}
	
	protected void showErrorDialog(RuntimeException e) {
	    try {
	        reread(); // restore "legal" widget contents; must be done first, or error dialog will pop up twice
	    } 
	    catch (RuntimeException e2) {
	        InifileEditorPlugin.logError("error while handling error \""+e.getMessage()+"\"", e2);
	        MessageDialog.openError(getShell(), "Error", e.getMessage()+".\nAlso: unable to re-parse document after the error: "+e2.getMessage());
	        return;
	    }
		MessageDialog.openError(getShell(), "Error", e.getMessage()+".");
	}

	protected Label createLabel(ConfigKey entry, String labelText) {
		Label label = new Label(this, SWT.NONE);
		label.setBackground(BGCOLOR);
		label.setText(labelText);
		addTooltipSupport(label);
		return label;
	}

	protected void addTooltipSupport(Control control) {
		formPage.addTooltipSupport(control, hoverTextProvider);
	}
	
	protected String getTooltipText() {
		return InifileHoverUtils.getConfigHoverText(GENERAL, entry.getKey(), inifile);
	}

	protected Button createResetButton() {
		Button resetButton = new Button(this, SWT.PUSH);
		resetButton.setText("Reset");
		resetButton.setToolTipText("Reset value to default, by removing corresponding entry from the document");
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
		removeFromFile(GENERAL);
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

	/**
	 * Returns the configuration key this editor edits.
	 */
	public ConfigKey getConfigKey() {
		return entry;
	}
}
