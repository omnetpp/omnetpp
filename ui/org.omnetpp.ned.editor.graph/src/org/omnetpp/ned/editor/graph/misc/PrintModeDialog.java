package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.draw2d.PrintFigureOperation;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;

/**
 * @author Eric Bordeau
 */
public class PrintModeDialog extends Dialog {

    private Button tile, fitPage, fitWidth, fitHeight;

    public PrintModeDialog(Shell shell) {
        super(shell);
    }

    @Override
    protected void cancelPressed() {
        setReturnCode(-1);
        close();
    }

    @Override
    protected void configureShell(Shell newShell) {
        newShell.setText("Print ned file");
        super.configureShell(newShell);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        Composite composite = (Composite) super.createDialogArea(parent);

        tile = new Button(composite, SWT.RADIO);
        tile.setText("Print dialog title");
        tile.setSelection(true);

        fitPage = new Button(composite, SWT.RADIO);
        fitPage.setText("Page");

        fitWidth = new Button(composite, SWT.RADIO);
        fitWidth.setText("Width");

        fitHeight = new Button(composite, SWT.RADIO);
        fitHeight.setText("Height");

        return composite;
    }

    @Override
    protected void okPressed() {
        int returnCode = -1;
        if (tile.getSelection())
            returnCode = PrintFigureOperation.TILE;
        else if (fitPage.getSelection())
            returnCode = PrintFigureOperation.FIT_PAGE;
        else if (fitHeight.getSelection())
            returnCode = PrintFigureOperation.FIT_HEIGHT;
        else if (fitWidth.getSelection()) returnCode = PrintFigureOperation.FIT_WIDTH;
        setReturnCode(returnCode);
        close();
    }
}
