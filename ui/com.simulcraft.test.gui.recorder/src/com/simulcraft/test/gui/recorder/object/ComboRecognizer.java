/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ComboRecognizer extends ObjectRecognizer {
    public ComboRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Combo) {
            Combo combo = (Combo)uiObject;
            Composite container = findContainer(combo);
            Label label = getPrecedingUniqueLabel(container, combo);
            if (label != null)
                return makeMethodCall(container, expr("findComboAfterLabel("+quoteLabel(label.getText()) + ")", 0.8, combo));
        }
        return null;
    }
}