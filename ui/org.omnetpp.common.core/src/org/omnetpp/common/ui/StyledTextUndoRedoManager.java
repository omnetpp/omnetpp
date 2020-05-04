package org.omnetpp.common.ui;

import java.util.Stack;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ExtendedModifyEvent;
import org.eclipse.swt.custom.ExtendedModifyListener;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;

/**
 * Adds the Undo-Redo functionality (working Ctrl+Z and Ctrl+Y) to an instance of {@link StyledText}.
 * Source: https://sourceforge.net/p/etinyplugins/blog/2013/02/add-undoredo-support-to-your-swt-styledtext-s/
 *
 * @author Petr Bodnar
 *
 * @see {@linkplain http://www.java2s.com/Code/Java/SWT-JFace-Eclipse/SWTUndoRedo.htm} -
 *      inspiration for this code, though not really functioning - it mainly shows which listeners to use...
 * @see {@linkplain http://stackoverflow.com/questions/7179464/swt-how-to-recreate-a-default-context-menu-for-text-fields} - "SWT's StyledText doesn't support Undo-Redo out-of-the-box"
 */
public class StyledTextUndoRedoManager implements KeyListener, ExtendedModifyListener {
    public static String KEY = "undoManager";

    /**
     * Encapsulation of the Undo and Redo stack(s).
     */
    private static class UndoRedoStack {

        private Stack<ExtendedModifyEvent> undo = new Stack<>();
        private Stack<ExtendedModifyEvent> redo = new Stack<>();

        public UndoRedoStack() {
        }

        public void pushUndo(ExtendedModifyEvent delta) {
            undo.add(delta);
        }

        public void pushRedo(ExtendedModifyEvent delta) {
            redo.add(delta);
        }

        public ExtendedModifyEvent popUndo() {
            return undo.pop();
        }

        public ExtendedModifyEvent popRedo() {
            return redo.pop();
        }

        public void clearRedo() {
            redo.clear();
        }

        public boolean hasUndo() {
            return !undo.isEmpty();
        }

        public boolean hasRedo() {
            return !redo.isEmpty();
        }

    }

    private StyledText editor;
    private UndoRedoStack stack;
    private boolean isUndo;
    private boolean isRedo;

    /**
     * Creates a new instance of this class. Automatically starts listening to
     * corresponding key and modify events coming from the given <var>editor</var>.
     */
    public StyledTextUndoRedoManager(StyledText editor) {
        editor.addExtendedModifyListener(this);
        editor.addKeyListener(this);

        this.editor = editor;
        stack = new UndoRedoStack();

        editor.setData(KEY, this);
    }

    public static StyledTextUndoRedoManager getManagerOf(StyledText editor) {
        return (StyledTextUndoRedoManager) editor.getData(KEY);
    }

    public static StyledTextUndoRedoManager getOrCreateManagerOf(StyledText editor) {
        StyledTextUndoRedoManager manager = getManagerOf(editor);
        if (manager == null)
            manager = new StyledTextUndoRedoManager(editor);
        return manager;
    }

    @Override
    public void keyPressed(KeyEvent e) {
        // Listen to CTRL+Z for Undo, to CTRL+Y or CTRL+SHIFT+Z for Redo
        boolean isCtrl = (e.stateMask & SWT.MOD1) > 0;
        boolean isAlt = (e.stateMask & SWT.ALT) > 0;
        if (isCtrl && !isAlt) {
            boolean isShift = (e.stateMask & SWT.SHIFT) > 0;
            if (!isShift && e.keyCode == 'z') {
                undo();
            } else if (!isShift && e.keyCode == 'y' || isShift
                    && e.keyCode == 'z') {
                redo();
            }
        }
    }

    @Override
    public void keyReleased(KeyEvent e) {
        // ignore
    }

    /**
     * Creates a corresponding Undo or Redo step from the given event and pushes
     * it to the stack. The Redo stack is, logically, emptied if the event comes
     * from a normal user action.
     */
    @Override
    public void modifyText(ExtendedModifyEvent event) {
        //dumpEvent(event);
        if (isUndo) {
            stack.pushRedo(event);
        } else { // is Redo or a normal user action
            stack.pushUndo(event);
            if (!isRedo) {
                stack.clearRedo();
                // TODO Switch to treat consecutive characters as one event?
            }
        }
    }

    /**
     * Performs the Undo action. A new corresponding Redo step is automatically
     * pushed to the stack.
     */
    public void undo() {
        if (stack.hasUndo()) {
            isUndo = true;
            revertEvent(stack.popUndo());
            isUndo = false;
        }
    }

    /**
     * Performs the Redo action. A new corresponding Undo step is automatically
     * pushed to the stack.
     */
    public void redo() {
        if (stack.hasRedo()) {
            isRedo = true;
            revertEvent(stack.popRedo());
            isRedo = false;
        }
    }

    /**
     * Reverts the given modify event, in the way the Eclipse text editor does it.
     */
    private void revertEvent(ExtendedModifyEvent event) {
        editor.replaceTextRange(event.start, event.length, event.replacedText);
        // (causes the modifyText() listener method to be called)
        editor.setSelectionRange(event.start, event.replacedText.length());
    }

    static void dumpEvent(ExtendedModifyEvent event) {
        String insertedText = ((StyledText)event.widget).getTextRange(event.start,  event.length);
        System.out.println("StyledTextUndoRedoManager: received ExtendedModifyEvent: start=" + event.start + " length=" + event.length + " insertedText='" + insertedText + "' replaced='" + event.replacedText + "'");
    }
}