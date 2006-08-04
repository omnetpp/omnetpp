package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.Polyline;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.common.simulation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.ConnectionFigure;

/**
 * Draws an ellipse if the message transmission time is 0. Otherwise it draws a
 * growing line following the connection as time goes on.
 */
public class SendMessageAnimation extends AbstractAnimationPrimitive {
	//private static final Color COLOR = new Color(null, 128, 0, 0);

	private ConnectionId connectionId;

	private IRuntimeMessage msg; //FIXME should not be here...
	
	private double transmissionTime;

	private double endSimulationTime;
	
	private Ellipse messageEllipse;
	
	private Polyline messageLine;
	
	private Label toolTip;
	
	private Label messageLabel;

	public SendMessageAnimation(ReplayAnimationController animationController,
								long eventNumber,
								double simulationTime,
								long animationNumber,
								double propagationTime,
								double transmissionTime,
								ConnectionId connectionId,
								IRuntimeMessage msg) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.transmissionTime = transmissionTime;
		this.endSimulationTime = simulationTime + propagationTime + transmissionTime;
		this.connectionId = connectionId;
		this.msg = msg;
		
		toolTip = new Label();
		toolTip.setText("Sending time: " + simulationTime +
				" arrival time: " + endSimulationTime +
				" propagation time: " + propagationTime +
				" transmission time: " + transmissionTime);

		messageEllipse = new Ellipse();
		messageEllipse.setSize(20, 20);
		Color color = ColorFactory.getGoodColor(msg==null ? 0 : msg.getEncapsulationId()); 
		messageEllipse.setForegroundColor(color);
		messageEllipse.setBackgroundColor(color);
		messageEllipse.setToolTip(toolTip);

		messageLine = new Polyline();
		messageLine.setForegroundColor(color);
		messageLine.setLineWidth(5);
		messageLine.setToolTip(toolTip);
		
		messageLabel = new Label();
	}
	
	@Override
	public double getEndSimulationTime() {
		return endSimulationTime;
	}
	
	@Override
	public double getEndAnimationTime() {
		if (transmissionTime == 0)
			return super.getEndAnimationTime();
		else
			return animationEnvironment.getAnimationTimeForSimulationTime(endSimulationTime);
	}
	
	public void redo() {
		ConnectionFigure connectionFigure = (ConnectionFigure)animationEnvironment.getFigure(connectionId);
		if (connectionFigure != null) {
			CompoundModuleFigure enclosingModuleFigure = getEnclosingModuleFigure();
			
			if (transmissionTime != 0)
				enclosingModuleFigure.addMessageFigure(messageLine);
			enclosingModuleFigure.addMessageFigure(messageEllipse);

			if (msg != null)
				messageLabel.setText((msg.getName() != null ? msg.getName() : "") + "(" + msg.getClassName() + ")");
			enclosingModuleFigure.addMessageFigure(messageLabel);
		}
	}

	public void undo() {
		ConnectionFigure connectionFigure = (ConnectionFigure)animationEnvironment.getFigure(connectionId);
		if (connectionFigure != null) {
			CompoundModuleFigure enclosingModuleFigure = getEnclosingModuleFigure();
		
			if (transmissionTime != 0)
				enclosingModuleFigure.removeMessageFigure(messageLine);
			enclosingModuleFigure.removeMessageFigure(messageEllipse);
			
			enclosingModuleFigure.removeMessageFigure(messageLabel);
		}
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		ConnectionFigure connectionFigure = (ConnectionFigure)animationEnvironment.getFigure(connectionId);

		if (connectionFigure != null) {
			Point p1 = connectionFigure.getStart();
			Point p2 = connectionFigure.getEnd();
			double simulationTimeDelta = endSimulationTime - beginSimulationTime - transmissionTime;

			// translate connection line coordinates orthogonal to the line
			PrecisionPoint n = new PrecisionPoint(p1.y - p2.y, p2.x - p1.x);
			n.performScale(4 / Math.sqrt(n.x * n.x + n.y * n.y));
			p1.translate(n);
			p2.translate(n);

			double alpha;
			if (simulationTimeDelta != 0)
				alpha = (simulationTime - beginSimulationTime) / simulationTimeDelta;
			else
				alpha = (animationTime - getBeginAnimationTime()) / (getEndAnimationTime() - getBeginAnimationTime());
			alpha = Math.max(0, Math.min(alpha, 1));
			
			if (transmissionTime == 0) {
				Point p = getConvexCombination(p1, p2, alpha);
				//setConstraint(messageEllipse, new Rectangle(p.x - 3, p.y - 3, 7, 7));
				messageEllipse.setBounds(new Rectangle(p.x - 3, p.y - 3, 7, 7));//XXX clarify constraint vs setbounds
				messageLabel.setBounds(new Rectangle(p.x, p.y, 200, 20)); // FIXME: should set location instead but does not show up
			}
			else {
				double beta;
				if (simulationTimeDelta != 0) {
					beta = (simulationTime - beginSimulationTime - transmissionTime) / simulationTimeDelta;
					beta = Math.max(0, Math.min(beta, 1));
				}
				else {
					alpha = 0;
					beta = 1;
				}

				Point pAlpha = getConvexCombination(p1, p2, alpha);
				Point pBeta = getConvexCombination(p1, p2, beta);
				messageLine.setEndpoints(pAlpha, pBeta); //XXX clarify constraint vs setEndsPoints
				//setConstraint(messageEllipse, new Rectangle(pAlpha.x - 3, pAlpha.y - 3, 7, 7));
				messageEllipse.setBounds(new Rectangle(pAlpha.x - 3, pAlpha.y - 3, 7, 7));  //XXX clarify constraint vs setbounds
				messageLabel.setBounds(new Rectangle(pAlpha.x, pAlpha.y, 200, 20)); // FIXME: should set location instead but does not show up
			}
			System.out.println("bounds:"+messageEllipse.getBounds() + " constraint:"+getLayoutManager().getConstraint(messageEllipse));
		}
	}

	private Point getConvexCombination(Point p1, Point p2, double alpha) {
		return new Point(Math.round((1 - alpha) * p1.x + alpha * p2.x), Math.round((1 - alpha) * p1.y + alpha * p2.y));
	}
}
