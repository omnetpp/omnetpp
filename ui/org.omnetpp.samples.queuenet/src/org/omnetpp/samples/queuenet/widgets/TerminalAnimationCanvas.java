package org.omnetpp.samples.queuenet.widgets;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.animation.widgets.AnimationCanvas;
import org.omnetpp.samples.queuenet.QueuenetPlugin;

public class TerminalAnimationCanvas extends AnimationCanvas
{
    private final static String IMAGE_DIR = "icons/";
    private final static String IMAGE_PASSENGER = IMAGE_DIR + "passenger.png";
    private final static String IMAGE_AIRPLANE = IMAGE_DIR + "airplane.png";
    private final static String IMAGE_CHECKIN_DESK = IMAGE_DIR + "checkindesk.png";

    public TerminalAnimationCanvas(Composite parent, int style) {
        super(parent, style);
        ImageFigure passengerFigure = new ImageFigure(QueuenetPlugin.getImageDescriptor(IMAGE_PASSENGER).createImage()) {
            @Override
            public void paint(Graphics graphics) {
                super.paint(graphics);
            }
        };
        ImageFigure airplaneFigure = new ImageFigure(QueuenetPlugin.getImageDescriptor(IMAGE_AIRPLANE).createImage());
        ImageFigure checkinDeskFigure = new ImageFigure(QueuenetPlugin.getImageDescriptor(IMAGE_CHECKIN_DESK).createImage());
        getRootFigure().add(passengerFigure);
        addFigure(airplaneFigure);
        addFigure(checkinDeskFigure);
        addFigure(new Label("Hello World"));
        getRootFigure().setConstraint(passengerFigure, new Rectangle(100, 100, 100, 100));
        reveal(passengerFigure);
    }
}
