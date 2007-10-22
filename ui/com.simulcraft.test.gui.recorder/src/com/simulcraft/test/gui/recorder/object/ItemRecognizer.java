package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.swt.widgets.TreeItem;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ItemRecognizer extends ObjectRecognizer {
    public ItemRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof CTabItem) {
            CTabItem item = (CTabItem)uiObject;
            //FIXME check label uniquely identifies item
            return makeMethodCall(item.getParent(), expr("findItemByText("+quoteLabel(item.getText())+")", 0.8, item));
        }
        if (uiObject instanceof TabItem) {
            TabItem item = (TabItem)uiObject;
            //FIXME check label uniquely identifies item
            return makeMethodCall(item.getParent(), expr("findItemByText("+quoteLabel(item.getText())+")", 0.8, item));
        }
        if (uiObject instanceof TreeItem) {
            TreeItem item = (TreeItem)uiObject;
            //FIXME check label uniquely identifies item
            return makeMethodCall(item.getParent(), expr("findTreeItemByContent("+quoteRegexText(item.getText())+")", 0.8, item));
        }
        if (uiObject instanceof TableItem) {
            TableItem item = (TableItem)uiObject;
            //FIXME check label uniquely identifies item
            return makeMethodCall(item.getParent(), expr("findTableItemByContent("+quoteLabel(item.getText())+")", 0.8, item));
        }
        if (uiObject instanceof ToolItem) {
            ToolItem item = (ToolItem)uiObject;
            Composite container = findContainer(item.getParent());
            //FIXME check tooltip text uniquely identifies button within container
            return makeMethodCall(container, expr("findToolItemWithTooltip("+quoteLabel(item.getToolTipText()) + ")", 0.8, item));
        }
        return null;
    }
}