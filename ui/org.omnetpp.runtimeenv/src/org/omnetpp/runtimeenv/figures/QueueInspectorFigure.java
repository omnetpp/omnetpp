package org.omnetpp.runtimeenv.figures;

import java.util.List;

import org.eclipse.draw2d.AbstractLayout;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.RoundedRectangle;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.runtime.nativelibs.simkernel.cObject;
import org.omnetpp.runtimeenv.editors.IInspectorFigure;
import org.omnetpp.runtimeenv.editors.IInspectorPart;

/**
 * Figure for QueueInspectorPart
 * 
 * @author Andras
 */
//FIXME messages are not clickable or hoverable
public class QueueInspectorFigure extends Figure implements IInspectorFigure {
	protected IInspectorPart inspectorPart;

	class QueueContentsLayout extends AbstractLayout {
		@Override
		protected Dimension calculatePreferredSize(IFigure container, int hint, int hint2) {
			return new Dimension(10,10);  // irrelevant
		}

		//@Override
		@SuppressWarnings("unchecked")
		public void layout(IFigure container) {
			// compute the area where we'll put the message figures
			Rectangle r = getClientArea().getCopy();
			int lineWidth = r.height/8;
			int serverWidth = 2*r.height/3;
			r.shrink((lineWidth+1)/2, (lineWidth+1)/2);
			r.x += serverWidth+lineWidth;
			r.width -= serverWidth+lineWidth;
			r.y += 3*lineWidth/2;
			r.height -= 3*lineWidth;
			
			Dimension cornerRadii = new Dimension(lineWidth, lineWidth);
			
			int x = r.x;
			for (IFigure child : (List<IFigure>)getChildren()) {
				child.setBounds(new Rectangle(x, r.y, r.height/2, r.height));
				x += r.height/2 + lineWidth/2;
				if (child instanceof RoundedRectangle)
					((RoundedRectangle)child).setCornerDimensions(cornerRadii);
					
			}
		}
	}

	public QueueInspectorFigure() {
		setOpaque(false);
		setLayoutManager(new QueueContentsLayout());
		setPreferredSize(200,50);
		setMinimumSize(new Dimension(32,16));

		setSelectionBorder(false);
	}
	
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

	public IFigure createQueueItemFigure() {
		//Ellipse f = new Ellipse();
		RoundedRectangle f = new RoundedRectangle();
		f.setFill(true);
		f.setBackgroundColor(ColorFactory.RED3);
		f.setForegroundColor(ColorFactory.RED4);
		f.setPreferredSize(20,20);
		return f;
	}
	
	@Override
	protected void paintClientArea(Graphics graphics) {
		super.paintClientArea(graphics);
		
		graphics.pushState();
		Rectangle r = getClientArea().getCopy();
		r.height--; r.width--;
		int lineWidth = r.height/8;
		r.shrink((lineWidth+1)/2, (lineWidth+1)/2);
		r.width += (lineWidth+1)/2;
		
		// draw the server ("O" on the left)
		int serverWidth = 2*r.height/3;
		graphics.setLineWidth(lineWidth);
		graphics.setForegroundColor(ColorFactory.STEEL_BLUE4);
		graphics.drawOval(r.x, r.y, serverWidth, r.height);

		// draw the queue outline ("=")
		graphics.drawLine(r.x+serverWidth+lineWidth, r.y+lineWidth/2, r.right(), r.y+lineWidth/2);
		graphics.drawLine(r.x+serverWidth+lineWidth, r.bottom()-lineWidth/2, r.right(), r.bottom()-lineWidth/2);

		graphics.popState();
	}
}
