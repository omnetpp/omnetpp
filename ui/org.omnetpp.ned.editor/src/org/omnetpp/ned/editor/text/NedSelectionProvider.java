/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.ITextSelection;
import org.eclipse.jface.viewers.IPostSelectionProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * A selection provider that attaches to a NED text editor's viewer and delegates
 * to it. In addition it provides Structured selection for selection events in the text editor
 * sending NEDElements as the selected object (the element under the cursor). Note that simply
 * moving the cursor also generates selection events (0 length selection).
 *
 * @author rhornig
 */
public class NedSelectionProvider implements IPostSelectionProvider {
    private TextualNedEditor fNedTextEditor;
    private boolean notificationInProgress = false;

    public NedSelectionProvider(TextualNedEditor editor) {
        super();
        fNedTextEditor = editor;
    }

    /*
     * @see org.eclipse.jface.viewers.ISelectionProvider#addSelectionChangedListener(ISelectionChangedListener)
     */
    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        if (fNedTextEditor != null)
            fNedTextEditor.getSelectionProvider().addSelectionChangedListener(listener);
    }

    /*
     * @see org.eclipse.jface.viewers.ISelectionProvider#getSelection()
     */
    public ISelection getSelection() {
        ISelection selection = fNedTextEditor.getSelectionProvider().getSelection();

        // calculate the ned element under the current position
        int offset = ((ITextSelection)selection).getOffset();
        try {
            int line = fNedTextEditor.getDocument().getLineOfOffset(offset);
            int column = offset - fNedTextEditor.getDocument().getLineOffset(line);
            IFile file = ((FileEditorInput) fNedTextEditor.getEditorInput()).getFile();
            INedElement selectedElement = NedResourcesPlugin.getNedResources().getNedElementAt(file, line+1, column);
            if (selectedElement instanceof NedFileElementEx) {
                // To improve the usability of NED views that follows selection (e.g. NED inheritance), we advertise
                // the next type element after the cursor as selected. Otherwise, right after opening a NED file
                // the view would be empty, even if there is only one NED type in that file. Adverse side effects are
                // not known.
                selectedElement = findNedTypeElementAfterLine((NedFileElementEx)selectedElement, line);
            }
            selection = (selectedElement != null) ? new StructuredSelection(selectedElement) : StructuredSelection.EMPTY;
        }
        catch (BadLocationException e) {
        }

        return selection;
    }

    private INedElement findNedTypeElementAfterLine(NedFileElementEx nedFileElement, int line) {
        for (INedElement child : nedFileElement)
            if (child instanceof INedTypeElement)
                if (child.getSourceLineNumber() >= line)
                    return child;
        return null;
    }

    /*
     * @see org.eclipse.jface.viewers.ISelectionProvider#removeSelectionChangedListener(ISelectionChangedListener)
     */
    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        if (fNedTextEditor != null && fNedTextEditor.getSelectionProvider() != null)
            fNedTextEditor.getSelectionProvider().removeSelectionChangedListener(listener);
    }

    /*
     * @see org.eclipse.jface.viewers.ISelectionProvider#setSelection(ISelection)
     */
    public void setSelection(ISelection selection) {
        notificationInProgress = true;
        fNedTextEditor.getSelectionProvider().setSelection(selection);
        notificationInProgress = false;
    }

    /*
     * @see org.eclipse.jface.text.IPostSelectionProvider#addPostSelectionChangedListener(org.eclipse.jface.viewers.ISelectionChangedListener)
     * @since 3.0
     */
    public void addPostSelectionChangedListener(ISelectionChangedListener listener) {
        if (fNedTextEditor != null) {
            if (fNedTextEditor.getSelectionProvider() instanceof IPostSelectionProvider)  {
                IPostSelectionProvider provider= (IPostSelectionProvider) fNedTextEditor.getSelectionProvider();
                provider.addPostSelectionChangedListener(listener);
            }
        }
    }

    /*
     * @see org.eclipse.jface.text.IPostSelectionProvider#removePostSelectionChangedListener(org.eclipse.jface.viewers.ISelectionChangedListener)
     * @since 3.0
     */
    public void removePostSelectionChangedListener(ISelectionChangedListener listener) {
        if (fNedTextEditor != null)  {
            if (fNedTextEditor.getSelectionProvider() instanceof IPostSelectionProvider)  {
                IPostSelectionProvider provider= (IPostSelectionProvider) fNedTextEditor.getSelectionProvider();
                provider.removePostSelectionChangedListener(listener);
            }
        }
    }

    public boolean isNotificationInProgress() {
        return notificationInProgress;
    }
}
