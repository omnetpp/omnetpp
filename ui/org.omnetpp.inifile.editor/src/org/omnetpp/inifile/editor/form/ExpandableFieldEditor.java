package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.IInifileChangeListener;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * An inifile field editor, which displays either a TextFieldEditor or a TableFieldEditor,
 * and lets the user toggle between them. 
 * 
 * @author Andras
 */
//XXX when focus is in the table, form cannot be scrolled. why??
public abstract class ExpandableFieldEditor extends FieldEditor  {
	private static Image IMAGE_EXPAND = InifileEditorPlugin.getCachedImage("icons/full/elcl16/expand.png");
	private static Image IMAGE_COLLAPSE = InifileEditorPlugin.getCachedImage("icons/full/elcl16/collapse.png");
	
	protected String labelText;
	protected FieldEditor fieldEditor;
	protected boolean isExpanded;
	protected ToolItem toggleButton;
	
	private IInifileChangeListener inifileChangeListener = new IInifileChangeListener() {
		public void modelChanged() {
			updateToggleButton();
		}
	};

	public ExpandableFieldEditor(Composite parent, ConfigKey entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, SWT.NONE, entry, inifile, formPage);
		this.labelText = labelText;
		GridLayout gridLayout = new GridLayout(2, false);
		gridLayout.marginHeight = gridLayout.marginWidth = 0;
		setLayout(gridLayout);

		isExpanded = (!entry.isGlobal() && !canBeCollapsed());
		createControl();
		
		// need to update the toggle button's state on inifile changes
		inifile.addInifileChangeListener(inifileChangeListener);
		addDisposeListener(new DisposeListener() {
			public void widgetDisposed(DisposeEvent e) {
				ExpandableFieldEditor.this.inifile.removeInifileChangeListener(inifileChangeListener);
			}
		});
	}

	protected void recreateControl() {
		for (Control c : getChildren())
			c.dispose();
		createControl();
		formPage.layoutForm();
	}

	protected void createControl() {
		fieldEditor = createFieldEditor(isExpanded);
		fieldEditor.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
				
		toggleButton = createFlatImageButton(this);
		toggleButton.setImage(isExpanded ? IMAGE_COLLAPSE : IMAGE_EXPAND);
		toggleButton.setToolTipText(isExpanded ? "Specify value globally, in the [General] section": "Specify values individually for different sections");
		toggleButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				isExpanded = !isExpanded;
				recreateControl();
			}
		});
		updateToggleButton();
	}

	abstract protected FieldEditor createFieldEditor(boolean isExpanded);

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
	
	protected boolean canBeCollapsed() {
	    if (!entry.isPerObject()) {
	        for (String section : inifile.getSectionNames())
	            if (inifile.containsKey(section, entry.getKey()) && !section.equals(GENERAL))
	                return false;
	    } 
	    else {
	        // if it contains any occurrence of this key except **.<key> in [General], it cannot be collapsed
            for (String section : inifile.getSectionNames())
                for (String key : inifile.getKeys(section))
                    if (key.endsWith("."+entry.getKey()) && (!section.equals(GENERAL) || !key.equals("**."+entry.getKey())))
                        return false;
	    }
		return true;
	}
	
	@Override
	public void commit() {
		fieldEditor.commit();
	}

	@Override
	public void reread() {
		fieldEditor.reread();
	}

	protected void updateToggleButton() {
		// need to update the toggle button's state when the last per-section value is removed
		boolean shouldBeEnabled = !isExpanded || canBeCollapsed();
		if (toggleButton.getEnabled() != shouldBeEnabled)
			toggleButton.setEnabled(shouldBeEnabled);
	}

	public boolean isExpanded() {
	    return isExpanded;
	}
	
	public FieldEditor getInnerFieldEditor() {
	    return fieldEditor;
	}
}
