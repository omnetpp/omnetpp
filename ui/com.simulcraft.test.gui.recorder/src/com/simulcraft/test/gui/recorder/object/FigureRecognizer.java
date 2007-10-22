package com.simulcraft.test.gui.recorder.object;

import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.IFigure;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.util.IPredicate;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class FigureRecognizer extends ObjectRecognizer {
    public FigureRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof IFigure) {
            IFigure figure = (IFigure)uiObject;
           
            FigureCanvas figureCanvas = findParentCanvas(figure);

            return makeSeq(figureCanvas, expr("findFigure(blablabla)", 0.8, figure));
        }
        return null;
    }

    protected FigureCanvas findParentCanvas(IFigure figure) {
        final IFigure rootFigure = getRootFigure(figure);
        dumpFigureHierarchy(rootFigure);

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