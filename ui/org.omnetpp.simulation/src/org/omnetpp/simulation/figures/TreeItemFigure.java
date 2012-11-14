package org.omnetpp.simulation.figures;

import org.eclipse.draw2d.AbstractLayout;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.MouseMotionListener;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;

class TreeItemFigure extends Figure {

    static class ToggleFigure extends Figure{

        boolean isExpanded;
        boolean isMouseOver;
        int [] vertices = new int[6];

        MouseListener mouseListener = new MouseListener.Stub() {
            @Override
            public void mouseReleased(MouseEvent event) {
                onMouseReleased(event);
            }
        };

        MouseMotionListener mouseMotionListener = new MouseMotionListener.Stub() {
            @Override
            public void mouseEntered(MouseEvent me) {
                if (!isMouseOver) {
                    isMouseOver = true;
                    repaint();
                }
            }

            @Override
            public void mouseExited(MouseEvent me) {
                if (isMouseOver) {
                    isMouseOver = false;
                    repaint();
                }
            }
        };

        ToggleFigure() {
            addMouseListener(mouseListener);
            addMouseMotionListener(mouseMotionListener);
        }

        @Override
        public Dimension getPreferredSize(int wHint, int hHint) {
            return isExpanded ? new Dimension(9, 7) : new Dimension(7,9);
        }

        @Override
        public void paint(Graphics graphics) {
            graphics.setForegroundColor(ColorFactory.BLACK);
            graphics.setBackgroundColor(ColorFactory.BLACK);
            Rectangle area = getBounds();
            int x = area.x, y = area.y, w = area.width, h = area.height;
            if (isExpanded) {
                vertices[0] = x;
                vertices[1] = y;
                vertices[2] = x+w;
                vertices[3] = y;
                vertices[4] = x+w/2;
                vertices[5] = y+h;
            }
            else {
                vertices[0] = x;
                vertices[1] = y;
                vertices[2] = x;
                vertices[3] = y+h;
                vertices[4] = x+w;
                vertices[5] = y+h/2;
            }

            if (isMouseOver)
                graphics.fillPolygon(vertices);
            else
                graphics.drawPolygon(vertices);
        }

        private void onMouseReleased(MouseEvent event) {
            if (event.button == 1) {
                isExpanded = !isExpanded;
                revalidate();
                repaint();
            }
        }
    }

    class TreeItemLayout extends AbstractLayout {
        final static int hmargin = 5;
        final static int vmargin = 2;
        private static final int indent = 10;
        final static int spacing = 2;
        final static int toggleWidth = 12;
        final static int toggleHeight = 10;

        @Override
        protected Dimension calculatePreferredSize(IFigure container, int wHint, int hHint) {
            int w = toggleWidth + 2*hmargin + spacing;
            int h = toggleHeight;
            Dimension imageSize = imageFigure.getPreferredSize();
            Dimension labelSize = labelFigure.getPreferredSize();
            w += level * indent + imageSize.width + labelSize.width;
            h = Math.max(h, imageSize.height);
            h = Math.max(h, labelSize.height);
            h += 2 * vmargin;
            return new Dimension(w,h);
        }

        @Override
        public void layout(IFigure container) {
            Rectangle clientArea = container.getClientArea();
            Dimension imageSize = imageFigure.getPreferredSize();
            Dimension labelSize = labelFigure.getPreferredSize();

            Rectangle newBounds = new Rectangle();
            newBounds.x = clientArea.x + hmargin + level * indent;
            newBounds.y = clientArea.y + vmargin;

            if (toggleFigure != null) {
                Dimension toggleSize = toggleFigure.getPreferredSize();
                newBounds.width = toggleSize.width;
                newBounds.height = toggleSize.height;
                toggleFigure.setBounds(newBounds);
            }

            newBounds.x += toggleWidth;
            newBounds.width = imageSize.width;
            newBounds.height = imageSize.height;
            imageFigure.setBounds(newBounds);

            newBounds.x += imageSize.width + spacing;
            newBounds.width = labelSize.width;
            newBounds.height = labelSize.height;
            labelFigure.setBounds(newBounds);
        }
    }

    private Object content;
    private ToggleFigure toggleFigure;
    private ImageFigure imageFigure;
    private StyledLabel labelFigure;
    private boolean isSelected;
    private int level;

    public TreeItemFigure(Object content, int level, boolean hasChildren) {
        this.content = content;
        this.level = level;
        if (hasChildren)
            this.toggleFigure = new ToggleFigure();
        this.imageFigure = new ImageFigure();
        this.labelFigure = new StyledLabel();
        setLayoutManager(new TreeItemLayout());
        if (toggleFigure != null)
            add(toggleFigure);
        add(imageFigure);
        add(labelFigure);
    }

    public Object getContent() {
        return content;
    }

    public int getLevel() {
        return level;
    }

    public void setImage(Image image) {
        imageFigure.setImage(image);
    }

    public void setLabel(StyledString label) {
        labelFigure.setStyledString(label);
    }

    public boolean isExpandable() {
        return toggleFigure != null;
    }

    public boolean isExpanded() {
        return toggleFigure != null && toggleFigure.isExpanded;
    }

    public void setExpanded(boolean isExpanded) {
        if (toggleFigure != null)
            toggleFigure.isExpanded = isExpanded;
    }

    public void setSelected(boolean isSelected) {
        if (this.isSelected == isSelected)
            return;

        this.isSelected = isSelected;
        repaint();
    }

    public boolean isSelected() {
        return isSelected;
    }

    public Figure getToggle() {
        return toggleFigure;
    }

    @Override
    protected void paintFigure(Graphics graphics) {
        TreeFigureTheme theme = TreeFigure.getTheme();
        theme.paintBackground(graphics, getBounds(), 19+level*10 /*FIXME toggleWidth + hmargin + spacing + level*indent*/, isSelected, false /*FIXME*/, true /*FIXME*/);
    }
}
