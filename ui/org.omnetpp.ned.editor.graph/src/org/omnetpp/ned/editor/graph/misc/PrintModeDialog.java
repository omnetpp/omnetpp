/*******************************************************************************
 * Copyright (c) 2003, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
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

    protected void cancelPressed() {
        setReturnCode(-1);
        close();
    }

    protected void configureShell(Shell newShell) {
        newShell.setText(MessageFactory.PrintDialog_Title);
        super.configureShell(newShell);
    }

    protected Control createDialogArea(Composite parent) {
        Composite composite = (Composite) super.createDialogArea(parent);

        tile = new Button(composite, SWT.RADIO);
        tile.setText(MessageFactory.PrintDialog_Tile);
        tile.setSelection(true);

        fitPage = new Button(composite, SWT.RADIO);
        fitPage.setText(MessageFactory.PrintDialog_Page);

        fitWidth = new Button(composite, SWT.RADIO);
        fitWidth.setText(MessageFactory.PrintDialog_Width);

        fitHeight = new Button(composite, SWT.RADIO);
        fitHeight.setText(MessageFactory.PrintDialog_Height);

        return composite;
    }

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
