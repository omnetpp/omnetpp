package org.omnetpp.ned.editor.text.util;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.operations.IUndoContext;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextInputListener;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.ITextViewerExtension;
import org.eclipse.jface.text.ITextViewerExtension5;
import org.eclipse.jface.text.IUndoManager;
import org.eclipse.jface.text.IUndoManagerExtension;
import org.eclipse.jface.text.Region;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.text.undo.DocumentUndoEvent;
import org.eclipse.text.undo.DocumentUndoManager;
import org.eclipse.text.undo.DocumentUndoManagerRegistry;
import org.eclipse.text.undo.IDocumentUndoListener;
import org.eclipse.text.undo.IDocumentUndoManager;


/**
 * Implementation of {@link org.eclipse.jface.text.IUndoManager} using the shared
 * shared document undo manager.
 * <p>
 * It registers with the connected text viewer as text input listener, and obtains
 * its undo manager from the current document.  It also monitors mouse and keyboard
 * activities in order to partition the stream of text changes into undo-able
 * edit commands.
 * <p>
 * This class is not intended to be subclassed.
 * </p>
 * 
 * @see ITextViewer
 * @see ITextInputListener
 * @see IDocumentUndoManager
 * @see MouseListener
 * @see KeyListener
 * @see DocumentUndoManager
 * 
 * @since 3.2
 */
// NOTE This is the copy of TextViewerUndoManager except that it does not set the 
// autoinsert strategies and do not select the text on undo
public class NedTextViewerUndoManager implements IUndoManager, IUndoManagerExtension {

    /**
     * Internal listener to mouse and key events.
     */
    private class KeyAndMouseListener implements MouseListener, KeyListener {

        /*
         * @see MouseListener#mouseDoubleClick
         */
        public void mouseDoubleClick(MouseEvent e) {
        }

        /*
         * If the right mouse button is pressed, the current editing command is closed
         * @see MouseListener#mouseDown
         */
        public void mouseDown(MouseEvent e) {
            if (e.button == 1)
                if (isConnected())
                    fDocumentUndoManager.commit();
        }

        /*
         * @see MouseListener#mouseUp
         */
        public void mouseUp(MouseEvent e) {
        }

        /*
         * @see KeyListener#keyPressed
         */
        public void keyReleased(KeyEvent e) {
        }

        /*
         * On cursor keys, the current editing command is closed
         * @see KeyListener#keyPressed
         */
        public void keyPressed(KeyEvent e) {
            switch (e.keyCode) {
                case SWT.ARROW_UP:
                case SWT.ARROW_DOWN:
                case SWT.ARROW_LEFT:
                case SWT.ARROW_RIGHT:
                    if (isConnected()) {
                        fDocumentUndoManager.commit();
                    }
                    break;
            }
        }
    }


    /**
     * Internal text input listener.
     */
    private class TextInputListener implements ITextInputListener {

        /*
         * @see org.eclipse.jface.text.ITextInputListener#inputDocumentAboutToBeChanged(org.eclipse.jface.text.IDocument, org.eclipse.jface.text.IDocument)
         */
        public void inputDocumentAboutToBeChanged(IDocument oldInput, IDocument newInput) {
            disconnectDocumentUndoManager();
        }

        /*
         * @see org.eclipse.jface.text.ITextInputListener#inputDocumentChanged(org.eclipse.jface.text.IDocument, org.eclipse.jface.text.IDocument)
         */
        public void inputDocumentChanged(IDocument oldInput, IDocument newInput) {
            connectDocumentUndoManager(newInput);
        }
    }


    /**
     * Internal document undo listener.
     */
    private class DocumentUndoListener implements IDocumentUndoListener {

