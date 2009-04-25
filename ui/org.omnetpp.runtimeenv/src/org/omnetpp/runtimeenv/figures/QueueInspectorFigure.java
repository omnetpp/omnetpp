package org.omnetpp.runtimeenv.figures;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.ToolbarLayout;
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
//FIXME layouting is completely broken; messages are not clickable or hoverable
public class QueueInspectorFigure extends Figure implements IInspectorFigure {
	protected IInspectorPart inspectorPart;

	public QueueInspectorFigure() {
		setOpaque(false);
		ToolbarLayout layout = new ToolbarLayout();
		layout.setVertical(false);
		layout.setSpacing(2); //XXX but this should scale too
		setLayoutManager(layout);
		setPreferredSize(200,50);
		setMinimumSize(new Dimension(16,32));

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

	public void setQueueContents(cObject[] msgs) {
		removeAll();

		if (msgs.length != 0) {
//			// compute the area where we'll put the message figures
//			Rectangle r = getClientArea().getCopy();
//			r.height--; r.width--;
//			int lineWidth = r.height/8;
//			int serverWidth = 2*r.height/3;
//			r.shrink((lineWidth+1)/2, (lineWidth+1)/2);
//			r.x = serverWidth+lineWidth;
//			r.width -= serverWidth+lineWidth;
//			r.y = 2*lineWidth;
//			r.height -= 4*lineWidth;

			// insert the message figures
			for (cObject msg : msgs) {
				Ellipse f = new Ellipse();
				f.setFill(true);
				f.setBackgroundColor(ColorFactory.RED3);
				f.setPreferredSize(20,20);
				add(f);
			}
		}
	}
	
	@Override
	protected void paintClientArea(Graphics graphics) {
		super.paintClientArea(graphics);
		
		graphics.pushState();
		Rectangle r = getClientArea().getCopy();
		r.height--; r.width--;
		int lineWidth = r.height/8;
		r.shrink((lineWidth+1)/2, (lineWidth+1)/2);
		
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
