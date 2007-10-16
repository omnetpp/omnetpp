package com.simulcraft.test.gui.recorder;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;


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
        Step bestStep = null;
        for (IRecognizer recognizer : recognizers) {
            Step step = recognizer.recognize(e, modifierState);
            if (step != null) {
                if (bestStep == null || step.getQuality() > bestStep.getQuality())
                    bestStep = step;
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
    
    //=================
    
    public class WorkspaceWindowRecognizer implements IRecognizer {
        public Step identifyWidget(Control control, Point point) {
            if (control instanceof Shell && control == PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell())
                return new Step("getWorkbenchWindow()", 0.8);
            return null;
        }

        public Step recognize(Event e, int modifierState) {
            return null;
        }
    }

    public class ShellRecognizer implements IRecognizer {
        public Step identifyWidget(Control control, Point point) {
            if (control instanceof Shell)
                return new Step("findShellWithTitle(\"" + ((Shell)control).getText() + "\")", 0.5);
            return null;
        }

        public Step recognize(Event e, int modifierState) {
            return null;
        }
    }

    public class ButtonRecognizer implements IRecognizer {
        public Step identifyWidget(Control control, Point point) {
            if (control instanceof Button)
                return new Step("findButtonWithLabel(\"" + ((Button)control).getText() + "\")", 0.5);
            return null;
        }

        public Step recognize(Event e, int modifierState) {
            if (e.type == SWT.MouseDown) {
                Step step = identifyWidget((Control)e.widget, new Point(e.x, e.y));
                if (step != null)
                    return new Step(step.getJavaCode()+".activateWithMouseClick()", step.getQuality());
            }
            return null;
        }
    }

}

