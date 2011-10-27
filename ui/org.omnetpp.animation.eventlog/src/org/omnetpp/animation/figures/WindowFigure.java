package org.omnetpp.animation.figures;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.image.ImageFactory;

public class WindowFigure extends Figure {
    protected static final int TITLE_HEIGHT = 20;

    private IFigure content;

    private Label title;

    private ImageFigure closer;

    public WindowFigure(IFigure content, String title) {
        this.content = content;
        this.title = new Label(title);
        this.closer = new ImageFigure(ImageFactory.getImage(ImageFactory.DEFAULT_ERROR));
        setLayoutManager(new XYLayout());
        add(this.title);
        setConstraint(this.title, new Rectangle(4, 2, -1, -1));
        add(closer);
        closer.setSize(bounds.width, bounds.height);
        closer.addMouseListener(new MouseListener() {
            public void mousePressed(MouseEvent me) {
                getParent().remove(WindowFigure.this);
            }

            public void mouseReleased(MouseEvent me) {
            }

            public void mouseDoubleClicked(MouseEvent me) {
            }
        });
        setContent(content);
    }

    public IFigure getContent() {
        return content;
    }

    public void setContent(IFigure content) {
        if (getChildren().contains(this.content))
            remove(this.content);
        if (content != null) {
            add(content);
            setConstraint(content, new Rectangle(0, TITLE_HEIGHT, -1, -1));
        }
        this.content = content;
    }

    @Override
    public void paint(Graphics graphics) {
        org.eclipse.swt.graphics.Rectangle imageBounds = closer.getImage().getBounds();
        int y = (TITLE_HEIGHT - imageBounds.height) / 2;
        setConstraint(closer, new Rectangle(getSize().width - imageBounds.width - y, y, -1, -1));
        Rectangle headerRectangle = getClientArea();
        headerRectangle.height = TITLE_HEIGHT;
        graphics.setBackgroundColor(Display.getDefault().getSystemColor(SWT.COLOR_TITLE_BACKGROUND));
        graphics.fillRectangle(headerRectangle);
        graphics.drawRectangle(headerRectangle.x, headerRectangle.y, headerRectangle.width - 1, headerRectangle.height);
        super.paint(graphics);
    }
}
