package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class TabFolderRecognizer extends ObjectRecognizer {
    public TabFolderRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof TabFolder) {
            TabFolder tabFolder = (TabFolder)uiObject;
            Composite container = findContainer(tabFolder);
            if (findDescendantControl(container, TabFolder.class) == uiObject)
                return makeMethodCall(container, expr("findTabFolder()", 0.8, tabFolder)); //FIXME no such method yet
            Label label = getPrecedingUniqueLabel(container, tabFolder);
            if (label != null)
                return makeMethodCall(container, expr("findTabFolderAfterLabel("+quoteRegexText(label.getText()) + ")", 0.8, tabFolder)); //FIXME no such method yet
        }
        return null;
    }
}