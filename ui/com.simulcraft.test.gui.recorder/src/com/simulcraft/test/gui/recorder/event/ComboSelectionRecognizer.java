/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ComboSelectionRecognizer extends EventRecognizer {
    public ComboSelectionRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        if (e.type == SWT.Selection && e.widget instanceof Combo) {
            Combo combo = (Combo)e.widget;
            return makeMethodCall(combo, expr("selectItem("+quoteRegexText(combo.getText())+")", 0.8, null));
        }
        return null;
    }
}

