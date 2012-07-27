package org.omnetpp.simulation.inspectors;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.CoordinateListener;
import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.FigureListener;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
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

	// Note: this is a Layer because it needs to be transparent. (It is not a good idea
	// to visually mix Figures and SWT controls, because SWT controls are always on
	// top so "slicing" can occur if such inspector overlaps with another).
	public class ContainerFigure extends Layer implements IInspectorFigure {
		protected IInspectorPart inspectorPart;

		public ContainerFigure() {
		}

		@Override
		public IInspectorPart getInspectorPart() {
			return inspectorPart;
		}

		@Override
		public void setInspectorPart(IInspectorPart part) {
			this.inspectorPart = part;
		}

		@Override
		public int getDragOperation(int x, int y) {
		    return 0;  // we do it on the SWT control
		}

		@Override
		public void setSelectionBorder(boolean isSelected) {
		    // nothing (we do it in SWT)
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
	
        addFloatingControlsSupportTo(control);
        
	    return figure;
	}

	/**
	 * Create the SWT control here. The control's size should also be set within this
	 * method (the inspector figure will be set to the same size).
	 */
	abstract protected Control createControl(Composite parent);

	public Control getControl() {
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
	    getFloatingControls().moveAbove(control);
	}
	
	@Override
	public void dispose() {
	    control.dispose();
	    control = null;
	    super.dispose();
	}
}
