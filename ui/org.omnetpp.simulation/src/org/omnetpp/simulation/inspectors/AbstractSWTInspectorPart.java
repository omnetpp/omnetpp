package org.omnetpp.simulation.inspectors;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.FigureListener;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.RoundedRectangle;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.simulation.figures.FigureUtils;
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

	public class ContainerFigure extends RoundedRectangle implements IInspectorFigure {
		protected IInspectorPart inspectorPart;

		public ContainerFigure() {
			setBackgroundColor(ColorFactory.LIGHT_BLUE2);
			setLayoutManager(new ToolbarLayout());
			setSelectionBorder(false);
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
	    	return FigureUtils.getBorderMoveResizeDragOperation(x, y, getBounds());
		}

		@Override
		public void setSelectionBorder(boolean isSelected) {
	        //setBorder(isSelected ? new SelectionBorder() : null); //XXX SelectionBorder crashes the VM !! ????
	        setBorder(isSelected ? new LineBorder(5) : null); //XXX for now
		}

	}

	public AbstractSWTInspectorPart(IInspectorContainer parent, cObject object) {
		super(parent, object);
	}

	@Override
	protected IInspectorFigure createFigure() {
	    ContainerFigure figure = new ContainerFigure();

	    // create and set up the SWT control
	    Composite canvas = getContainer().getControl();
        Assert.isTrue(canvas.getLayout()==null); // must NOT have a layouter (we'll place children manually)

        control = createControl(canvas);

        Point controlSize = control.getSize();
        figure.setPreferredSize(controlSize.x, controlSize.y);
        
        if (arrowCursor == null) 
            arrowCursor = new Cursor(Display.getCurrent(), SWT.CURSOR_ARROW);
        control.setCursor(arrowCursor);  // otherwise border's resize cursor will remain in effect
        
        figure.addFigureListener(new FigureListener() {
            @Override
            public void figureMoved(IFigure source) { // actually, moved or resized...
                adjustControlBounds();
            }
        });
        
        //TODO: also when FigureCanvas is scrolled!!!
	    
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
		r.shrink(10, 10); // leave room for resize handles
		control.setBounds(new org.eclipse.swt.graphics.Rectangle(r.x, r.y, r.width, r.height));
	}
	
	@Override
	public void dispose() {
	    super.dispose();
	    control.dispose();
	    control = null;
	}
}
