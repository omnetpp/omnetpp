package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class BlankLineInserter extends EventRecognizer {
//    private int previousNumShells = 1;
    
    public BlankLineInserter(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        // insert a blank line into the code whenever the number of shells changes
        // (that is, a dialog opens/closes or something like that)
        //XXX not a good idea: menu selections insert multiple new lines!
//        int numShells = Display.getCurrent().getShells().length;
//        if (numShells != previousNumShells) {
//            recorder.add(new JavaExpr("", 1.0, null));
//            previousNumShells = numShells;
//        }
        return null;
    }
}

