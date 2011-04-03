package org.omnetpp.common.ui;


import java.util.List;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;

/**
 * MessageDialog containing a table that displays several strings with an error icon.
 * 
 * @author andras
 */
public class ProblemsMessageDialog extends MessageDialog {
	private List<String> problemTexts;
	private Image problemImage;

	public ProblemsMessageDialog(Shell parentShell, String title, Image titleImage, String message, 
	        List<String> problemTexts, Image problemImage, int dialogImageType, String[] dialogButtonLabels, int defaultIndex) {
		super(parentShell, title, titleImage, message, dialogImageType, dialogButtonLabels, defaultIndex);
		this.problemTexts = problemTexts;
		this.problemImage = problemImage;
	}

    /**
     * Convenience method to open a simple confirm (OK/Cancel) dialog.
     */
    public static boolean openConfirm(Shell parent, String title, String message, List<String> problemTexts, Image problemImage) {
        MessageDialog dialog = new ProblemsMessageDialog(parent, title, null,
        	message, problemTexts, problemImage, QUESTION, new String[] { IDialogConstants.OK_LABEL,
            IDialogConstants.CANCEL_LABEL }, 0);
        return dialog.open() == 0;
    }

    /**
     * Convenience method to open a standard error dialog.
     */
    public static void openError(Shell parent, String title, String message, List<String> problemTexts, Image problemImage) {
        MessageDialog dialog = new ProblemsMessageDialog(parent, title, null,
        	message, problemTexts, problemImage, ERROR, new String[] { IDialogConstants.OK_LABEL }, 0);
        dialog.open();
        return;
    }

    /**
     * Convenience method to open a standard information dialog.
     */
    public static void openInformation(Shell parent, String title, String message, List<String> problemTexts, Image problemImage) {
        MessageDialog dialog = new ProblemsMessageDialog(parent, title, null,
        		message, problemTexts, problemImage, INFORMATION,
                new String[] { IDialogConstants.OK_LABEL }, 0);
        dialog.open();
        return;
    }

    /**
     * Convenience method to open a simple Yes/No question dialog.
     */
    public static boolean openQuestion(Shell parent, String title, String message, List<String> problemTexts, Image problemImage) {
        MessageDialog dialog = new ProblemsMessageDialog(parent, title, null,
        		message, problemTexts, problemImage, QUESTION, new String[] { IDialogConstants.YES_LABEL,
                        IDialogConstants.NO_LABEL }, 0);
        return dialog.open() == 0;
    }

    /**
     * Convenience method to open a standard warning dialog.
     */
    public static void openWarning(Shell parent, String title, String message, List<String> problemTexts, Image problemImage) {
        MessageDialog dialog = new ProblemsMessageDialog(parent, title, null,
        		message, problemTexts, problemImage, WARNING, new String[] { IDialogConstants.OK_LABEL }, 0);
        dialog.open();
        return;
    }

    @Override
    protected int getShellStyle() {
        return super.getShellStyle() | SWT.RESIZE;
    }

    @Override
    protected Control createCustomArea(Composite parent) {
        TableViewer tableViewer = new TableViewer(parent);
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.widthHint = convertWidthInCharsToPixels(80);
        gridData.heightHint = 10 + convertHeightInCharsToPixels(Math.min(10, Math.max(3, problemTexts.size())));
        tableViewer.getTable().setLayoutData(gridData);
        tableViewer.setLabelProvider(new LabelProvider() {
            @Override
            public Image getImage(Object element) {
                return problemImage;
            }
        });
        tableViewer.setContentProvider(new ArrayContentProvider());
        tableViewer.setInput(problemTexts);
        return tableViewer.getTable();
    }    
}
