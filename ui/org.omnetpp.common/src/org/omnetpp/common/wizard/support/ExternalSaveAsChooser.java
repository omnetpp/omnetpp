package org.omnetpp.common.wizard.support;

import java.io.File;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.wizard.IWidgetAdapter;

/**
 * A control for prompting for a file system file name to be saved.
 * Implemented as a Composite with a single-line Text and a Browse button.
 *
 * @author Andras
 */
public class ExternalSaveAsChooser extends Composite implements IWidgetAdapter {
    private Text text;
    private Button browseButton;

    public ExternalSaveAsChooser(Composite parent, int style) {
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
        FileDialog dialog = new FileDialog(getShell(), SWT.SAVE);

        String fileName = getTextControl().getText().trim();
        dialog.setFileName(fileName);

        String result = dialog.open();
        return result;
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
        if (value instanceof File)
            setText(((File)value).getPath());
        else
            setText(value.toString());
    }

}
