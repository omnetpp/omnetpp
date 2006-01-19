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
package org.omnetpp.ned.editor.graph.actions;

import java.io.InputStream;
import java.io.ObjectInputStream;

import org.eclipse.swt.SWT;
import org.eclipse.swt.printing.PrintDialog;
import org.eclipse.swt.printing.Printer;
import org.eclipse.swt.printing.PrinterData;
import org.eclipse.swt.widgets.Shell;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;

import org.eclipse.gef.DefaultEditDomain;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.editparts.ScalableFreeformRootEditPart;
import org.eclipse.gef.print.PrintGraphicalViewerOperation;
import org.eclipse.gef.ui.parts.ScrollingGraphicalViewer;

import org.omnetpp.ned.editor.graph.edit.NedEditPartFactory;
import org.omnetpp.ned.editor.graph.misc.PrintModeDialog;

/**
 * @author Eric Bordeau
 */
public class ModulePrintAction extends Action implements IObjectActionDelegate {

    private Object contents;
    private IFile selectedFile;

    public ModulePrintAction() {
    }

    protected Object getContents() {
        return contents;
    }

    /**
     * @see org.eclipse.ui.IObjectActionDelegate#setActivePart(org.eclipse.jface.action.IAction,
     *      org.eclipse.ui.IWorkbenchPart)
     */
    public void setActivePart(IAction action, IWorkbenchPart targetPart) {
    }

    /**
     * @see org.eclipse.ui.IActionDelegate#run(org.eclipse.jface.action.IAction)
     */
    public void run(IAction action) {
        Shell shell = new Shell();
        GraphicalViewer viewer = new ScrollingGraphicalViewer();
        viewer.createControl(shell);
        viewer.setEditDomain(new DefaultEditDomain(null));
        viewer.setRootEditPart(new ScalableFreeformRootEditPart());
        viewer.setEditPartFactory(new NedEditPartFactory());
        viewer.setContents(getContents());
        viewer.flush();

        int printMode = new PrintModeDialog(shell).open();
        if (printMode == -1) return;
        PrintDialog dialog = new PrintDialog(shell, SWT.NULL);
        PrinterData data = dialog.open();
        if (data != null) {
            PrintGraphicalViewerOperation op = new PrintGraphicalViewerOperation(new Printer(data), viewer);
            op.setPrintMode(printMode);
            op.run(selectedFile.getName());
        }
    }

    /**
     * @see org.eclipse.ui.IActionDelegate#selectionChanged(org.eclipse.jface.action.IAction,
     *      org.eclipse.jface.viewers.ISelection)
     */
    public void selectionChanged(IAction action, ISelection selection) {
        if (!(selection instanceof IStructuredSelection)) return;
        IStructuredSelection sel = (IStructuredSelection) selection;
        if (sel.size() != 1) return;
        selectedFile = (IFile) sel.getFirstElement();
        try {
            InputStream is = selectedFile.getContents(false);
            ObjectInputStream ois = new ObjectInputStream(is);
            setContents(ois.readObject());
            ois.close();
        } catch (Exception e) {
            // This is just an example. All exceptions caught here.
            e.printStackTrace();
        }

    }

    protected void setContents(Object o) {
        contents = o;
    }

}
