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
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.Debug;
import org.omnetpp.common.displaymodel.DimensionF;
import org.omnetpp.common.displaymodel.PointF;
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

    private static final DimensionF DEFAULT_SIZE = new DimensionF(300.0f, 200.0f);
    protected Map<SubmoduleFigure, Integer> submoduleToId;
    protected int algSeed = 1;
    private CompoundModuleFigure compoundModuleFigure;
    private float scale = Float.NaN;

    /**
     * Constructor.
     */
    public CompoundModuleLayout(CompoundModuleFigure compoundModuleFigure) {
        super();
        this.compoundModuleFigure = compoundModuleFigure;
    }

    /**
     * Set a new seed for the layouting algorithm. The next layout will use this seed.
     */
    public void setSeed(int algSeed) {
        // NOTE: this increment is not a mistake, see modinsp.cc where random_seed gets incremented before relayouting starts
        this.algSeed = algSeed + 1;
    }

    /**
     * Set the scale for unit-to-pixel translation. This value is not used much,
     * because the layouter operates in model coordinates, not pixels.
     */
    public void setScale(float scale) {
        this.scale = scale;
    }

    /**
     * Creates the autoLayout algorithm using all currently specified constraints.
     * NOTE: the layouter operates in model coordinates, not pixels.
     */
    @SuppressWarnings("unchecked")
    protected ILayoutAlgorithm createAutoLayouter() {
        ILayoutAlgorithm autoLayouter = createLayouterAlgorithm();
        autoLayouter.setSeed(algSeed);

        // set the layouting area
        float width = compoundModuleFigure.getBackgroundSize().width / scale; //FIXME not quite!!!
        float height = compoundModuleFigure.getBackgroundSize().height / scale; //FIXME not quite!!!

        // 1/2 of the width or height for border area, but maximum 30 (see modinsp.cc)
        if (width < 0) width = 0;
        if (height < 0) height = 0;
        float border = 30;
        if (width != 0 && width < 2 * border)
            border = width / 2;
        if (height != 0 && height < 2 * border)
            border = height / 2;
        autoLayouter.setSize(width, height, border);

        // use the current index of the figure as an ID so we will be able to identify
        // the module when we get back it's coordinates from the layouter.
        int nodeIndex = 0;

        // we store the index as an ID using the map
        submoduleToId = new HashMap<SubmoduleFigure, Integer>();

        // iterate over the nodes and add them to the algorithm
        // all child figures on this layer are considered as node
        IFigure nodeParent = compoundModuleFigure.getSubmoduleLayer();
        for (SubmoduleFigure node : (List<SubmoduleFigure>)nodeParent.getChildren()) {
            submoduleToId.put(node, nodeIndex);

            ISubmoduleConstraint constraint = getConstraint(node);
            DimensionF shapeSize = constraint.getShapeSize();
            if (shapeSize == null)
                shapeSize = new DimensionF(0, 0);
            PointF layoutedLoc = constraint.getLayoutedLocation();
            if (layoutedLoc != null) {
                // use cached (previously layouted) coordinates
                autoLayouter.addFixedNode(nodeIndex, layoutedLoc.x, layoutedLoc.y, shapeSize.width, shapeSize.height);
            }
            else {
                // lay out this node, and store the coordinates with setCenterLocation()
                PointF baseLoc = constraint.getBaseLocation();
                if (baseLoc != null) {
                    PointF offset = getArrangementOffset(constraint, 80); // handle vector layouts; note: we use fixed spacing NOT shape size, because items in a vector may be of different sizes which would result in strange arrangements
                    autoLayouter.addFixedNode(nodeIndex, baseLoc.x + offset.x, baseLoc.y + offset.y, shapeSize.width, shapeSize.height);
                }
                else if (constraint.getVectorIdentifier()==null || constraint.getVectorArrangement()==VectorArrangement.none) {
                    autoLayouter.addMovableNode(nodeIndex, shapeSize.width, shapeSize.height);
                }
                else {
                    PointF offset = getArrangementOffset(constraint, 80); // handle vector layouts
                    autoLayouter.addAnchoredNode(nodeIndex, constraint.getVectorIdentifier().toString(), offset.x, offset.y, shapeSize.width, shapeSize.height);
                }
            }
            nodeIndex++;
        }

        // iterate over the connections and add them to the algorithm
        // all child figures of type Connection on this layer are considered as edge
        IFigure edgeParent = compoundModuleFigure.getConnectionLayer();
        for (IFigure edge : (List<IFigure>)edgeParent.getChildren()) {
            if (edge instanceof Connection) {
                Connection conn = (Connection)edge;
                IFigure srcFig = conn.getSourceAnchor().getOwner();
                IFigure targetFig = conn.getTargetAnchor().getOwner();
                // add the edge ONLY if it has figures at both side
                if (srcFig == null || targetFig == null)
                    continue;

                if (srcFig instanceof SubmoduleFigure && targetFig instanceof SubmoduleFigure) {
                    autoLayouter.addEdge(submoduleToId.get(srcFig), submoduleToId.get(targetFig), 0);
                }
                else if (targetFig instanceof SubmoduleFigure) {   // compound to submodule
                    autoLayouter.addEdgeToBorder(submoduleToId.get(targetFig), 0);
                }
                else if (srcFig instanceof SubmoduleFigure) {  // submodule to compound
                    autoLayouter.addEdgeToBorder(submoduleToId.get(srcFig), 0);
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

    protected PointF getArrangementOffset(ISubmoduleConstraint constraint, float spacing) {
        float x, y;
        VectorArrangementParameters p = constraint.getVectorArrangementParameters();
        switch (constraint.getVectorArrangement()) {
            case none: {
                x = 0;
                y = 0;
                break;
            }
            case exact: {
                x = fallback(p.x, 0);
                y = fallback(p.y, 0);
                break;
            }
            case row: {
                float dx = fallback(p.dx, spacing);
                x = constraint.getVectorIndex() * dx;
                y = 0;
                break;
            }
            case column: {
                float dy = fallback(p.dy, spacing);
                x = 0;
                y = constraint.getVectorIndex() * dy;
                break;
            }
            case matrix: {
                int numCols = (p.n == -1) ? 5 : p.n;
                float dx = fallback(p.dx, spacing);
                float dy = fallback(p.dy, spacing);
                int index = constraint.getVectorIndex();
                x = (index % numCols) * dx;
                y = (index / numCols) * dy;
                break;
            }
            case ring: {
                float rx = fallback(p.dx, (int)(spacing*constraint.getVectorSize()/(2*Math.PI)));
                float ry = fallback(p.dy, rx);
                double alpha = constraint.getVectorIndex() * 2 * Math.PI / constraint.getVectorSize();
                x = rx - (float)(rx*Math.sin(alpha));
                y = ry - (float)(ry*Math.cos(alpha));
                break;
            }
            default: {
                throw new AssertionFailedException("unhandled vector arrangement " + constraint.getVectorArrangement());
            }
        }
        return new PointF(x, y);
    }

    private static float fallback(float value, float fallbackValue) {
        return Float.isNaN(value) ? fallbackValue : value;
    }

    /**
     * Implements the algorithm to layout the components of the given container figure.
     *
     * @see LayoutManager#layout(IFigure)
     */
    public void layout(IFigure parent) {
        Assert.isTrue(parent == compoundModuleFigure.getSubmoduleLayer());

        long startTime = System.currentTimeMillis();
        if (parent.getChildren().size() != 0) {
            // create and run the layouter
            ILayoutAlgorithm alg = createAutoLayouter();
            alg.execute();

            // store back the new seed - otherwise unpinned modules created one-by-one would pop up at the exact same place
            algSeed = alg.getSeed();

            // get the computed location from the auto-layout algorithm, and write them back
            for (SubmoduleFigure node : submoduleToId.keySet()) {
                Integer id = submoduleToId.get(node);
                PointF loc = alg.getNodePosition(id);
                ISubmoduleConstraint constraint = getConstraint(node);
                constraint.setLayoutedLocation(loc);
                node.setCenterPosition(loc.toPixels(scale));
            }
        }
        if (debug)
            Debug.println("CompoundModuleLayout: " + (System.currentTimeMillis()-startTime) + "ms");
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
        IFigure nodeParent = compoundModuleFigure.getSubmoduleLayer();
        for (IFigure node : (List<IFigure>)nodeParent.getChildren()) {
            ISubmoduleConstraint constr = getConstraint(node);
            constr.setLayoutedLocation(null);
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
            return DEFAULT_SIZE.toPixels(scale);

        // we use the same amount of space on the up/down and the left/right side
        Dimension result = new Dimension(rect.x + Math.max(rect.x,0) + rect.width + f.getInsets().getWidth(), rect.y + Math.max(rect.y,0) + rect.height + f.getInsets().getHeight()).
            union(getBorderPreferredSize(f));
        return result;
    }

    @Override
    public ISubmoduleConstraint getConstraint(IFigure child) {
        return ((SubmoduleFigure)child).getLayoutConstraint();
    }
}
