/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class TableRecognizer extends ObjectRecognizer {
    public TableRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Table) {
            Table table = (Table)uiObject;
            Composite container = findContainer(table);
            if (findDescendantControl(container, Table.class) == uiObject)
                return makeMethodCall(container, expr("findTable()", 0.8, table));
            Label label = getPrecedingUniqueLabel(container, table);
            if (label != null)
                return makeMethodCall(container, expr("findTableAfterLabel("+quoteRegexText(label.getText()) + ")", 0.8, table)); //FIXME no such method yet
        }
        return null;
    }
}