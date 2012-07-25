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
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.simulation.figures.FigureUtils;
import org.omnetpp.simulation.model.cObject;

/**
 * Base class for inspectors that contain SWT widgets.
 * @author Andras
 */
public abstract class AbstractSWTInspectorPart extends InspectorPart {
	protected static Cursor arrowCursor;
	protected Control control;

	public class ContainerFigure extends RoundedRectangle implements IInspectorFigure {
		protected IInspectorPart inspectorPart;
//		protected Insets insets = new Insets(6, 10, 6, 10);
//		protected Insets insets = new Insets(26, 10, 6, 10);

		public ContainerFigure() {
			setBackgroundColor(ColorFactory.LIGHT_BLUE2);
			setLayoutManager(new ToolbarLayout());
			setSelectionBorder(false);
		}

//		@Override
//		public Insets getInsets() {
//			return insets;
//		}

		//@Override
		public IInspectorPart getInspectorPart() {
			return inspectorPart;
		}

		//@Override
		public void setInspectorPart(IInspectorPart part) {
			this.inspectorPart = part;
		}

		//@Override
		public int getDragOperation(int x, int y) {
	    	return FigureUtils.getBorderMoveResizeDragOperation(x, y, getBounds());
		}

		//@Override
		public void setSelectionBorder(boolean isSelected) {
	        //setBorder(isSelected ? new SelectionBorder() : null); //XXX SelectionBorder crashes the VM !! ????
	        setBorder(isSelected ? new LineBorder(5) : null); //XXX for now
		}

	}

	public AbstractSWTInspectorPart(cObject object) {
		super(object);
		figure = new ContainerFigure();
		figure.setInspectorPart(this);
		// we defer the SWT part to when IInspectorContainer gets set.
	}

	@Override
	public void setContainer(IInspectorContainer container) {
		super.setContainer(container);

		Assert.isTrue(container.getControl().getLayout()==null); // must NOT have a layouter (we'll place children manually)
		control = createContents(container.getControl());
		if (arrowCursor == null) arrowCursor = new Cursor(Display.getCurrent(), SWT.CURSOR_ARROW);
		control.setCursor(arrowCursor);  // otherwise border's resize cursor will remain in effect
		adjustControlBounds();

		figure.addFigureListener(new FigureListener() {
			//@Override
			public void figureMoved(IFigure source) { // actually, moved or resized...
				adjustControlBounds();
			}
		});

        // add move/resize/selection support
        new InspectorMouseListener(this); //XXX revise this 
	}

	/**
	 * Create the SWT control here. Also, set the figure's preferred size to
	 * the required size, e.g. like this:
	 *
	 * <pre>
	 * Point p = control.computeSize(SWT.DEFAULT, SWT.DEFAULT);
 	 * figure.setPreferredSize(new Dimension(p.x,p.y));
 	 * </pre>
	 */
	abstract protected Control createContents(Composite parent);

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
