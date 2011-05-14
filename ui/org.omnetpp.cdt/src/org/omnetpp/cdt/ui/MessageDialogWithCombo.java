package org.omnetpp.cdt.ui;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ComboViewer;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;

/**
 * Message dialog with a combo on it.
 * 
 * @author Andras
 */
public class MessageDialogWithCombo extends MessageDialog {
    private String comboLabel;
    private Object comboInput;
    private ILabelProvider comboLabelProvider;
    private Object comboInitialSelection;
    private ComboViewer comboViewer;
    private Object result;

    public MessageDialogWithCombo(Shell parentShell, String dialogTitle, Image dialogTitleImage, String dialogMessage, String comboLabel,
            int dialogImageType, String[] dialogButtonLabels, int defaultIndex, 
            Object comboInput, ILabelProvider comboLabelProvider, Object comboInitialSelection) {
        super(parentShell, dialogTitle, dialogTitleImage, dialogMessage, dialogImageType, dialogButtonLabels, defaultIndex);
        this.comboLabel = comboLabel;
        this.comboInput = comboInput;
        this.comboLabelProvider = comboLabelProvider;
        this.comboInitialSelection = comboInitialSelection;
    }
    
    @Override
    protected Control createCustomArea(Composite parent) {
        Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayout(new GridLayout(3, false));
        Label spacer = new Label(composite, SWT.NONE);
        spacer.setLayoutData(new GridData());
        ((GridData)spacer.getLayoutData()).widthHint = 34; //XXX should be the icon width, sort of
        Label label = new Label(composite, SWT.NONE);
        label.setText(comboLabel);
        comboViewer = new ComboViewer(composite);
        comboViewer.setLabelProvider(comboLabelProvider);
        comboViewer.setContentProvider(new ArrayContentProvider());
        comboViewer.setInput(comboInput);
        comboViewer.setSelection(new StructuredSelection(comboInitialSelection));
        return comboViewer.getControl();
    }
    
    @Override
    protected void buttonPressed(int buttonId) {
        result = ((IStructuredSelection)comboViewer.getSelection()).getFirstElement();
        super.buttonPressed(buttonId);
    }
    
    public Object getResult() {
        return result;
    }

    /**
     * Opens the dialog. Returns the selected item, or null if the user cancelled the dialog.
     */
    public static Object openAndSelect(Shell parent, String title, String message, String comboLabel,
            Object comboInput, ILabelProvider comboLabelProvider, Object comboInitialSelection) {
        MessageDialogWithCombo dialog = new MessageDialogWithCombo(parent, title, null, message, comboLabel,
                QUESTION, new String[] { IDialogConstants.OK_LABEL, IDialogConstants.CANCEL_LABEL }, 0, 
                comboInput, comboLabelProvider, comboInitialSelection);
        return dialog.open() == 0 ? dialog.getResult() : null;
    }
    
}
