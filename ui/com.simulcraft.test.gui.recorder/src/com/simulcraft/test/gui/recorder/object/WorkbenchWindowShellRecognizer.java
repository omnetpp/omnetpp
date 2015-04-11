/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class WorkbenchWindowShellRecognizer extends ObjectRecognizer {
    public WorkbenchWindowShellRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Shell && uiObject == PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell()) {
            Shell shell = (Shell)uiObject;
            return makeStatement(expr("Access.getWorkbenchWindow().getShell()", 0.7, shell));
        }
        return null;
    }
}