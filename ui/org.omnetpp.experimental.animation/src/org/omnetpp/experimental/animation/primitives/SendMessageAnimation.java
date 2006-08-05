package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.Polyline;
import org.eclipse.draw2d.geometry.Point;
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
public class SendMessageAnimation extends AbstractSendMessageAnimation {
	private ConnectionId connectionId;

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
		super(animationController, eventNumber, simulationTime, animationNumber, propagationTime, transmissionTime, msg);
		this.connectionId = connectionId;
		
		toolTip = new Label();
		toolTip.setText("Sending time: " + simulationTime +
				" arrival time: " + endSimulationTime +
				" propagation time: " + propagationTime +
				" transmission time: " + transmissionTime);

		Color color = ColorFactory.getGoodColor(msg == null ? 0 : msg.getEncapsulationId()); 
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
	
	public void redo() {
		if (isDisplayed()) {
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
		if (isDisplayed()) {
			CompoundModuleFigure enclosingModuleFigure = getEnclosingModuleFigure();

			if (transmissionTime != 0)
				enclosingModuleFigure.removeMessageFigure(messageLine);

			enclosingModuleFigure.removeMessageFigure(messageEllipse);
			enclosingModuleFigure.removeMessageFigure(messageLabel);
		}
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		if (isDisplayed()) {
			Point[] ps = getMessageSendPoints(simulationTime, animationTime, 4);
			messageEllipse.setBounds(new Rectangle(ps[0].x - 3, ps[0].y - 3, 7, 7));
			messageLabel.setBounds(new Rectangle(new Point(ps[0].x, ps[0].y), messageLabel.getPreferredSize().getCopy().expand(10, 0)));
			
			if (!ps[0].equals(ps[1]))
				messageLine.setEndpoints(ps[0], ps[1]);
		}
	}

	private ConnectionFigure getConnectionFigure() {
		return (ConnectionFigure)animationEnvironment.getFigure(connectionId);
	}

	protected Point getBeginPoint() {
		return getConnectionFigure().getStart();
	}
	
	protected Point getEndPoint() {
		return getConnectionFigure().getEnd();
	}

	protected boolean isDisplayed() {
		return getConnectionFigure() != null;
	}
}
