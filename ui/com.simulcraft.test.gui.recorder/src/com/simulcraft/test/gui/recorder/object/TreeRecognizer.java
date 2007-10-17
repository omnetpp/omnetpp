package com.simulcraft.test.gui.recorder.object;

import java.util.List;

import org.eclipse.swt.widgets.Tree;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class TreeRecognizer extends ObjectRecognizer {
    public TreeRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public List<JavaExpr> identifyObject(Object uiObject) {
        if (uiObject instanceof Tree) {
            Tree table = (Tree)uiObject;
            if (findDescendantControl(table.getShell(), Tree.class) == uiObject)
                return chain(recorder.identifyObject(table.getShell()), "findTree()", 0.8); // should be lower than TreeItemRecognizer
        }
        return null;
    }
}