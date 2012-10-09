package org.omnetpp.common.ui;


import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

/**
 * MessageDialog containing a scrollable text field for longer messages.
 *
 * @author tomi, andras
 */
public class DetailMessageDialog extends MessageDialog {
    private String detailsMessage;
    private Text detailsText;
    private Button detailsButton;
    private boolean detailsShown = false;

    public DetailMessageDialog(Shell parentShell, String dialogTitle,
            Image dialogTitleImage, String shortDialogMessage, String detailsMessage, int dialogImageType,
            String[] dialogButtonLabels, int defaultIndex) {
        super(parentShell, dialogTitle, dialogTitleImage, shortDialogMessage,
                dialogImageType, dialogButtonLabels, defaultIndex);
        this.detailsMessage = detailsMessage;
    }

    /**
     * Convenience method to open a simple confirm (OK/Cancel) dialog.
     *
     * @param parent
     *            the parent shell of the dialog, or <code>null</code> if none
     * @param title
     *            the dialog's title, or <code>null</code> if none
     * @param message
     *            the message
     * @return <code>true</code> if the user presses the OK button,
     *         <code>false</code> otherwise
     */
    public static boolean openConfirm(Shell parent, String title, String message, String detailsMessage) {
        MessageDialog dialog = new DetailMessageDialog(parent, title, null,
            message, detailsMessage, QUESTION, new String[] { IDialogConstants.OK_LABEL,
            IDialogConstants.CANCEL_LABEL }, 0);
        return dialog.open() == 0;
    }

    /**
     * Convenience method to open a standard error dialog.
     *
     * @param parent
     *            the parent shell of the dialog, or <code>null</code> if none
     * @param title
     *            the dialog's title, or <code>null</code> if none
     * @param message
     *            the message
     */
    public static void openError(Shell parent, String title, String message, String detailsMessage) {
        MessageDialog dialog = new DetailMessageDialog(parent, title, null,
            message, detailsMessage, ERROR, new String[] { IDialogConstants.OK_LABEL }, 0);
        dialog.open();
        return;
    }

    /**
     * Convenience method to open a standard information dialog.
     *
     * @param parent
     *            the parent shell of the dialog, or <code>null</code> if none
     * @param title
     *            the dialog's title, or <code>null</code> if none
     * @param message
     *            the message
     */
    public static void openInformation(Shell parent, String title, String message, String detailsMessage) {
        MessageDialog dialog = new DetailMessageDialog(parent, title, null,
                message, detailsMessage, INFORMATION,
                new String[] { IDialogConstants.OK_LABEL }, 0);
        dialog.open();
        return;
    }

    /**
     * Convenience method to open a simple Yes/No question dialog.
     *
     * @param parent
     *            the parent shell of the dialog, or <code>null</code> if none
     * @param title
     *            the dialog's title, or <code>null</code> if none
     * @param message
     *            the message
     * @return <code>true</code> if the user presses the OK button,
     *         <code>false</code> otherwise
     */
    public static boolean openQuestion(Shell parent, String title, String message, String detailsMessage) {
        MessageDialog dialog = new DetailMessageDialog(parent, title, null,
                message, detailsMessage, QUESTION, new String[] { IDialogConstants.YES_LABEL,
                        IDialogConstants.NO_LABEL }, 0);
        return dialog.open() == 0;
    }

    /**
     * Convenience method to open a standard warning dialog.
     *
     * @param parent
     *            the parent shell of the dialog, or <code>null</code> if none
     * @param title
     *            the dialog's title, or <code>null</code> if none
     * @param message
     *            the message
     */
    public static void openWarning(Shell parent, String title, String message, String detailsMessage) {
        MessageDialog dialog = new DetailMessageDialog(parent, title, null,
                message, detailsMessage, WARNING, new String[] { IDialogConstants.OK_LABEL }, 0);
        dialog.open();
        return;
    }

    @Override
    protected Control createCustomArea(Composite parent) {
        if (detailsMessage != null) {
            Composite composite = new Composite(parent, SWT.NONE);
            composite.setLayoutData(new GridData(GridData.FILL_BOTH));
            composite.setLayout(new GridLayout());

            detailsButton = new Button(composite, SWT.PUSH);
            detailsButton.setLayoutData(new GridData(SWT.END, SWT.CENTER, true, false));
            detailsButton.setText("Show Details"); // initially

            detailsText = new Text(parent, SWT.READ_ONLY | SWT.BORDER | SWT.MULTI | SWT.WRAP | SWT.V_SCROLL);
            GridData gridData = new GridData(GridData.FILL_BOTH);
            gridData.heightHint = 300;
            gridData.widthHint = 400;
            gridData.exclude = true; // initially
            detailsText.setLayoutData(gridData);
            detailsText.setText(detailsMessage);

            detailsButton.addSelectionListener(new SelectionListener() {
                public void widgetDefaultSelected(SelectionEvent e) {
                    detailsShown = !detailsShown;
                    updateDetailsState();
                }
                public void widgetSelected(SelectionEvent e) {
                    detailsShown = !detailsShown;
                    updateDetailsState();
                }
            });
            // updateDetailsState(); -- don't! (breaks dialog size)
            return composite;
        }
        else
            return null;
    }

    protected void updateDetailsState() {
        detailsButton.setText(detailsShown ? "Hide Details" : "Show Details");
        ((GridData)detailsText.getLayoutData()).exclude = !detailsShown;
        getShell().layout(true);
        getShell().setSize(getShell().computeSize(SWT.DEFAULT, SWT.DEFAULT));
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        super.createButtonsForButtonBar(parent);
        getButton(IDialogConstants.OK_ID).setFocus();
    }
}
