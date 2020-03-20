package org.omnetpp.sequencechart.widgets;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;

import org.eclipse.draw2d.geometry.Rectangle;

public class QuadTree {
    public interface IVisitor {
        public void visit(Rectangle region, Object object);
    }

    private static class Leaf {
        public Rectangle region;
        public Object object;

        public Leaf(Rectangle r, Object v) {
            region = r;
            object = v;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result
                    + ((object == null) ? 0 : object.hashCode());
            result = prime * result
                    + ((region == null) ? 0 : region.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            Leaf other = (Leaf) obj;
            if (object == null) {
                if (other.object != null)
                    return false;
            } else if (!object.equals(other.object))
                return false;
            if (region == null) {
                if (other.region != null)
                    return false;
            } else if (!region.equals(other.region))
                return false;
            return true;
        }

        @Override
        public String toString() {
            return region.toString() + ": " + object;
        }
    }

    private static class Node {
        private interface IVisitor {
            public void visit(Leaf leaf);
        }

        private int size = 0;
        private Rectangle region;
        private Node northWest;
        private Node northEast;
        private Node southWest;
        private Node southEast;
        private ArrayList<Leaf> leaves = new ArrayList<Leaf>();

        public Node(Rectangle r) {
            region = r;
        }

        public int getSize() {
            return size;
        }

        public void insert(Rectangle r, Object v) {
            size++;
            if (leaves != null) {
                leaves.add(new Leaf(r, v));
                if (leaves.size() > 16) {
                    boolean smallRegion = true;
                    for (Leaf leaf : leaves)
                        smallRegion &= leaf.region.contains(region);
                    if (!smallRegion)
                        subdivide();
                }
            }
            else {
                if (northWest.region.intersects(r))
                    northWest.insert(r, v);
                if (northEast.region.intersects(r))
                    northEast.insert(r, v);
                if (southWest.region.intersects(r))
                    southWest.insert(r, v);
                if (southEast.region.intersects(r))
                    southEast.insert(r, v);
            }
        }

        public void query(Rectangle r, IVisitor v) {
            if (region.intersects(r)) {
                if (leaves != null) {
                    for (Leaf leaf : leaves)
                        if (leaf.region.intersects(r))
                            v.visit(leaf);
                }
                else {
                    northWest.query(r, v);
                    northEast.query(r, v);
                    southWest.query(r, v);
                    southEast.query(r, v);
                }
            }
        }

        private void subdivide() {
            int halfWidth = region.width / 2;
            int halfHeight = region.height / 2;
            northWest = new Node(new Rectangle(region.x, region.y, halfWidth, halfHeight));
            northEast = new Node(new Rectangle(region.x + halfWidth, region.y, halfWidth, halfHeight));
            southWest = new Node(new Rectangle(region.x, region.y + halfHeight, halfWidth, halfHeight));
            southEast = new Node(new Rectangle(region.x + halfWidth, region.y + halfHeight, halfWidth, halfHeight));
            for (Leaf leaf : leaves) {
                if (northWest.region.intersects(leaf.region))
                    northWest.leaves.add(leaf);
                if (northEast.region.intersects(leaf.region))
                    northEast.leaves.add(leaf);
                if (southWest.region.intersects(leaf.region))
                    southWest.leaves.add(leaf);
                if (southEast.region.intersects(leaf.region))
                    southEast.leaves.add(leaf);
            }
            leaves = null;
        }

        @Override
        public String toString() {
            if (leaves != null) {
                StringBuffer buffer = new StringBuffer("{");
                for (Leaf leaf : leaves) {
                    buffer.append(", ");
                    buffer.append(leaf.toString());
                }
                buffer.append("}");
                return buffer.toString();
            }
            else
                return "NW: " + northWest.toString() + ", NE: " + northEast.toString() + ", SW: " + southWest.toString() + ", SE: " + southEast.toString();
        }
    }

    private Node root;

    public int getSize() {
        return root != null ? root.getSize() : 0;
    }

    public void insert(Rectangle r, Object o) {
        if (root == null) {
            root = new Node(r);
            root.insert(r, o);
        }
        else if (root.region.contains(r))
            root.insert(r, o);
        else {
            Node node = new Node(root.region.getUnion(r));
            query(root.region, (Rectangle region, Object object) -> node.insert(region, object));
            node.insert(r, o);
            root = node;
        }
    }

    public void query(Rectangle r, IVisitor v) {
        if (root != null) {
            Set<Leaf> seen = new HashSet<Leaf>();
            root.query(r, (Leaf leaf) -> {
                if (!seen.contains(leaf)) {
                    seen.add(leaf);
                    v.visit(leaf.region, leaf.object);
                }
            });
        }
    }

    public void clear() {
        root = null;
    }

    @Override
    public String toString() {
        return root != null ? root.toString() : "{}";
    }
}
