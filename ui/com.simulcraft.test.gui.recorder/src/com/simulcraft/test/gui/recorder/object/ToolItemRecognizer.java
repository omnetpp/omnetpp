package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.ToolItem;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ToolItemRecognizer extends ObjectRecognizer {
    
    public ToolItemRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof ToolItem) {
            ToolItem item = (ToolItem)uiObject;
            Composite container = findContainer(item.getParent());
            //FIXME check tooltip text uniquely identifies button within container
            return makeSeq(container, expr("findToolItemWithTooltip("+quoteLabel(item.getToolTipText()) + ")", 0.8, item));
        }
        return null;
    }
}