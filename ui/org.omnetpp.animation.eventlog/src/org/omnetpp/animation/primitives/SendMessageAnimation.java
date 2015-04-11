/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

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
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Pattern;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.figures.Completion;
import org.omnetpp.animation.figures.SelectableEllipse;
import org.omnetpp.animation.figures.SelectablePolyline;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.GateModel;
import org.omnetpp.common.simulation.MessageModel;
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
// TODO: show the message that will be processed at least for a single moment when handle message animation starts
// TODO: when there are multiple connection segments: e.g. P0T0, P1T1, P0T0
// TODO:  - highlight first segment when there's a bit at the beginning of the second segment
// TODO:  - highlight last segment when there's a bit at the end of the second segment
public class SendMessageAnimation extends AbstractSendMessageAnimation {
	protected GateModel gate;

	protected Ellipse messageEllipse;

	protected Polyline messageLine;

	protected Label messageToolTip;

	protected Label messageLabel;

	protected Completion messageCompletion;

    protected Completion messageBeginCompletion;

    protected Completion messageEndCompletion;

    protected boolean isPropagationDelayZero;

    protected boolean isTransmissionDelayZero;

	public SendMessageAnimation(AnimationController animationController, BigDecimal propagationDelay, BigDecimal transmissionDelay, GateModel gate, MessageModel message, double messageHighlightAnimationDuration) {
		super(animationController, propagationDelay, transmissionDelay, message, messageHighlightAnimationDuration);
		this.gate = gate;
        this.isPropagationDelayZero = propagationDelay.equals(BigDecimal.getZero());
        this.isTransmissionDelayZero = transmissionDelay.equals(BigDecimal.getZero());
		createFigures();
	}

    private void createFigures() {
        messageToolTip = new Label();
		Color color = ColorFactory.getGoodDarkColor(message == null ? 0 : message.getEncapsulationId());
        messageLabel = new Label();
        messageLabel.setFont(JFaceResources.getDialogFont());
        messageLabel.setText((message.getName() != null ? message.getName() : "") + " (" + message.getClassName() + ")");
        if (!isPropagationDelayZero && isTransmissionDelayZero) {
    		messageEllipse = new SelectableEllipse();
    		messageEllipse.setSize(20, 20);
    		messageEllipse.setForegroundColor(color);
    		messageEllipse.setBackgroundColor(color);
    		messageEllipse.setToolTip(messageToolTip);
    		messageEllipse.addMouseListener(new MouseListener() {
                public void mouseDoubleClicked(MouseEvent me) {
                }

                public void mousePressed(MouseEvent me) {
                }

                public void mouseReleased(MouseEvent me) {
                    animationController.getAnimationCanvas().setSelectedElement(messageEllipse, message);
                }
    		});
        }
        else {
            if (isPropagationDelayZero && isTransmissionDelayZero) {
                ConnectionFigure connectionFigure = new ConnectionFigure();
                connectionFigure.setStyle(color, 1, null);
                messageLine = connectionFigure;
            }
            else {
        		messageLine = new SelectablePolyline() {
        		    @Override
                    public void paintFigure(org.eclipse.draw2d.Graphics graphics) {
        		        if (isTransmissionDelayZero)
                            super.paintFigure(graphics);
        		        else {
                            Point begin = getBeginPoint();
                            Point end = getEndPoint();
                            Dimension difference = end.getDifference(begin);
                            double length = Math.sqrt(difference.width * difference.width + difference.height * difference.height);
                            double datarate = message.getBitLength() / transmissionDelay.doubleValue();
                            Dimension patternDimension = difference.scale((15 - Math.min(Math.log10(datarate), 15)) * 4 / end.getDistance(begin));
                            double patternLength = Math.sqrt(patternDimension.width * patternDimension.width + patternDimension.height * patternDimension.height);
                            double alpha;
                            if (isPropagationDelayZero)
                                alpha = 0;
                            else {
                                alpha = animationController.getCurrentSimulationTime().subtract(getBeginSimulationTime()).doubleValue() / propagationDelay.doubleValue() * length / patternLength;
                                alpha = alpha - Math.floor(alpha);
                            }
                            int x1 = (int)(alpha * patternDimension.width);
                            int y1 = (int)(alpha * patternDimension.height);
                            int x2 = x1 + patternDimension.width;
                            int y2 = y1 + patternDimension.height;
                            Color foregroundColor = getForegroundColor();
                            Display display = Display.getCurrent();
                            Color darkerColor = new Color(display, foregroundColor.getRed() / 2, foregroundColor.getGreen() / 2, foregroundColor.getBlue() / 2);
                            Color lighterColor = new Color(display, 255 - (255 - foregroundColor.getRed()) / 2, 255 - (255 - foregroundColor.getGreen()) / 2, 255 - (255 - foregroundColor.getBlue()) / 2);
                            Pattern pattern = new Pattern(display, x1, y1, x2, y2, lighterColor, darkerColor);
                            graphics.setForegroundPattern(pattern);
                            super.paintFigure(graphics);
                            graphics.setForegroundPattern(null);
                            pattern.dispose();
        		        }
        		    };
        		};
                messageLine.setLineWidth(5);
            }
    		messageLine.setForegroundColor(color);
    		messageLine.setToolTip(messageToolTip);
            messageLine.addMouseListener(new MouseListener() {
                public void mouseDoubleClicked(MouseEvent me) {
                }

                public void mousePressed(MouseEvent me) {
                }

                public void mouseReleased(MouseEvent me) {
                    animationController.getAnimationCanvas().setSelectedElement(messageLine, message);
                }
            });
            messageCompletion = new Completion(true, true, false, 0, color);
            messageBeginCompletion = new Completion(true, true, false, 0, color);
            messageEndCompletion = new Completion(true, true, false, 0, color);
        }
    }