        /*
         * @see org.eclipse.jface.text.IDocumentUndoListener#documentUndoNotification(DocumentUndoEvent)
         */
        public void documentUndoNotification(DocumentUndoEvent event ){
            if (!isConnected()) return;
            
            int eventType= event.getEventType();
            if (((eventType & DocumentUndoEvent.ABOUT_TO_UNDO) != 0) || ((eventType & DocumentUndoEvent.ABOUT_TO_REDO) != 0))  {
                if (event.isCompound()) {
                    ITextViewerExtension extension= null;
                    if (fTextViewer instanceof ITextViewerExtension)
                        extension= (ITextViewerExtension) fTextViewer;

                    if (extension != null)
                        extension.setRedraw(false);
                }
                fTextViewer.getTextWidget().getDisplay().syncExec(new Runnable() {
                    public void run() {
//                        if (fTextViewer instanceof TextViewer)
//                            ((TextViewer)fTextViewer).ignoreAutoEditStrategies(true);
                    }
                });
                
            } else if (((eventType & DocumentUndoEvent.UNDONE) != 0) || ((eventType & DocumentUndoEvent.REDONE) != 0))  {
                fTextViewer.getTextWidget().getDisplay().syncExec(new Runnable() {
                    public void run() {
//                        if (fTextViewer instanceof TextViewer)
//                            ((TextViewer)fTextViewer).ignoreAutoEditStrategies(false);
                    }
                });
                if (event.isCompound()) {
                    ITextViewerExtension extension= null;
                    if (fTextViewer instanceof ITextViewerExtension)
                        extension= (ITextViewerExtension) fTextViewer;

                    if (extension != null)
                        extension.setRedraw(true);
                }
                
                // Reveal the change if this manager's viewer has the focus.
                if (fTextViewer != null) {
                    StyledText widget= fTextViewer.getTextWidget();
                    if (widget != null && !widget.isDisposed() && (widget.isFocusControl()))// || fTextViewer.getTextWidget() == control))
                        selectAndReveal(event.getOffset(), 
                                event.getText() == null || 
                                (event.getOffset()==0 && fTextViewer.getDocument().getLength()==event.getText().length())
                                   ? 0 : event.getText().length());
                }
            }   
        }

    }

    /** The internal key and mouse event listener */
    private KeyAndMouseListener fKeyAndMouseListener;
    /** The internal text input listener */
    private TextInputListener fTextInputListener;


    /** The text viewer the undo manager is connected to */
    private ITextViewer fTextViewer;
    
    /** The undo level */
    private int fUndoLevel;
    
    /** The document undo manager that is active. */
    private IDocumentUndoManager fDocumentUndoManager;
    
    /** The document that is active. */
    private IDocument fDocument;
    
    /** The document undo listener */
    private IDocumentUndoListener fDocumentUndoListener;

    /**
     * Creates a new undo manager who remembers the specified number of edit commands.
     *
     * @param undoLevel the length of this manager's history
     */
    public NedTextViewerUndoManager(int undoLevel) {
        fUndoLevel= undoLevel;
    }

    /**
     * Returns whether this undo manager is connected to a text viewer.
     *
     * @return <code>true</code> if connected, <code>false</code> otherwise
     */
    private boolean isConnected() {
        return fTextViewer != null && fDocumentUndoManager != null;
    }

    /*
     * @see IUndoManager#beginCompoundChange
     */
    public void beginCompoundChange() {
        if (isConnected()) {
            fDocumentUndoManager.beginCompoundChange();
        }
    }


    /*
     * @see IUndoManager#endCompoundChange
     */
    public void endCompoundChange() {
        if (isConnected()) {
            fDocumentUndoManager.endCompoundChange();
        }
    }

    /**
     * Registers all necessary listeners with the text viewer.
     */
    private void addListeners() {
        StyledText text= fTextViewer.getTextWidget();
        if (text != null) {
            fKeyAndMouseListener= new KeyAndMouseListener();
            text.addMouseListener(fKeyAndMouseListener);
            text.addKeyListener(fKeyAndMouseListener);
            fTextInputListener= new TextInputListener();
            fTextViewer.addTextInputListener(fTextInputListener);
        }
    }

    /**
     * Unregister all previously installed listeners from the text viewer.
     */
    private void removeListeners() {
        StyledText text= fTextViewer.getTextWidget();
        if (text != null) {
            if (fKeyAndMouseListener != null) {
                text.removeMouseListener(fKeyAndMouseListener);
                text.removeKeyListener(fKeyAndMouseListener);
                fKeyAndMouseListener= null;
            }
            if (fTextInputListener != null) {
                fTextViewer.removeTextInputListener(fTextInputListener);
                fTextInputListener= null;
            }
        }
    }

    /**
     * Shows the given exception in an error dialog.
     *
     * @param title the dialog title
     * @param ex the exception
     */
    private void openErrorDialog(final String title, final Exception ex) {
        Shell shell= null;
        if (isConnected()) {
            StyledText st= fTextViewer.getTextWidget();
            if (st != null && !st.isDisposed())
                shell= st.getShell();
        }
        if (Display.getCurrent() != null)
            MessageDialog.openError(shell, title, ex.getLocalizedMessage());
        else {
            Display display;
            final Shell finalShell= shell;
            if (finalShell != null)
                display= finalShell.getDisplay();
            else
                display= Display.getDefault();
            display.syncExec(new Runnable() {
                public void run() {
                    MessageDialog.openError(finalShell, title, ex.getLocalizedMessage());
                }
            });
        }
    }

