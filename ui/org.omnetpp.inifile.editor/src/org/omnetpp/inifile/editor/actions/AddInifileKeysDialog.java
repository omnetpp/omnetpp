package org.omnetpp.inifile.editor.actions;

import java.util.ArrayList;
import java.util.Iterator;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.SelectionDialog;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;


/**
 * A standard dialog which solicits a list of selections from the user.
 * This class is configured with an arbitrary data model represented by content
 * and label provider objects. The <code>getResult</code> method returns the
 * selected elements.
 */
public class AddInifileKeysDialog extends SelectionDialog {
	// table input and providers for populating this dialog
    private Object inputElement;
    private ILabelProvider labelProvider;
    private IStructuredContentProvider contentProvider;

    // the visual selection widget group
    CheckboxTableViewer listViewer;

    // sizing constants
    private final static int SIZING_SELECTION_WIDGET_HEIGHT = 80;
    private final static int SIZING_SELECTION_WIDGET_WIDTH = 300;

    /**
     * Creates a list selection dialog.
     *
     * @param parentShell the parent shell
     * @param input	the root element to populate this dialog with
     * @param contentProvider the content provider for navigating the model
     * @param labelProvider the label provider for displaying model elements
     * @param message the message to be displayed at the top of this dialog, or
     *    <code>null</code> to display a default message
     */
    public AddInifileKeysDialog(Shell parentShell, InifileAnalyzer analyzer, String message) {
        super(parentShell);
        setTitle("Generate Inifile Contents"); //XXX
        inputElement = new String[] {"one", "two", "three"};
        this.contentProvider = new ArrayContentProvider();
        this.labelProvider = new LabelProvider();
        setMessage(message!=null ? message : "Choose keys to be added to the file.");
    }

    /**
     * Add the selection and deselection buttons to the dialog.
     * @param composite org.eclipse.swt.widgets.Composite
     */
    private void addSelectionButtons(Composite composite) {
        Composite buttonComposite = new Composite(composite, SWT.NONE);
        GridLayout layout = new GridLayout();
        layout.numColumns = 0;
		layout.marginWidth = 0;
		layout.horizontalSpacing = convertHorizontalDLUsToPixels(IDialogConstants.HORIZONTAL_SPACING);
        buttonComposite.setLayout(layout);
        buttonComposite.setLayoutData(new GridData(SWT.END, SWT.TOP, true, false));

        Button selectButton = createButton(buttonComposite, IDialogConstants.SELECT_ALL_ID, "Select All", false);

        SelectionListener listener = new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                listViewer.setAllChecked(true);
            }
        };
        selectButton.addSelectionListener(listener);

        Button deselectButton = createButton(buttonComposite, IDialogConstants.DESELECT_ALL_ID, "Deselect All", false);

        listener = new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                listViewer.setAllChecked(false);
            }
        };
        deselectButton.addSelectionListener(listener);
    }

    /**
     * Visually checks the previously-specified elements in this dialog's list 
     * viewer.
     */
    private void checkInitialSelections() {
        Iterator itemsToCheck = getInitialElementSelections().iterator();

        while (itemsToCheck.hasNext()) {
			listViewer.setChecked(itemsToCheck.next(), true);
		}
    }

    /* (non-Javadoc)
     * Method declared on Dialog.
     */
    protected Control createDialogArea(Composite parent) {
        // page group
        Composite composite = (Composite) super.createDialogArea(parent);
        initializeDialogUnits(composite);
        createMessageArea(composite);

        Button applyDefault = new Button(composite, SWT.CHECK);
        applyDefault.setText("Apply default value of parameters that have one");
        
		// radiobutton group
        Group group = new Group(composite, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		group.setText("Pattern style");
		group.setLayout(new GridLayout(1, false));

        // radiobuttons
		Button b1 = new Button(group, SWT.RADIO);
        Button b2 = new Button(group, SWT.RADIO);
        Button b3 = new Button(group, SWT.RADIO);
        b1.setText("Parameter name only (**.queueSize)");
        b2.setText("Module and parameter only (**.mac.queueSize)");
        b3.setText("Full path (Network.host[*].mac.queueSize)");
        
		// table group
        Group group2 = new Group(composite, SWT.NONE);
		group2.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		group2.setText("Keys to insert");
		group2.setLayout(new GridLayout(1, false));
        
        // table and buttons
		listViewer = CheckboxTableViewer.newCheckList(group2, SWT.BORDER);
        GridData data = new GridData(GridData.FILL_BOTH);
        data.heightHint = SIZING_SELECTION_WIDGET_HEIGHT;
        data.widthHint = SIZING_SELECTION_WIDGET_WIDTH;
        listViewer.getTable().setLayoutData(data);

        listViewer.setLabelProvider(labelProvider);
        listViewer.setContentProvider(contentProvider);
        listViewer.setInput(inputElement);

        addSelectionButtons(group2);

        // initialize page
        if (!getInitialElementSelections().isEmpty()) {
			checkInitialSelections();
		}

        Dialog.applyDialogFont(composite);
        
        return composite;
    }

    /**
     * Returns the viewer used to show the list.
     */
    protected CheckboxTableViewer getViewer() {
        return listViewer;
    }

    /**
     * Builds a list of the selected elements for later
     * retrieval by the client and closes this dialog.
     */
    @SuppressWarnings("unchecked")
	protected void okPressed() {
        // Get the input children.
        Object[] children = contentProvider.getElements(inputElement);

        // Build a list of selected children.
        if (children != null) {
            ArrayList list = new ArrayList();
            for (Object element : children)
                if (listViewer.getChecked(element))
					list.add(element);
            setResult(list);
        }

        super.okPressed();
    }
}
