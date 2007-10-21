package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class TraversalRecognizer extends EventRecognizer {
    public TraversalRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        if (e.type == SWT.Traverse) {
            // See TraversalEvent docu.
            switch (e.detail) {
                case SWT.TRAVERSE_NONE: break;
                case SWT.TRAVERSE_ESCAPE: return pressKey(SWT.ESC, 0); //FIXME generates 5 lines or so (???)
                case SWT.TRAVERSE_RETURN: return pressKey(SWT.CR, 0); //FIXME generates 5 lines or so (???)
                case SWT.TRAVERSE_TAB_NEXT: return pressKey(SWT.TAB, 0); 
                case SWT.TRAVERSE_TAB_PREVIOUS: return pressKey(SWT.TAB, SWT.SHIFT); 
                case SWT.TRAVERSE_ARROW_NEXT: return pressKey(SWT.ARROW_RIGHT, 0); 
                case SWT.TRAVERSE_ARROW_PREVIOUS: return pressKey(SWT.ARROW_LEFT, 0); 
                case SWT.TRAVERSE_MNEMONIC: break;  //XXX ???
                case SWT.TRAVERSE_PAGE_NEXT: return pressKey(SWT.PAGE_DOWN, 0); 
                case SWT.TRAVERSE_PAGE_PREVIOUS: return pressKey(SWT.PAGE_UP, 0); 
            }
        }
        return null;
    }

    private JavaSequence pressKey(int keyCode, int modifierState) {
        return makeSeq(expr("Access." + KeyboardEventRecognizer.toPressKeyInvocation(keyCode, modifierState), 0.5, null));
    }

 }