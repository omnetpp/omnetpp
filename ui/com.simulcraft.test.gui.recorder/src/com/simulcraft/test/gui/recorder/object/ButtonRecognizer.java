/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ButtonRecognizer extends ObjectRecognizer {

    public ButtonRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Button) {
            Button button = (Button)uiObject;
            Composite container = findContainer(button);
            //FIXME check label uniquely identifies button within container
            return makeMethodCall(container, expr("findButtonWithLabel("+quoteLabel(button.getText()) + ")", 0.8, button));
        }
        return null;
    }
}