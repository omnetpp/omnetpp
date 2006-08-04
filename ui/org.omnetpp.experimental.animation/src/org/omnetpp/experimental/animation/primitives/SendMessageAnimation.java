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

		Color color = ColorFactory.getGoodColor(msg==null ? 0 : msg.getEncapsulationId()); 
		messageEllipse = new Ellipse();
		messageEllipse.setSize(20, 20);
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
	public double getEndAnimationTime() {
		if (transmissionTime == 0)
			return super.getEndAnimationTime();
		else
			return animationEnvironment.getAnimationTimeForSimulationTime(endSimulationTime);
	}
	
	public void redo() {
		ConnectionFigure connectionFigure = (ConnectionFigure)animationEnvironment.getFigure(connectionId);
		if (connectionFigure != null)
			redoInternal();
	}

	protected void redoInternal() {
		CompoundModuleFigure enclosingModuleFigure = getEnclosingModuleFigure();
		
		if (transmissionTime != 0)
			enclosingModuleFigure.addMessageFigure(messageLine);
		enclosingModuleFigure.addMessageFigure(messageEllipse);

		if (msg != null)
			messageLabel.setText((msg.getName() != null ? msg.getName() : "") + "(" + msg.getClassName() + ")");
		enclosingModuleFigure.addMessageFigure(messageLabel);
	}

	public void undo() {
		ConnectionFigure connectionFigure = (ConnectionFigure)animationEnvironment.getFigure(connectionId);
		if (connectionFigure != null)
			undoInternal();
	}

	protected void undoInternal() {
		CompoundModuleFigure enclosingModuleFigure = getEnclosingModuleFigure();

		if (transmissionTime != 0)
			enclosingModuleFigure.removeMessageFigure(messageLine);
		enclosingModuleFigure.removeMessageFigure(messageEllipse);
		
		enclosingModuleFigure.removeMessageFigure(messageLabel);
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		ConnectionFigure connectionFigure = (ConnectionFigure)animationEnvironment.getFigure(connectionId);

		if (connectionFigure != null)
			animateSendMessageAt(simulationTime, animationTime, connectionFigure.getStart(), connectionFigure.getEnd());
	}

	protected void animateSendMessageAt(double simulationTime, double animationTime, Point p1, Point p2) {
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
		
		Point p = null;
		if (transmissionTime == 0)
			p = getConvexCombination(p1, p2, alpha);
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
			messageLine.setEndpoints(pAlpha, pBeta);
			p = pAlpha;
		}

		messageEllipse.setBounds(new Rectangle(p.x - 3, p.y - 3, 7, 7));
		messageLabel.setBounds(new Rectangle(new Point(p.x, p.y), messageLabel.getPreferredSize().getCopy().expand(10, 0)));
		//System.out.println("bounds:"+messageEllipse.getBounds() + " constraint:"+getLayoutManager().getConstraint(messageEllipse));
	}
}
