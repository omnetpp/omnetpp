/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.recorder.object;

import org.eclipse.ui.IWorkbenchWindow;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class WorkbenchWindowRecognizer extends ObjectRecognizer {
    public WorkbenchWindowRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof IWorkbenchWindow) {
            return makeStatement(expr("Access.getWorkbenchWindow()", 0.5, uiObject));
        }
        return null;
    }
}