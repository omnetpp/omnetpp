/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.layout;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.AssertionFailedException;
import org.eclipse.draw2d.AbstractLayout;
import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.Debug;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.layout.ISubmoduleConstraint.VectorArrangement;
import org.omnetpp.layout.engine.BasicGraphLayouterEnvironment;


/**
 * This lays out submodules inside a compound module.
 *
 * @author rhornig, andras
 */
public class CompoundModuleLayout extends AbstractLayout {
    private static boolean debug = false;

    private static final Dimension DEFAULT_SIZE = new Dimension(300, 200);
    protected Map<IFigure, Integer> moduleToId;
    protected int algSeed = 1;
    private CompoundModuleFigure compoundFigure;


    /**
     * Constructor.
     */
    public CompoundModuleLayout(CompoundModuleFigure compoundFigure) {
        super();
        this.compoundFigure = compoundFigure;
    }

    /**
     * Creates the autoLayout algorithm using all currently specified constraints.
     */
    @SuppressWarnings("unchecked")
    protected ILayoutAlgorithm createAutoLayouter() {
        ILayoutAlgorithm autoLayouter = createLayouterAlgorithm();
        autoLayouter.setSeed(algSeed);

        // set the layouting area
        int width = compoundFigure.getBackgroundSize().width;
        int height = compoundFigure.getBackgroundSize().height;

        // 1/2 of the width or height for border area, but maximum 30 (see modinsp.cc)
        if (width < 0) width = 0;
        if (height < 0) height = 0;
        int border = 30;
        if (width != 0 && width < 2 * border)
            border = width / 2;
        if (height != 0 && height < 2 * border)
            border = height / 2;
        autoLayouter.setSize(width, height, border);

        // use the current index of the figure as an ID so we will be able to identify
        // the module when we get back it's coordinates from the layouter.
        int nodeIndex = 0;
        // we store the index as an ID using the map
        moduleToId = new HashMap<IFigure, Integer>();
        // iterate over the nodes and add them to the algorithm
        // all child figures on this layer are considered as node
        IFigure nodeParent = compoundFigure.getSubmoduleLayer();
        for (IFigure node : (List<IFigure>)nodeParent.getChildren()) {
            moduleToId.put(node, nodeIndex);

            ISubmoduleConstraint constr = (ISubmoduleConstraint) getConstraint(node);
            Dimension shapeSize = constr.getShapeSize();
            if (shapeSize == null) shapeSize = new Dimension();
            Point centerLocation = constr.getCenterLocation();
            if (centerLocation != null) {
                // use cached (previously layouted) coordinates
                autoLayouter.addFixedNode(nodeIndex, centerLocation.preciseX(), centerLocation.preciseY(), shapeSize.preciseWidth(), shapeSize.preciseHeight());
            }
            else {
                // lay out this node, and store the coordinates with setCenterLocation()
                Point baseLoc = constr.getBaseLocation();
                if (baseLoc != null) {
                    Point offset = getArrangementOffset(constr, 80); // handle vector layouts; note: we use fixed spacing NOT shape size, because items in a vector may be of different sizes which would result in strange arrangements
                    autoLayouter.addFixedNode(nodeIndex, baseLoc.preciseX() + offset.preciseX(), baseLoc.preciseY() + offset.preciseY(), shapeSize.width, shapeSize.height);
                }
                else if (constr.getVectorIdentifier()==null || constr.getVectorArrangement()==VectorArrangement.none) {
                    autoLayouter.addMovableNode(nodeIndex, shapeSize.preciseWidth(), shapeSize.preciseHeight());
                }
                else {
                    Point offset = getArrangementOffset(constr, 80); // handle vector layouts
                    autoLayouter.addAnchoredNode(nodeIndex, constr.getVectorIdentifier().toString(), offset.preciseX(), offset.preciseY(), shapeSize.preciseWidth(), shapeSize.preciseHeight());
                }
            }
            nodeIndex++;
        }

        // iterate over the connections and add them to the algorithm
        // all child figures of type Connection on this layer are considered as edge
        IFigure edgeParent = compoundFigure.getConnectionLayer();
        for (IFigure edge : (List<IFigure>)edgeParent.getChildren()) {
            if (edge instanceof Connection) {
                Connection conn = (Connection)edge;
                IFigure srcFig = conn.getSourceAnchor().getOwner();
                IFigure targetFig = conn.getTargetAnchor().getOwner();
                // add the edge ONLY if it has figures at both side
                if (srcFig == null || targetFig == null)
                    continue;

                if (srcFig instanceof SubmoduleFigure && targetFig instanceof SubmoduleFigure) {
                    autoLayouter.addEdge(moduleToId.get(srcFig), moduleToId.get(targetFig), 0);
                }
                else if (targetFig instanceof SubmoduleFigure) {   // compound to submodule
                    autoLayouter.addEdgeToBorder(moduleToId.get(targetFig), 0);
                }
                else if (srcFig instanceof SubmoduleFigure) {  // submodule to compound
                    autoLayouter.addEdgeToBorder(moduleToId.get(srcFig), 0);
                }
                // we do not care about compound --> compound connections in layouting
            }
        }
        return autoLayouter;
    }

