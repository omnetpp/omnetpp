/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Tree;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class TreeRecognizer extends ObjectRecognizer {
    public TreeRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Tree) {
            Tree tree = (Tree)uiObject;
            Composite container = findContainer(tree);
            if (findDescendantControl(container, Tree.class) == uiObject)
                return makeMethodCall(container, expr("findTree()", 0.8, tree));
            Label label = getPrecedingUniqueLabel(container, tree);
            if (label != null)
                return makeMethodCall(container, expr("findTreeAfterLabel("+quoteRegexText(label.getText()) + ")", 0.8, tree)); //FIXME no such method yet
        }
        return null;
    }
}