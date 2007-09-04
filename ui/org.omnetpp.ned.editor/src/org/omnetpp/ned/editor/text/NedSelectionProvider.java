package org.omnetpp.ned.editor.text;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.ITextSelection;
import org.eclipse.jface.viewers.IPostSelectionProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;

/**
 * A selection provider that attaches to a NED text editor's viewer and delegates
 * to it. In addition it provides Structured selection for selection events in the text editor
 * sending NEDElements as the selected object
 *
 * @author rhornig
 */
public class NedSelectionProvider implements IPostSelectionProvider {
        private TextualNedEditor fNedTextEditor;

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
        	int line;
        	try {
        		line = fNedTextEditor.getDocument().getLineOfOffset(offset);
        		int column = offset - fNedTextEditor.getDocument().getLineOffset(line);
        		IFile file = ((FileEditorInput) fNedTextEditor.getEditorInput()).getFile();
        		INEDElement selectedElement = NEDResourcesPlugin.getNEDResources().getNedElementAt(file, line+1, column);
        		// create a structured selection
        		selection = (selectedElement != null) ? new StructuredSelection(selectedElement) : StructuredSelection.EMPTY;
        	} catch (BadLocationException e) {
        	}

        	return selection;
        }

        /*
         * @see org.eclipse.jface.viewers.ISelectionProvider#removeSelectionChangedListener(ISelectionChangedListener)
         */
        public void removeSelectionChangedListener(ISelectionChangedListener listener) {
            if (fNedTextEditor != null)
                fNedTextEditor.getSelectionProvider().removeSelectionChangedListener(listener);
        }

        /*
         * @see org.eclipse.jface.viewers.ISelectionProvider#setSelection(ISelection)
         */
        public void setSelection(ISelection selection) {
            fNedTextEditor.getSelectionProvider().setSelection(selection);
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
}