    protected ILayoutAlgorithm createLayouterAlgorithm() {
        // Java implementation:
        // ILayoutAlgorithm layouter = new BasicSpringEmbedderLayoutAlgorithm();

        // Levy's C++ implementation (can be parameterized via BasicGraphLayouterEnvironment):
        // ILayoutAlgorithm layouter = new NativeForceDirectedLayoutAlgorithm(new BasicGraphLayouterEnvironment());

        // Classic C++ implementation:
        BasicGraphLayouterEnvironment environment = new BasicGraphLayouterEnvironment();
        environment.setTimeout(15); //seconds
        ILayoutAlgorithm layouter = new NativeBasicSpringEmbedderLayoutAlgorithm(environment);
        //layouter.setDefaultEdgeLength(100);
        //autoLayouter.setRepulsiveForce(500.0);
        //autoLayouter.setMaxIterations(500);
        return layouter;
    }

    protected Point getArrangementOffset(ISubmoduleConstraint constr, int spacing) {
        int x, y;
        VectorArrangementParameters p = constr.getVectorArrangementParameters();
        switch (constr.getVectorArrangement()) {
            case none: {
                x = 0;
                y = 0;
                break;
            }
            case exact: {
                x = toPixels(p.x, p.scale, 0);
                y = toPixels(p.y, p.scale, 0);
                break;
            }
            case row: {
                int dx = toPixels(p.dx, p.scale, spacing);
                x = constr.getVectorIndex() * dx;
                y = 0;
                break;
            }
            case column: {
                int dy = toPixels(p.dy, p.scale, spacing);
                x = 0;
                y = constr.getVectorIndex() * dy;
                break;
            }
            case matrix: {
                int numCols = (p.n == -1) ? 5 : p.n;
                int dx = toPixels(p.dx, p.scale, spacing);
                int dy = toPixels(p.dy, p.scale, spacing);
                int index = constr.getVectorIndex();
                x = (index % numCols)*dx;
                y = (index / numCols)*dy;
                break;
            }
            case ring: {
                int rx = toPixels(p.dx, p.scale, (int)(spacing*constr.getVectorSize()/(2*Math.PI)));
                int ry = toPixels(p.dy, p.scale, rx);
                double alpha = constr.getVectorIndex() * 2 * Math.PI / constr.getVectorSize();
                x = (int)(rx - rx*Math.sin(alpha));
                y = (int)(ry - ry*Math.cos(alpha));
                break;
            }
            default: {
                throw new AssertionFailedException("unhandled vector arrangement " + constr.getVectorArrangement());
            }
        }
        return new Point(x, y);
    }

