package com.simulcraft.test.gui.recorder.object;

import java.util.List;

import org.eclipse.draw2d.IFigure;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class FigureRecognizer extends ObjectRecognizer {
    public FigureRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    @SuppressWarnings("unchecked")
    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof IFigure) {
            IFigure figure = (IFigure)uiObject;

            // go up the parent chain, and find the highest one within which this figure is still identifiable
            // try to identify figure as the only figure with that class within the ancestor:
            IFigure ancestor = figure;
            while (ancestor.getParent() != null && findDescendantFigure(ancestor.getParent(), figure.getClass()) == figure)
                ancestor = ancestor.getParent();
            if (ancestor != figure)
                return makeMethodCall(ancestor, expr("getDescendantFigure("+figure.getClass().getSimpleName()+")", 0.5, figure));

            // try to identify figure as the kth figure with that class within its parent:
            if (figure.getParent() != null) {
                IFigure parent = figure.getParent();
                int k = 0;
                for (IFigure child : (List<IFigure>)parent.getChildren())
                    if (child == figure)
                        break;
                    else if (child.getClass() == figure.getClass())
                        k++;
                return makeMethodCall(parent, expr("getChildFigure("+figure.getClass().getSimpleName()+", " + k + ")", 0.5, figure));
            }
            
            //FIXME improve! 
            // e.g. may identify a Label figure with its label;
            // or may identify as container as getFigureContainingLabelFigure(clazz, labelText, depth)
            
        }
        return null;
    }
}
