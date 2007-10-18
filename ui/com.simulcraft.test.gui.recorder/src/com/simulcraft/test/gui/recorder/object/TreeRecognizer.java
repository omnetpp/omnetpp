package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Composite;
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
                return makeSeq(container, expr("findTree()", 0.8, tree));
        }
        return null;
    }
}