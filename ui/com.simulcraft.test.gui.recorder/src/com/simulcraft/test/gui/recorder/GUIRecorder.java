package com.simulcraft.test.gui.recorder;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.SafeRunner;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;

import com.simulcraft.test.gui.recorder.recognizer.ButtonRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.ComboRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.KeyboardEventRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.MenuRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.ShellRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.TableRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.TextRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.TreeRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.WorkspaceWindowRecognizer;


/**
 * Records GUI events for playback.
 * Must be installed on Display as an event filter.
 *   
 * @author Andras
 */
public class GUIRecorder implements Listener {
    private int modifierState = 0;
    private List<JavaExpr> result = new ArrayList<JavaExpr>();

    private List<IRecognizer> recognizers = new ArrayList<IRecognizer>();
    
    public GUIRecorder() {
        recognizers.add(new KeyboardEventRecognizer(this));
        recognizers.add(new WorkspaceWindowRecognizer(this));
        recognizers.add(new ShellRecognizer(this));
        recognizers.add(new ButtonRecognizer(this));
        recognizers.add(new ComboRecognizer(this));
        recognizers.add(new TextRecognizer(this));
        recognizers.add(new TreeRecognizer(this));
        recognizers.add(new TableRecognizer(this));
        recognizers.add(new MenuRecognizer(this));
    }

    public int getKeyboardModifierState() {
        return modifierState;
    }

    public void handleEvent(final Event e) {
        SafeRunner.run(new ISafeRunnable() {
            public void run() throws Exception {
                safeHandleEvent(e);
            }

            public void handleException(Throwable ex) {
                Activator.logError("An error occurred during recording of event "+e, ex);
            }
        });
    }

    protected void safeHandleEvent(Event e) {
        // housekeeping: we need to keep modifier states ourselves (it doesn't arrive in the event) 
        if (e.type == SWT.KeyDown || e.type == SWT.KeyUp) {
            if (e.keyCode == SWT.SHIFT || e.keyCode == SWT.CONTROL || e.keyCode == SWT.ALT) {
                if (e.type==SWT.KeyDown) modifierState |= e.keyCode;
                if (e.type==SWT.KeyUp) modifierState &= ~e.keyCode;
            }
        }
        
        // collect the best one of the guesses
        List<JavaExpr> list = new ArrayList<JavaExpr>();
        for (IRecognizer recognizer : recognizers) {
            JavaExpr javaExpr = recognizer.recognizeEvent(e);
            if (javaExpr != null) list.add(javaExpr);
        }
        JavaExpr bestJavaExpr = getBestJavaExpr(list);
        
        // and print it
        if (bestJavaExpr != null) {
            add(bestJavaExpr);
        }
        else {
            // unprocessed -- only print message if event is significant
            if (e.type==SWT.KeyDown || e.type==SWT.MouseDown)
                System.out.println("unrecognized mouse click or keydown event: " + e); //XXX record as postEvent() etc?
        }
    }

    public JavaExpr identifyControl(Control control) {
        return identifyControl(control, null);
    }

    public JavaExpr identifyControl(Control control, Point point) {
        List<JavaExpr> list = new ArrayList<JavaExpr>();
        for (IRecognizer recognizer : recognizers) {
            JavaExpr javaExpr = recognizer.identifyControl(control, point);
            if (javaExpr != null) list.add(javaExpr);
        }
        return getBestJavaExpr(list);
    }

    protected JavaExpr getBestJavaExpr(List<JavaExpr> list) {
        return list.isEmpty() ? null : Collections.max(list, new Comparator<JavaExpr>() {
            public int compare(JavaExpr o1, JavaExpr o2) {
                double d = o1.getQuality() - o2.getQuality();
                return d==0 ? 0 : d<0 ? -1 : 1;
            }
        });
    }
    
    public void add(JavaExpr expr) {
        if (expr != null && expr.getQuality() > 0) {
            System.out.println(expr.getJavaCode());
            result.add(expr);
        }
    }

}

