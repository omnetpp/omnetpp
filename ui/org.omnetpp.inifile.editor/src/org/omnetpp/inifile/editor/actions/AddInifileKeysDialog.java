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
    private final static int SIZING_SELECTION_WIDGET_HEIGHT = 250;
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
        setTitle("Select Something"); //XXX
        inputElement = new String[] {"one", "two", "three"};
        this.contentProvider = new ArrayContentProvider();
        this.labelProvider = new LabelProvider();
        setMessage(message);
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

        
        listViewer = CheckboxTableViewer.newCheckList(composite, SWT.BORDER);
        GridData data = new GridData(GridData.FILL_BOTH);
        data.heightHint = SIZING_SELECTION_WIDGET_HEIGHT;
        data.widthHint = SIZING_SELECTION_WIDGET_WIDTH;
        listViewer.getTable().setLayoutData(data);

        listViewer.setLabelProvider(labelProvider);
        listViewer.setContentProvider(contentProvider);

        addSelectionButtons(composite);

        initializeViewer();

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
     * Initializes this dialog's viewer after it has been laid out.
     */
    private void initializeViewer() {
        listViewer.setInput(inputElement);
    }

    /**
     * The <code>ListSelectionDialog</code> implementation of this 
     * <code>Dialog</code> method builds a list of the selected elements for later
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