    private static int toPixels(float d, float scale, int fallbackValue) {
        if (Float.isNaN(d))
            return fallbackValue;
        return (int)(d * scale);
    }

    /**
     * Implements the algorithm to layout the components of the given container figure.
     *
     * @see LayoutManager#layout(IFigure)
     */
    @SuppressWarnings("unchecked")
    public void layout(IFigure parent) {
        long startTime = System.currentTimeMillis();
        if (parent.getChildren().size() != 0) {
            // create and run the layouter
            ILayoutAlgorithm alg = createAutoLayouter();
            alg.execute();
            // store back the new seed - otherwise unpinned modules created one-by-one would pop up at the exact same place
            algSeed = alg.getSeed();

            // lay out the children according to the auto-layouter
            for (IFigure f : (List<IFigure>)parent.getChildren()) {
                Integer id = moduleToId.get(f);
                Assert.isNotNull(id);

                // get the computed location from the auto-layout algorithm (if there is an algorithm at all)
                ISubmoduleConstraint constr = (ISubmoduleConstraint) getConstraint(f);
                Point locationFromAlg = alg.getNodePosition(id);
                Assert.isNotNull(locationFromAlg);

                // we write back the calculated locations
                constr.setCenterLocation(locationFromAlg);
            }
        }
        if (debug)
            Debug.println("CompoundModuleLayout: " + (System.currentTimeMillis()-startTime) + "ms");
    }

    /**
     * Set a new seed for the layouting algorithm. The next layout will use this seed.
     */
    public void setSeed(int algSeed) {
        // NOTE: this increment is not a mistake, see modinsp.cc where random_seed gets incremented before relayouting starts
        this.algSeed = algSeed + 1;
    }

    /**
     * Returns the current seed value
     */
    public int getSeed() {
        return algSeed;
    }

    /**
     * After calling this, the next layout process will call a full layout process.
     * Pinned nodes will stay, unpinned nodes will move.
     * Nodes with unspecified location will be layouted by generating a random
     * starting position.
     */
    @SuppressWarnings("unchecked")
    public void requestFullLayout() {
        // forget all cached coordinates
        IFigure nodeParent = compoundFigure.getSubmoduleLayer();
        for (IFigure node : (List<IFigure>)nodeParent.getChildren()) {
            ISubmoduleConstraint constr = (ISubmoduleConstraint) getConstraint(node);
            constr.setCenterLocation(null);
        }
    }

    // XXX this algorithm is using ONLY the figure bounds. It might be much better to move
    // it to submoduleLayer.getPreferredSize

    @SuppressWarnings("unchecked")
    @Override
    protected Dimension calculatePreferredSize(IFigure f, int hHint, int wHint) {
        // if we have a size hint, we always use that for the size
        if (hHint >= 0 && wHint >= 0)
            return new Dimension(hHint,wHint);

        layout(f);  //XXX maybe not always needed ??
        Rectangle rect = null;
        for (IFigure child : (List<IFigure>)f.getChildren()) {
            Rectangle r = child.getBounds();

            if (rect == null)
                rect = r.getCopy();
            else
                rect.union(r);

            if (child instanceof SubmoduleFigure) {
                // add the label bounds to it
                rect.union(((SubmoduleFigure)child).getNameBounds());
            }
        }

        // use the default size if no submodule children were present
        if (rect == null)
            return new Dimension(DEFAULT_SIZE);

        // we use the same amount of space on the up/down and the left/right side
        Dimension result = new Dimension(rect.x + Math.max(rect.x,0) + rect.width + f.getInsets().getWidth(), rect.y + Math.max(rect.y,0) + rect.height + f.getInsets().getHeight()).
            union(getBorderPreferredSize(f));
        return result;
    }

    @Override
    public Object getConstraint(IFigure child) {
        return child;  // submodules are their own constraints
    }
}
