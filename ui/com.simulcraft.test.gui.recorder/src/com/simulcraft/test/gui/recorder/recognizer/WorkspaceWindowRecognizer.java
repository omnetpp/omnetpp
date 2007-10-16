/**
 * 
 */
package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;

import com.simulcraft.test.gui.recorder.IRecognizer;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class WorkspaceWindowRecognizer implements IRecognizer {
    public JavaExpr identifyWidget(Control control, Point point) {
        if (control instanceof Shell && control == PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell())
            return new JavaExpr("getWorkbenchWindow()", 0.8);
        return null;
    }

    public JavaExpr recognize(Event e, int modifierState) {
        return null;
    }
}