package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Widget;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class IDBasedControlRecognizer extends ObjectRecognizer {
    // TODO duplicated in com.simulcraft.test.gui.util.Predicate, move to common?
    public static final String WIDGET_ID = "com.simulcraft.test.gui.WidgetID";
    
    public IDBasedControlRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Widget && ((Widget)uiObject).getData(WIDGET_ID) != null) {
            Widget widget = (Widget)uiObject;
            Control control = GUIRecorder.getParentControl(widget);
            Composite container = findContainer(control); 
            String widgetID = widget.getData(WIDGET_ID).toString();
            //FIXME check WIDGET_ID uniquely identifies widget within the container
            return makeSeq(container, expr("findWidgetWithID("+toJavaLiteral(widgetID) + ")", 0.8, widget));
        }
        return null;
    }

}