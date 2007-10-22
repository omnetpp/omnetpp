package com.simulcraft.test.gui.recorder.object;

import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class RootFigureRecognizer extends ObjectRecognizer {
    public RootFigureRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof IFigure && ((IFigure)uiObject).getParent() == null) {
            IFigure figure = (IFigure)uiObject;
            FigureCanvas figureCanvas = findCanvasOf(figure);
            return makeMethodCall(figureCanvas, expr("getRootFigure()", 0.8, figure));
        }
        return null;
    }

    protected FigureCanvas findCanvasOf(IFigure figure) {
        IFigure rootFigure = getRootFigure(figure);
        LightweightSystem lightweightSystem = (LightweightSystem)ReflectionUtils.getFieldValue(rootFigure, "this$0");
        return (FigureCanvas)ReflectionUtils.getFieldValue(lightweightSystem, "canvas");
    }

    protected FigureCanvas findCanvasOf2(IFigure figure) {
        // same as above, but works by checking all canvases whether their root figure is the desired one
        final IFigure rootFigure = getRootFigure(figure);
        Shell workbenchWindowShell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
        FigureCanvas figureCanvas = (FigureCanvas) findDescendantControl(workbenchWindowShell, new IPredicate() {
            public boolean matches(Object object) {
                if (object instanceof FigureCanvas && ((FigureCanvas)object).getLightweightSystem().getRootFigure() == rootFigure)
                    return true;
                return false;
            }
        });
        return figureCanvas;
    }

    protected IFigure getRootFigure(IFigure figure) {
        IFigure rootFigure = figure;
        while (rootFigure.getParent() != null)
            rootFigure = rootFigure.getParent();
        return rootFigure;
    }
}
