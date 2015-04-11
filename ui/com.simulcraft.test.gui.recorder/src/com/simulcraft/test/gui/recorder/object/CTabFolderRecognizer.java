/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class CTabFolderRecognizer extends ObjectRecognizer {
    public CTabFolderRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof CTabFolder) {
            CTabFolder tabFolder = (CTabFolder)uiObject;
            Composite container = findContainer(tabFolder);
            if (findDescendantControl(container, CTabFolder.class) == uiObject)
                return makeMethodCall(container, expr("findCTabFolder()", 0.8, tabFolder)); //FIXME no such method yet
            Label label = getPrecedingUniqueLabel(container, tabFolder);
            if (label != null)
                return makeMethodCall(container, expr("findCTabFolderAfterLabel("+quoteRegexText(label.getText()) + ")", 0.8, tabFolder)); //FIXME no such method yet
        }
        return null;
    }
}