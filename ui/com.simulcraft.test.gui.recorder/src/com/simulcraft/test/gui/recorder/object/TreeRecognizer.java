package com.simulcraft.test.gui.recorder.object;

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
            if (findDescendantControl(tree.getShell(), Tree.class) == uiObject)
                return makeSeq(tree.getShell(), expr("findTree()", 0.8, tree));
        }
        return null;
    }
}