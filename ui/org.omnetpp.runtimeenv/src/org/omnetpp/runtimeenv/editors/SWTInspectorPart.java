package org.omnetpp.runtimeenv.editors;

import org.eclipse.draw2d.FigureListener;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.RoundedRectangle;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.experimental.simkernel.swig.cObject;

/**
 * Base class for inspectors that contain SWT widgets.
 * @author Andras
 */
//XXX TODO
public abstract class SWTInspectorPart extends InspectorPart {
	protected Control control;
	
	public class ContainerFigure extends RoundedRectangle implements IInspectorFigure {
		protected IInspectorPart inspectorPart;
		protected Insets insets = new Insets(4, 8, 4, 8);

		public ContainerFigure() {
			setBackgroundColor(ColorFactory.LIGHT_BLUE2);
			setLayoutManager(new ToolbarLayout());
			setSelectionBorder(false);
		}
		
		@Override
		public Insets getInsets() {
			return insets;
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
			return SWT.TOP|SWT.BOTTOM|SWT.LEFT|SWT.RIGHT; // always move (only)
		}

		@Override
		public void setSelectionBorder(boolean isSelected) {
	        //setBorder(isSelected ? new SelectionBorder() : null); //XXX SelectionBorder crashes the VM !! ????
	        setBorder(isSelected ? new LineBorder(5) : null); //XXX for now
		}

	}

	public SWTInspectorPart(cObject object) {
		super(object);
		figure = new ContainerFigure();
		figure.setInspectorPart(this);
		// we defer the SWT part to when IInspectorContainer gets set.
	}

	@Override
	public void setContainer(IInspectorContainer container) {
		super.setContainer(container);
		control = createContents(container.getControl());
		adjustControlBounds();
		
		figure.addFigureListener(new FigureListener() {
			@Override
			public void figureMoved(IFigure source) {
				adjustControlBounds();
			}
		});
	}

	abstract protected Control createContents(Composite parent);
	
	public Control getControl() {
		return control;
	}
	
	protected void adjustControlBounds() {
		Rectangle r = figure.getBounds().getCopy();
		figure.translateToAbsolute(r);
		control.setBounds(new org.eclipse.swt.graphics.Rectangle(r.x, r.y, r.width, r.height));
	}
}
