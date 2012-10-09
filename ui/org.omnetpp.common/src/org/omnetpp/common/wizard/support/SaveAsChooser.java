package org.omnetpp.common.wizard.support;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.SaveAsDialog;
import org.omnetpp.common.wizard.IWidgetAdapter;

/**
 * A control for prompting for a workspace file name to be saved.
 * Implemented as a Composite with a single-line Text and a Browse button.
 *
 * @author Andras
 */
public class SaveAsChooser extends Composite implements IWidgetAdapter {
    private Text text;
    private Button browseButton;

    public SaveAsChooser(Composite parent, int style) {
        super(parent, style);
        GridLayout layout = new GridLayout(2,false);
        layout.marginHeight = layout.marginWidth = 0;
        setLayout(layout);

        text = new Text(this, SWT.SINGLE|SWT.BORDER);
        text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, true));
        browseButton = new Button(this, SWT.PUSH);
        browseButton.setText("Browse...");

        browseButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                String newText = browse();
                if (newText != null) {
                    text.setText(newText);
                    text.selectAll();
                    text.setFocus();
                }
            }
        });
    }

    public Text getTextControl() {
        return text;
    }

    public Button getBrowseButton() {
        return browseButton;
    }

    @Override
    public boolean setFocus() {
        return getTextControl().setFocus();
    }

    public String getText() {
        return getTextControl().getText();
    }

    public void setText(String value) {
        getTextControl().setText(value);
        getTextControl().selectAll();
    }

    public String getFileName() {
        return getText();
    }

    public void setFileName(String file) {
        setText(file);
    }

    protected String browse() {
        SaveAsDialog dialog = new SaveAsDialog(getShell());

        // select current file in the dialog
        String fileName = getTextControl().getText().trim();
        if (!fileName.equals("")) {
            try {
                IFile initialSelection = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(fileName));
                dialog.setOriginalFile(initialSelection);
            }
            catch (IllegalArgumentException e) { } // on bad file name syntax
        }

        if (dialog.open() == IDialogConstants.OK_ID) {
            IPath result = dialog.getResult();
            if (result != null)
                return result.toString();
        }
        return null;
    }

    /**
     * Adapter interface.
     */
    public Object getValue() {
        return getText();
    }

    /**
     * Adapter interface.
     */
    public void setValue(Object value) {
        if (value instanceof IResource)
            setText(((IResource)value).getFullPath().toString());
        else
            setText(value.toString());
    }

}