    /*
     * @see org.eclipse.jface.text.IUndoManager#setMaximalUndoLevel(int)
     */
    public void setMaximalUndoLevel(int undoLevel) {
        fUndoLevel= Math.max(0, undoLevel);
        if (isConnected()) {
            fDocumentUndoManager.setMaximalUndoLevel(fUndoLevel);
        }
    }

    /*
     * @see org.eclipse.jface.text.IUndoManager#connect(org.eclipse.jface.text.ITextViewer)
     */
    public void connect(ITextViewer textViewer) {
        if (fTextViewer == null && textViewer != null) {
            fTextViewer= textViewer;
            addListeners();
        }
        IDocument doc= fTextViewer.getDocument();
        connectDocumentUndoManager(doc);
    }

    /*
     * @see org.eclipse.jface.text.IUndoManager#disconnect()
     */
    public void disconnect() {
        if (fTextViewer != null) {
            removeListeners();
            fTextViewer= null;
        }
        disconnectDocumentUndoManager();
    }

    /*
     * @see org.eclipse.jface.text.IUndoManager#reset()
     */
    public void reset() {
        if (isConnected())
            fDocumentUndoManager.reset();
        
    }

    /*
     * @see org.eclipse.jface.text.IUndoManager#redoable()
     */
    public boolean redoable() {
        if (isConnected())
            return fDocumentUndoManager.redoable();
        return false;
    }

    /*
     * @see org.eclipse.jface.text.IUndoManager#undoable()
     */
    public boolean undoable() {
        if (isConnected())
            return fDocumentUndoManager.undoable();
        return false;
    }

    /*
     * @see org.eclipse.jface.text.IUndoManager#redo()
     */
    public void redo() {
        if (isConnected()) {
            try {
                fDocumentUndoManager.redo();
            } catch (ExecutionException ex) {
                openErrorDialog("Redo Failed", ex); //$NON-NLS-1$
            }
        }
    }

    /*
     * @see org.eclipse.jface.text.IUndoManager#undo()
     */
    public void undo() {
        if (isConnected()) {
            try {
                fDocumentUndoManager.undo();
            } catch (ExecutionException ex) {
                openErrorDialog("Undo Failed", ex); //$NON-NLS-1$
            }
        }
    }

    /**
     * Selects and reveals the specified range.
     *
     * @param offset the offset of the range
     * @param length the length of the range
     */
    private void selectAndReveal(int offset, int length) {
        if (fTextViewer instanceof ITextViewerExtension5) {
            ITextViewerExtension5 extension= (ITextViewerExtension5) fTextViewer;
            extension.exposeModelRange(new Region(offset, length));
        } else if (!fTextViewer.overlapsWithVisibleRegion(offset, length))
            fTextViewer.resetVisibleRegion();

        fTextViewer.setSelectedRange(offset, length);
        fTextViewer.revealRange(offset, length);
    }

    /*
     * @see org.eclipse.jface.text.IUndoManagerExtension#getUndoContext()
     */
    public IUndoContext getUndoContext() {
        if (isConnected()) {
            return fDocumentUndoManager.getUndoContext();
        }
        return null;
    }
    
    private void connectDocumentUndoManager(IDocument document) {
        disconnectDocumentUndoManager();
        if (document != null) {
            fDocument= document;
            DocumentUndoManagerRegistry.connect(fDocument);
            fDocumentUndoManager= DocumentUndoManagerRegistry.getDocumentUndoManager(fDocument);
            fDocumentUndoManager.connect(this);
            setMaximalUndoLevel(fUndoLevel);
            fDocumentUndoListener= new DocumentUndoListener();
            fDocumentUndoManager.addDocumentUndoListener(fDocumentUndoListener);
        }
    }

    private void disconnectDocumentUndoManager() {
        if (fDocumentUndoManager != null) {
            fDocumentUndoManager.disconnect(this);
            DocumentUndoManagerRegistry.disconnect(fDocument);
            fDocumentUndoManager.removeDocumentUndoListener(fDocumentUndoListener);
            fDocumentUndoListener= null;
            fDocumentUndoManager= null;
        }
    }
}

//public class NedTextViewerUndoManager extends TextViewerUndoManager {
//
//    public NedTextViewerUndoManager(int undoLevel) {
//        super(undoLevel);
//    }
//    
//    private void selectAndReveal(int offset, int length) {
//    }
//    
//}
