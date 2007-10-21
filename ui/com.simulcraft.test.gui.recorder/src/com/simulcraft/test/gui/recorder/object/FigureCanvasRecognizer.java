package com.simulcraft.test.gui.recorder.object;

import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.ui.parts.GraphicalEditor;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class FigureCanvasRecognizer extends ObjectRecognizer {
    public FigureCanvasRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof FigureCanvas) {
            final FigureCanvas figureCanvas = (FigureCanvas)uiObject;

            GraphicalEditor editorParent = (GraphicalEditor) findWorkbenchPart(true, new IPredicate() {
                public boolean matches(Object object) {
                    if (object instanceof GraphicalEditor) {
                        GraphicalEditor editor = (GraphicalEditor) object;
                        GraphicalViewer viewer = (GraphicalViewer)ReflectionUtils.invokeMethod(editor, "getGraphicalViewer");
                        if (viewer != null && viewer.getControl() == figureCanvas)
                            return true;
                    }
                    return false;
                }
            });

            if (editorParent != null)
                return makeSeq(editorParent, expr("getFigureCanvas()", 0.8, figureCanvas));
        }
        return null;
    }
}