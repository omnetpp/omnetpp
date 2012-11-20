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
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.graphics.Image;

/**
 * One line in a TreeFigure draw2d widget.
 *
 * @author Tomi
 */
//Note: it would be possible to implement this using a single Figure (except mouse listeners on the toggle)
class TreeItemFigure extends Figure {

    class ToggleFigure extends Figure {
        private final Dimension preferredSize = new Dimension(16,16);

        boolean isExpanded;
        boolean isMouseOver;

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
            return preferredSize;
        }

        @Override
        public void paint(Graphics graphics) {
            Rectangle bounds = getBounds();
            Point centerLoc = new Point(bounds.x + bounds.width / 2, bounds.y + bounds.height / 2);
            TreeFigure.getTheme().paintToggle(graphics, centerLoc, isExpanded, TreeItemFigure.this.isSelected, isMouseOver, TreeItemFigure.this.isActive(), tree.isMouseOver());
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
        final static int hmargin = 1;
        final static int vmargin = 2;
        private static final int indent = 10;
        final static int spacing = 2;
        final static int toggleWidth = 16;
        final static int toggleHeight = 16;

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

    private TreeFigure tree;
    private Object content;
    private ToggleFigure toggleFigure;
    private ImageFigure imageFigure;
    private StyledLabel labelFigure;
    private StyledString labelStyledString;
    private StyledString selectedLabelStyledString;
    private boolean isSelected;
    private boolean isMouseOver;
    private int level;

    private MouseMotionListener mouseMotionListener = new MouseMotionListener.Stub() {
        @Override
        public void mouseEntered(MouseEvent event) {
            if (!isMouseOver) {
                isMouseOver = true;
                repaint();
            }
        }

        @Override
        public void mouseExited(MouseEvent event) {
            if (isMouseOver) {
                isMouseOver = false;
                repaint();
            }
        }
    };

    public TreeItemFigure(TreeFigure tree, Object content, int level, boolean hasChildren) {
        this.tree = tree;
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
        addMouseMotionListener(mouseMotionListener);
    }

    public TreeFigure getTree() {
        return tree;
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
        labelStyledString = label;
        selectedLabelStyledString = TreeFigure.getTheme().getSelectedItemLabel(label, isMouseOver, isActive());
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
        if (isSelected)
            labelFigure.setStyledString(selectedLabelStyledString);
        else
            labelFigure.setStyledString(labelStyledString);
        repaint();
    }

    public boolean isSelected() {
        return isSelected;
    }

    public boolean isActive() {
        return tree.hasFocus();
    }

    public boolean isMouseOver() {
        return isMouseOver;
    }

    public Figure getToggle() {
        return toggleFigure;
    }

    @Override
    protected void paintFigure(Graphics graphics) {
        TreeFigureTheme theme = TreeFigure.getTheme();
        Rectangle imageBounds = imageFigure.getBounds();
        Rectangle labelBounds = labelFigure.getBounds();
        theme.paintBackground(graphics, getBounds(), imageBounds.x, labelBounds.right() - imageBounds.x, isSelected, isMouseOver, isActive(), tree.isMouseOver());
    }
}
