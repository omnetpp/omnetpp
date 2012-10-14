package org.omnetpp.simulation.inspectors;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.CoordinateListener;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.FigureListener;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.figures.IInspectorFigure;
import org.omnetpp.simulation.model.cObject;

/**
 * Base class for inspectors that contain SWT widgets. Subclasses should override
 * createControl() instead of createFigure().
 *
 * @author Andras
 */
public abstract class AbstractSWTInspectorPart extends AbstractInspectorPart {
    protected static Cursor arrowCursor;
    protected Control control;

    // Note: this is a transparent Figure. (It is not a good idea to visually mix Figures
    // and SWT controls, because SWT controls are always on top so "slicing" can occur
    // if such inspector overlaps with another). Note: this cannot be a layer because
    // SimulationCanvas.findInspectorPartAt() wouldn't find it!
    public class ContainerFigure extends Figure implements IInspectorFigure {
        public ContainerFigure() {
        }
    }

    public AbstractSWTInspectorPart(IInspectorContainer parent, cObject object) {
        super(parent, object);
    }

    @Override
    protected IInspectorFigure createFigure() {
        ContainerFigure figure = new ContainerFigure();

        // create and set up the SWT control
        FigureCanvas canvas = getContainer().getControl();
        Assert.isTrue(canvas.getLayout()==null); // must NOT have a layouter (we'll place children manually)

        control = createControl(canvas);

        // set figure to same size as SWT control
        Point controlSize = control.getSize();
        figure.setPreferredSize(controlSize.x, controlSize.y);

        // set a cursor to the control, otherwise border's resize cursor will remain in effect
        if (control.getCursor() == null) {
            if (arrowCursor == null)
                arrowCursor = new Cursor(Display.getCurrent(), SWT.CURSOR_ARROW);
            control.setCursor(arrowCursor);
        }

        // need to adjust control's bounds when figure is moved or resized
        figure.addFigureListener(new FigureListener() {
            @Override
            public void figureMoved(IFigure source) {
                if (isDisposed()) return; //FIXME rather: remove listeners in dispose!!!!
                adjustControlBounds();
            }
        });

        // also also when FigureCanvas is scrolled
        getContainer().getControl().getViewport().addCoordinateListener(new CoordinateListener() {
            @Override
            public void coordinateSystemChanged(IFigure source) {
                if (isDisposed()) return; //FIXME rather: remove listeners in dispose!!!!
                adjustControlBounds();
            }
        });

        return figure;
    }

    /**
     * Create the SWT control here. The control's size should also be set within this
     * method (the inspector figure will be set to the same size).
     */
    abstract protected Control createControl(Composite parent);

    public Control getSWTControl() {
        return control;
    }

    protected void adjustControlBounds() {
        Rectangle r = figure.getBounds().getCopy();
        figure.translateToAbsolute(r);
        control.setBounds(new org.eclipse.swt.graphics.Rectangle(r.x, r.y, r.width, r.height));
    }

    @Override
    public void raiseToTop() {
        super.raiseToTop();
        control.moveAbove(null);
    }

    @Override
    protected void setSelectionMark(boolean isSelected) {
        control.setBackground(isSelected ? ColorFactory.GREY50 : null);
    }

    @Override
    public void dispose() {
        control.dispose();
        control = null;
        super.dispose();
    }

}
