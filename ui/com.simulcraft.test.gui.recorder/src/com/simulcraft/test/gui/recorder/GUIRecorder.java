package com.simulcraft.test.gui.recorder;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;

import com.simulcraft.test.gui.recorder.recognizer.ButtonRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.KeyboardEventRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.ShellRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.WorkspaceWindowRecognizer;


/**
 * Records GUI events for playback.
 * Must be installed on Display as an event filter.
 *   
 * @author Andras
 */
public class GUIRecorder implements Listener {
    private int modifierState = 0;
    private List<Object> result = new ArrayList<Object>();

    private List<IRecognizer> recognizers = new ArrayList<IRecognizer>();
    
    public GUIRecorder() {
        recognizers.add(new KeyboardEventRecognizer(this));
        recognizers.add(new WorkspaceWindowRecognizer());
        recognizers.add(new ShellRecognizer());
        recognizers.add(new ButtonRecognizer());
    }

    public void handleEvent(Event e) {
        // housekeeping: we need to keep modifier states ourselves (it doesn't arrive in the event) 
        if (e.type == SWT.KeyDown || e.type == SWT.KeyUp) {
            if (e.keyCode == SWT.SHIFT || e.keyCode == SWT.CONTROL || e.keyCode == SWT.ALT) {
                if (e.type==SWT.KeyDown) modifierState |= e.keyCode;
                if (e.type==SWT.KeyUp) modifierState &= ~e.keyCode;
            }
        }

        // collect the best one of the guesses
        JavaExpr bestStep = null;
        for (IRecognizer recognizer : recognizers) {
            JavaExpr javaExpr = recognizer.recognize(e, modifierState);
            if (javaExpr != null) {
                if (bestStep == null || javaExpr.getQuality() > bestStep.getQuality())
                    bestStep = javaExpr;
            }
        }

        // and print it
        if (bestStep != null) {
            System.out.println(bestStep.getJavaCode());
        }
        else {
            // unprocessed -- only print message if event is significant
            if (e.type==SWT.KeyDown || e.type==SWT.MouseDown)
                System.out.println("unrecognized mouse click or keydown event: " + e); //XXX record as postEvent() etc?
        }
    }
    
    public void add(Object step) {
        System.out.println(step.toString());
        result.add(step);
    }

}

