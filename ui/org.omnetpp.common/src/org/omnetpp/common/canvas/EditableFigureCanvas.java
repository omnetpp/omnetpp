package org.omnetpp.common.canvas;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.image.ImageUtils;
import org.omnetpp.common.ui.FigureCanvas;

/**
 * @author Andras
 */
//XXX snap to grid for the move/resize?
//FIXME how to evaluate "$PARNAME" references in display strings???
public class EditableFigureCanvas extends ScrolledComposite {
    protected FigureCanvas figureCanvas;

    public EditableFigureCanvas(Composite parent, int style) {
        super(parent, SWT.V_SCROLL | SWT.H_SCROLL);
        figureCanvas = new FigureCanvas(this, SWT.DOUBLE_BUFFERED);
        getHorizontalBar().setIncrement(10);
        getVerticalBar().setIncrement(10);
        setContent(figureCanvas);
    }

    public void addFigure(IFigure figure) {
        int spacing = 2;
        getRootFigure().add(figure);
        Dimension size = figure.getPreferredSize();
        ArrayList<Point> candidates = new ArrayList<Point>();
        candidates.add(new Point(0, 0));
        for (IFigure presentFigure : getEditableFigures()) {
            if (presentFigure != figure) {
                Rectangle r = presentFigure.getBounds();
                candidates.add(r.getTopLeft().translate(-spacing - size.width, 0));
                candidates.add(r.getTopLeft().translate(0, -spacing - size.height));
                candidates.add(r.getTop().translate(-size.width / 2, -spacing - size.height));
                candidates.add(r.getTopRight().translate(spacing, 0));
                candidates.add(r.getTopRight().translate(-size.width, -spacing - size.height));
                candidates.add(r.getRight().translate(spacing, - size.height / 2));
                candidates.add(r.getBottomRight().translate(-size.width, spacing));
                candidates.add(r.getBottomRight().translate(spacing, -size.height));
                candidates.add(r.getBottom().translate(-size.width / 2, spacing));
                candidates.add(r.getBottomLeft().translate(-spacing - size.width, -size.height));
                candidates.add(r.getBottomLeft().translate(0, spacing));
                candidates.add(r.getLeft().translate(-spacing - size.width, -size.height / 2));
            }
        }
        double bestDistance = Double.POSITIVE_INFINITY;
        Point bestPoint = null;
        outer: for (Point point : candidates) {
            double distance = point.getDistance(new Point(0, 0));
            Rectangle rectangle = new Rectangle(point, size);
            for (IFigure editableFigure : getEditableFigures())
                if (editableFigure != figure && editableFigure.getBounds().intersects(rectangle))
                    continue outer;
            if (point.x >= 0 && point.y >= 0 && distance < bestDistance) {
                bestPoint = point;
                bestDistance = distance;
            }
        }
        initializeFigure(figure, bestPoint.x, bestPoint.y);
    }

    @SuppressWarnings("unchecked")
    protected List<IFigure> getEditableFigures() {
        return getRootFigure().getChildren();
    }

    public void addFigure(IFigure figure, int x, int y) {
        getRootFigure().add(figure);
        initializeFigure(figure, x, y);
    }

    protected void initializeFigure(final IFigure figure, int x, int y) {
        getRootFigure().setConstraint(figure, new Rectangle(x, y, -1, -1));
        // reveal new figure on canvas (later when layouting already took place)
        Display.getCurrent().asyncExec(new Runnable() {
            //@Override
            public void run() {
                if (!isDisposed())
                    reveal(figure); //XXX also select it
            }});
        // add move/resize/selection support
        new DraggableFigureMouseListener(figure); //XXX
    }

    public void removeFigure(IFigure figure) {
        getRootFigure().remove(figure);
    }

    public IFigure getRootFigure() {
        return figureCanvas.getRootFigure();
    }
    public void close(IFigure figure) {
        removeFigure(figure);
    }

    public void reveal(IFigure figure) {
        Rectangle bounds = figure.getBounds(); //XXX maybe not good if coords are parent-relative
        setOrigin(bounds.x, bounds.y);
    }

    /**
     * Copies the image of the chart to the clipboard.
     * Uses AWT functionality, because SWT does not support ImageTransfer yet.
     * See: https://bugs.eclipse.org/bugs/show_bug.cgi?id=78856.
     */
    public void copyToClipboard() {
        Clipboard cp = java.awt.Toolkit.getDefaultToolkit().getSystemClipboard();
        ClipboardOwner owner = new java.awt.datatransfer.ClipboardOwner() {
            public void lostOwnership(Clipboard clipboard, Transferable contents) {
            }
        };

        class ImageTransferable implements Transferable {
            public java.awt.Image image;

            public ImageTransferable(java.awt.Image image) {
                this.image = image;
            }

            public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException, IOException {
                if (flavor == DataFlavor.imageFlavor)
                    return image;
                else
                    throw new UnsupportedFlavorException(flavor);
            }

            public DataFlavor[] getTransferDataFlavors() {
                return new DataFlavor[] {DataFlavor.imageFlavor};
            }

            public boolean isDataFlavorSupported(DataFlavor flavor) {
                return flavor == DataFlavor.imageFlavor;
            }
        };

        Image image = getImage(figureCanvas.getSize().x, figureCanvas.getSize().y);
        cp.setContents(new ImageTransferable(ImageUtils.convertToAWT(image)), owner);
    }

    /**
     * Returns the image of the chart.
     */
    public Image getImage(int width, int height) {
        Image image = new Image(getDisplay(), width, height);
        GC gc = new GC(image);
        figureCanvas.print(gc);
        gc.dispose();
        return image;
    }
}