	@Override
	public void activate() {
        super.activate();
		Layer layer = getDecorationLayer();
        layer.add(messageLabel);
        if (!isPropagationDelayZero && isTransmissionDelayZero)
            layer.add(messageEllipse);
        else {
		    layer.add(messageLine);
            layer.add(messageCompletion);
            layer.add(messageBeginCompletion);
            layer.add(messageEndCompletion);
        }
		// TODO: this should be much more sophisticated than this, html stuff, etc.
        messageToolTip.setText("Name: " + message.getName() + ", class: " + message.getClassName() + ", sending time: " + getBeginSimulationTime() + ", arrival time: " + getEndSimulationTime() + ", propagation time: " + propagationDelay + ", transmission time: " + transmissionDelay);
	}

	@Override
	public void deactivate() {
        super.deactivate();
        Layer layer = getDecorationLayer();
        layer.remove(messageLabel);
        if (!isPropagationDelayZero && isTransmissionDelayZero)
            layer.remove(messageEllipse);
        else {
		    layer.remove(messageLine);
            layer.remove(messageCompletion);
            layer.remove(messageBeginCompletion);
            layer.remove(messageEndCompletion);
        }
	}

	@Override
	public void refreshAnimation(AnimationPosition animationPosition) {
	    if (gate == null || getConnectionFigure() != null) {
	        boolean visible = animationPosition.getEventNumber() != message.getArrivalEventNumber() || animationPosition.getFrameRelativeAnimationTime() >= messageHighlightAnimationDuration ||
                              2 * animationController.getCurrentRealTime() - Math.floor(2 * animationController.getCurrentRealTime()) < 0.5;
            if (messageLine != null)
                messageLine.setVisible(visible);
            if (messageEllipse != null)
                messageEllipse.setVisible(visible);
            if (messageLabel != null)
                messageLabel.setVisible(visible);
            if (messageCompletion != null)
                messageCompletion.setVisible(visible);
            if (messageBeginCompletion != null)
                messageBeginCompletion.setVisible(visible);
            if (messageEndCompletion != null)
                messageEndCompletion.setVisible(visible);
            if (visible) {
                if (isPropagationDelayZero && isTransmissionDelayZero) {
                    Point[] ps = getMessageSendPoints(animationPosition, 8);
                    Point center = ps[0].getTranslated(ps[1]).scale(0.5);
                    Dimension labelSize = messageLabel.getPreferredSize().getCopy().expand(10, 0);
                    messageLine.setEndpoints(ps[0], ps[1]);
                    ((ConnectionFigure)messageLine).setArrowHeadEnabled(true);
                    messageLabel.setBounds(new Rectangle(center.getTranslated(0, 5), labelSize));
                }
                else if (isPropagationDelayZero && !isTransmissionDelayZero) {
                    Point[] ps = getMessageSendPoints(animationPosition, 4);
                    Point center = ps[0].getTranslated(ps[1]).scale(0.5);
                    Dimension labelSize = messageLabel.getPreferredSize().getCopy().expand(10, 0);
                    messageLine.setEndpoints(ps[0], ps[1]);
                    double completion = animationPosition.getSimulationTime().subtract(getBeginSimulationTime()).doubleValue() / getSimulationTimeDuration().doubleValue();
                    messageCompletion.setCompletion(completion);
                    messageCompletion.setToolTip(new Label("Completed: " + (int)(completion * 100) + "%"));
                    messageCompletion.setBounds(new Rectangle(center.getTranslated(0, 4), new Dimension(7, 7)));
                    messageLabel.setBounds(new Rectangle(center.getTranslated(0, 8), labelSize));
                }
                else if (!isPropagationDelayZero && isTransmissionDelayZero) {
                    Point[] ps = getMessageSendPoints(animationPosition, 4);
                    Dimension labelSize = messageLabel.getPreferredSize().getCopy().expand(10, 0);
                    messageEllipse.setBounds(new Rectangle(ps[0].x - 3, ps[0].y - 2, 7, 7));
                    messageLabel.setBounds(new Rectangle(ps[0].getTranslated(0, 5), labelSize));
                }
                else if (!isPropagationDelayZero && !isTransmissionDelayZero) {
                    Point[] ps = getMessageSendPoints(animationPosition, 4);
                    Point center = ps[0].getTranslated(ps[1]).scale(0.5);
                    Dimension labelSize = messageLabel.getPreferredSize().getCopy().expand(10, 0);
                    messageLine.setEndpoints(ps[0], ps[1]);
                    double beginCompletion = animationPosition.getSimulationTime().subtract(getBeginSimulationTime()).doubleValue() / transmissionDelay.doubleValue();
                    messageBeginCompletion.setVisible(beginCompletion <= 1.0);
                    messageBeginCompletion.setCompletion(beginCompletion);
                    messageBeginCompletion.setToolTip(new Label("Completed: " + (int)(beginCompletion * 100) + "%"));
                    messageBeginCompletion.setBounds(new Rectangle(getBeginPoint().getTranslated(0, 8), new Dimension(7, 7)));
                    double endCompletion = animationPosition.getSimulationTime().subtract(getBeginSimulationTime().add(propagationDelay)).doubleValue() / transmissionDelay.doubleValue();
                    messageEndCompletion.setVisible(endCompletion >= 0.0);
                    messageEndCompletion.setCompletion(endCompletion);
                    messageEndCompletion.setToolTip(new Label("Completed: " + (int)(endCompletion * 100) + "%"));
                    messageEndCompletion.setBounds(new Rectangle(getEndPoint().getTranslated(-8, 8), new Dimension(7, 7)));
                    messageLabel.setBounds(new Rectangle(center.getTranslated(0, 8), labelSize));
                }
            }
	    }
	}

    @Override
    public String toString() {
        if (gate == null)
            return super.toString();
        else
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
