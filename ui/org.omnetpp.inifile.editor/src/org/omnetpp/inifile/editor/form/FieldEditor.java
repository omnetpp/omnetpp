/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.InifileUtils;

/**
 * Base class for inifile field editors
 * @author Andras
 */
public abstract class FieldEditor extends Composite {
    // colors and icons for subclasses
    public static final Image ICON_ERROR_SMALL = UIUtils.ICON_ERROR_SMALL;
    public static final Image ICON_WARNING_SMALL = UIUtils.ICON_WARNING_SMALL;
    public static final Image ICON_INFO_SMALL = UIUtils.ICON_INFO_SMALL;
    public static final Image ICON_ERROR = UIUtils.ICON_ERROR;
    public static final Image ICON_WARNING = UIUtils.ICON_WARNING;
    public static final Image ICON_INFO = UIUtils.ICON_INFO;
    private static final Image ICON_RESET = InifileEditorPlugin.getCachedImage("icons/full/elcl16/reset.png");

    protected ConfigOption entry;
    protected IInifileDocument inifile;
    protected FormPage formPage; // to access hoverSupport, and to be able to call setEditorSelection()
    protected Map<String,Object> hints;  // various options for the field editor; may be null; see HINT_xxx constants

    protected IHoverInfoProvider hoverTextProvider = new IHoverInfoProvider() {
        public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
            return new HtmlHoverInfo(getTooltipText());
        }
    };

    public FieldEditor(Composite parent, int style, ConfigOption entry, IInifileDocument inifile, FormPage formPage, Map<String,Object> hints) {
        super(parent, style);
        this.entry = entry;
        this.inifile = inifile;
        this.formPage = formPage;
        this.hints = hints;
        setBackground(formPage.getBackground());

    }

    protected String getValueFromFile(String section, String key) {
        Assert.isTrue(entry.isPerObject() ? key.endsWith("."+entry.getName()) : key.equals(entry.getName()));
        return inifile.getValue(section, key);
    }

    protected void setValueInFile(String section, String key, String value) {
        Assert.isTrue(entry.isPerObject() ? key.endsWith("."+entry.getName()) : key.equals(entry.getName()));
        try {
            if (!inifile.containsKey(section, key))
                InifileUtils.addEntry(inifile, section, key, value, "");
            else
                inifile.setValue(section, key, value);
        }
        catch (RuntimeException e) {
            showErrorDialog(e);
        }
    }

    protected void renameKeyInInifile(String section, String key, String newKey) {
        Assert.isTrue(entry.isPerObject() ? key.endsWith("."+entry.getName()) : key.equals(entry.getName()));
        Assert.isTrue(entry.isPerObject() ? newKey.endsWith("."+entry.getName()) : newKey.equals(entry.getName()));
        try {
            inifile.renameKey(section, key, newKey);
        }
        catch (RuntimeException e) {
            showErrorDialog(e);
        }
    }

    protected void removeFromFile(String section, String key) {
        Assert.isTrue(entry.isPerObject() ? key.endsWith("."+entry.getName()) : key.equals(entry.getName()));
        try {
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

    protected String getStringHint(String key, String defaultValue) {
        return (String)getHint(key, defaultValue);
    }

    protected int getIntHint(String key, int defaultValue) {
        return (Integer)getHint(key, defaultValue);
    }

    protected Object getHint(String key, Object defaultValue) {
        if (hints != null && hints.containsKey(key))
            return hints.get(key);
        else
            return defaultValue;
    }

    protected Label createLabel(ConfigOption entry, String labelText) {
        Label label = new Label(this, SWT.NONE);
        label.setText(labelText);
        addTooltipSupport(label);
        return label;
    }

    protected void addTooltipSupport(Control control) {
        formPage.addTooltipSupport(control, hoverTextProvider);
    }

    protected String getTooltipText() {
        String name = entry.getName();
        String key = entry.isPerObject() && !name.contains(".") ? "**." + name : name;
        if (entry.isPerObject())
            return InifileHoverUtils.getPerObjectConfigHoverText(GENERAL, key, inifile);
        else
            return InifileHoverUtils.getConfigHoverText(GENERAL, key, inifile);
    }

    protected ToolItem createResetButton() {
        ToolItem resetButton = FieldEditor.createFlatImageButton(this);
        resetButton.setImage(ICON_RESET);
        resetButton.setToolTipText("Reset to default (remove setting from document)");
        resetButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                resetToDefault();
            }
        });
        return resetButton;
    }

    protected void addFocusListenerTo(Control control) {
        control.addFocusListener(new FocusListener() {
            public void focusGained(FocusEvent e) {
                formPage.setEditorSelection(GENERAL, entry.getName());
            }

            public void focusLost(FocusEvent e) {
                commit();
            }
        });
    }

    /**
     * Override in subclasses that have combobox-like behavior
     */
    public void setComboContents(List<String> values) {
        // do-nothing convenience implementation
    }

    public abstract void reread();

    public abstract void commit();

    public void resetToDefault() {
        removeFromFile(GENERAL, entry.isPerObject() ? "**."+entry.getName() : entry.getName());  //XXX
        reread();
    }

    protected static ToolItem createFlatImageButton(Composite parent) {
        // code from TrayDialog.createHelpImageButton()
        ToolBar toolBar = new ToolBar(parent, SWT.FLAT | SWT.NO_FOCUS);
        toolBar.setLayoutData(new GridData(SWT.CENTER, SWT.BEGINNING, false, false));
        final Cursor cursor = new Cursor(parent.getDisplay(), SWT.CURSOR_HAND);
        toolBar.setCursor(cursor);
        toolBar.addDisposeListener(new DisposeListener() {
            public void widgetDisposed(DisposeEvent e) {
                cursor.dispose();
            }
        });
        ToolItem item = new ToolItem(toolBar, SWT.NONE);
        return item;
    }

    /**
     * Utility function: sets up a mouse listener so when a composite or label is clicked,
     * another control (editfield etc) gets the focus.
     */
    protected static void addFocusTransfer(final Control widgetClicked, final Control widgetToFocus) {
        widgetClicked.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseDown(MouseEvent e) {
                widgetToFocus.setFocus();
            }
        });
    }

    protected static Image getProblemImage(IMarker[] markers, boolean small, boolean ignoreInfos) {
        int severity = InifileUtils.getMaximumSeverity(markers);
        switch (severity) {
            case IMarker.SEVERITY_ERROR: return small ? ICON_ERROR_SMALL : ICON_ERROR;
            case IMarker.SEVERITY_WARNING: return small ? ICON_WARNING_SMALL : ICON_WARNING;
            case IMarker.SEVERITY_INFO: return ignoreInfos ? null : small ? ICON_INFO_SMALL : ICON_INFO;
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
    public ConfigOption getConfigKey() {
        return entry;
    }
}
