/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.Polyline;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.animation.figures.SelectableEllipse;
import org.omnetpp.animation.widgets.AnimationController;
import org.omnetpp.animation.widgets.AnimationPosition;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogGate;
import org.omnetpp.common.eventlog.EventLogMessage;
import org.omnetpp.figures.ConnectionFigure;

/**
 * Draws a filled circle if the message transmission time is 0. Otherwise it draws a
 * growing thick line along the connection as time goes on. The line length represents
 * the transmission time and its traveling speed corresponds to the propagation time.
 */
// TODO: four different message send types
// TODO: - no delay, no datarate -> filled thick line showing the send direction
// TODO: - delay, no datarate -> filled circle moving along the connection
// TODO: - no delay, datarate -> filled thick line showing the send direction, completion mark at the center
// TODO: - delay, datarate -> possibly finite filled thick line moving along the connection, completion marks at the beginning and the end
public class SendMessageAnimation extends AbstractSendMessageAnimation {
	protected EventLogGate gate;

	protected Ellipse messageEllipse;

	protected Polyline messageLine;

	protected Label toolTip;

	protected Label messageLabel;

	public SendMessageAnimation(AnimationController animationController, BigDecimal propagationDelay, BigDecimal transmissionDelay, EventLogGate gate, EventLogMessage message) {
		super(animationController, propagationDelay, transmissionDelay, message);
		this.gate = gate;
		createFigures();
	}

    private void createFigures() {
        toolTip = new Label();
		Color color = ColorFactory.getGoodDarkColor(message == null ? 0 : message.getEncapsulationId());
		messageEllipse = new SelectableEllipse();
		messageEllipse.setSize(20, 20);
		messageEllipse.setForegroundColor(color);
		messageEllipse.setBackgroundColor(color);
		messageEllipse.setToolTip(toolTip);
		messageEllipse.addMouseListener(new MouseListener() {
            public void mouseDoubleClicked(MouseEvent me) {
            }

            public void mousePressed(MouseEvent me) {
            }

            public void mouseReleased(MouseEvent me) {
                animationController.getAnimationCanvas().setSelectedElement(messageEllipse, message);
            }
		});
		messageLine = new Polyline();
		messageLine.setForegroundColor(color);
		messageLine.setLineWidth(5);
		messageLine.setToolTip(toolTip);
		messageLabel = new Label();
		messageLabel.setFont(JFaceResources.getDialogFont());
		messageLabel.setText((message.getName() != null ? message.getName() : "") + " (" + message.getClassName() + ")");
    }

	@Override
	public void activate() {
        super.activate();
		Layer layer = getDecorationLayer();
        if (transmissionDelay.greater(BigDecimal.getZero()))
		    layer.add(messageLine);
		layer.add(messageEllipse);
		layer.add(messageLabel);
		// TODO: this should be much more sophisticated than this, html stuff, etc.
        toolTip.setText("Name: " + message.getName() + ", class: " + message.getClassName() + ", sending time: " + getBeginSimulationTime() + ", arrival time: " + getEndSimulationTime() + ", propagation time: " + propagationDelay + ", transmission time: " + transmissionDelay);
	}

	@Override
	public void deactivate() {
        super.deactivate();
        Layer layer = getDecorationLayer();
        if (transmissionDelay.greater(BigDecimal.getZero()))
		    layer.remove(messageLine);
		layer.remove(messageEllipse);
		layer.remove(messageLabel);
	}

	@Override
	public void refreshAnimation(AnimationPosition animationPosition) {
	    if (gate == null || getConnectionFigure() != null) {
    		Point[] ps = getMessageSendPoints(animationPosition, 4);
    		messageEllipse.setBounds(new Rectangle(ps[0].x - 3, ps[0].y - 3, 7, 7));
    		messageLabel.setBounds(new Rectangle(new Point(ps[0].x, ps[0].y), messageLabel.getPreferredSize().getCopy().expand(10, 0)));
    		messageLine.setEndpoints(ps[0], ps[1]);
	    }
	}

    @Override
    public String toString() {
        return super.toString() + " sending from " + gate.toString();
    }

	@Override
	protected Point getBeginPoint() {
		return getConnectionFigure().getStart();
	}

	@Override
	protected Point getEndPoint() {
		return getConnectionFigure().getEnd();
	}

    protected Layer getDecorationLayer() {
        return getEnclosingCompoundModuleFigure(gate).getForegroundDecorationLayer();
    }

    protected ConnectionFigure getConnectionFigure() {
		return (ConnectionFigure)animationController.getFigure(gate, ConnectionFigure.class);
	}
}
