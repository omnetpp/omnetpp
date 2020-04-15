package org.omnetpp.scave.actions;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;
import org.omnetpp.common.util.StringUtils;

/**
 * TODO
 *
 * @author andras
 */
public class InputFileDialog extends Dialog {
    private String title;
    private String message;
    private String value = "";
    private IInputValidator validator;
    private Button okButton;
    private Text text;
    private Text errorMessageText;
    private String errorMessage;
    private IContainer baseDir;
    private boolean allowMultiple;

    private class InputValidator implements IInputValidator {
        public String isValid(String text) {
            text = text.trim();
            if (text.length() == 0)
                return "Input cannot be empty";
            if (!allowMultiple && text.contains(";"))
                return "Specify one item only (no semicolon)";
            return null;
        }
    }

    private static class FileOrFolderMultiSelectionDialog extends ElementTreeSelectionDialog {
        public FileOrFolderMultiSelectionDialog(Shell parent, String title, String message) {
            super(parent, new WorkbenchLabelProvider(), new WorkbenchContentProvider());
            setComparator(new ResourceComparator(ResourceComparator.NAME));
            setTitle(title);
            setMessage(message);
        }
    }

    public InputFileDialog(Shell parentShell, String title, String initialValue, boolean allowMultiple, IContainer baseDir) {
        super(parentShell);
        this.title = title;
        this.message = "Enter result file name" + (allowMultiple ? "(s)" : "") + " or directory. Wildcards (**,*,?) are accepted.";
        this.value = initialValue;
        this.allowMultiple = allowMultiple;
        this.baseDir = baseDir;
        this.validator = new InputValidator();
    }

    @Override
    protected void buttonPressed(int buttonId) {
        if (buttonId == IDialogConstants.OK_ID)
            value = text.getText();
        else
            value = null;
        super.buttonPressed(buttonId);
    }

    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        if (title != null)
            shell.setText(title);
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        // create OK and Cancel buttons by default
        Button browseButton = createButton(parent, IDialogConstants.CLIENT_ID+1, "Browse...", true);
        browseButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                browseButtonPressed();
            }
        });

        okButton = createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);

        // do this here because setting the text will set enablement on the ok button
        text.setFocus();
        if (value != null) {
            text.setText(value);
            text.selectAll();
        }
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        // create composite
        Composite composite = (Composite) super.createDialogArea(parent);
        // create message
        if (message != null) {
            Label label = new Label(composite, SWT.WRAP);
            label.setText(message);
            GridData data = new GridData(SWT.FILL, SWT.CENTER, true, true);
            data.widthHint = convertHorizontalDLUsToPixels(IDialogConstants.MINIMUM_MESSAGE_AREA_WIDTH);
            label.setLayoutData(data);
            label.setFont(parent.getFont());
        }
        text = new Text(composite, SWT.SINGLE | SWT.BORDER);
        text.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL | GridData.HORIZONTAL_ALIGN_FILL));
        text.addModifyListener(e -> validateInput());
        errorMessageText = new Text(composite, SWT.READ_ONLY | SWT.WRAP);
        errorMessageText.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL | GridData.HORIZONTAL_ALIGN_FILL));
        errorMessageText.setBackground(errorMessageText.getDisplay().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND));
        setErrorMessage(errorMessage);

        applyDialogFont(composite);
        return composite;
    }

    protected Button getOkButton() {
        return okButton;
    }

    protected Text getText() {
        return text;
    }

    protected IInputValidator getValidator() {
        return validator;
    }

    public String getValue() {
        return value;
    }

    protected void validateInput() {
        String errorMessage = null;
        if (validator != null)
            errorMessage = validator.isValid(text.getText());
        setErrorMessage(errorMessage);
    }

    protected void browseButtonPressed() {
        FileOrFolderMultiSelectionDialog dialog = new FileOrFolderMultiSelectionDialog(InputFileDialog.this.getShell(), "Selection", "Select result file(s) or folder(s)");
        dialog.setAllowMultiple(allowMultiple);
        dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
        if (dialog.open() == Dialog.OK) {
            List<String> list = new ArrayList<>();
            for (Object object : dialog.getResult()) {
                if (object instanceof IAdaptable)
                    object = ((IAdaptable) object).getAdapter(IResource.class);
                if (object instanceof IResource) {
                    IResource resource = (IResource)object;
                    list.add(makeRelative(resource.getFullPath()).toString());
                }
            }

            text.setText(StringUtils.join(list, "; "));
        }
    }

    protected IPath makeRelative(IPath path) {
        IPath basePath = baseDir.getFullPath();
        if (path.equals(basePath))
            return new Path(".");
        else if (basePath.isPrefixOf(path))
            return path.removeFirstSegments(basePath.segmentCount());
        else
            return path;
    }

    public void setErrorMessage(String errorMessage) {
        this.errorMessage = errorMessage;
        errorMessageText.setText(errorMessage == null ? " \n " : errorMessage);
        boolean hasError = !StringUtils.isBlank(errorMessage);
        errorMessageText.setEnabled(hasError);
        errorMessageText.setVisible(hasError);
        errorMessageText.getParent().update();
        if (okButton != null)
            okButton.setEnabled(errorMessage == null);
    }

}
