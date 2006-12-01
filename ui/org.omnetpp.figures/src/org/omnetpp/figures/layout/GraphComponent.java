package org.omnetpp.figures.layout;

import java.util.ArrayList;

/**
 * This is a special graph implementation for the force directed layout algorithm.
 */
class GraphComponent {
	public Vertex spanningTreeRoot;

	private ArrayList<Vertex> vertices = new ArrayList<Vertex>();

	private ArrayList<Edge> edges = new ArrayList<Edge>();
	
	public GraphComponent() {
	}
	
	public boolean isEmpty() {
		return vertices.size() == 0;
	}

	public int addVertex(Vertex vertex) {
		vertices.add(vertex);
		return vertices.size() - 1;
	}

	public int addEdge(Edge edge) {
		edges.add(edge);
		return edges.size() - 1;
	}

	public int getVertexCount() {
		return vertices.size();
	}

	public int IndexOfVertex(Vertex vertex) {
		return vertices.indexOf(vertex);
	}

	public ArrayList<Vertex> getVertices() {
		return vertices;
	}

	public Vertex getVertex(int index) {
		return vertices.get(index);
	}

	public int getEdgeCount() {
		return edges.size();
	}

	public ArrayList<Edge> getEdges() {
		return edges;
	}

	public Edge getEdge(int index) {
		return edges.get(index);
	}

	public boolean isFixed() {
		for (Vertex vertex : vertices)
			if (!vertex.isFixed())
				return false;
		
		return true;
	}

	public void calculateSpanningTree() {
		if (vertices.size() != 0)
			calculateSpanningTree(vertices.get(0));
	}

	public void calculateSpanningTree(Vertex rootVertex) {
		spanningTreeRoot = rootVertex;
		
		for (Vertex vertex : vertices) {
			vertex.spanningTreeChildren = null;
			vertex.spanningTreeParent = null;
		}

		addToSpanningTreeParent(null, rootVertex);
		calculateSpanningTreeRecursive(null, rootVertex);
	}

	protected void calculateSpanningTreeRecursive(Vertex parentVertex, Vertex vertex) {
		for (Vertex neighbour : getNeighbours(vertex))
			if (neighbour.spanningTreeChildren == null)
				addToSpanningTreeParent(vertex, neighbour);

		for (Vertex child : vertex.spanningTreeChildren)
			calculateSpanningTreeRecursive(vertex, child);
	}

	private ArrayList<Vertex> getNeighbours(Vertex vertex) {
		ArrayList<Vertex> neighbours = new ArrayList<Vertex>();

		for (Edge edge : edges) {
			if (edge.source == vertex)
				neighbours.add(edge.target);

			if (edge.target == vertex)
				neighbours.add(edge.source);
		}
		
		return neighbours;
	}

	private void addToSpanningTreeParent(Vertex parentVertex, Vertex vertex) {
		vertex.spanningTreeParent = parentVertex;
		
		if (vertex.spanningTreeChildren == null)
			vertex.spanningTreeChildren = new ArrayList<Vertex>();
		
		if (parentVertex != null)
			parentVertex.spanningTreeChildren.add(vertex);
	}

	public Rc getRc() {
		double x1 = Double.POSITIVE_INFINITY, y1 = Double.POSITIVE_INFINITY;
		double x2 = Double.NEGATIVE_INFINITY, y2 = Double.NEGATIVE_INFINITY;

		for (Vertex vertex : vertices) {
			x1 = Math.min(x1, vertex.pt.x);
			y1 = Math.min(y1, vertex.pt.y);
			x2 = Math.max(x2, vertex.pt.x + vertex.rs.width);
			y2 = Math.max(y2, vertex.pt.y + vertex.rs.height);
		}
			
		return new Rc(x1, y1, x2 - x1, y2 - y1);
	}
}

class Vertex {
	public Pt pt;

	public Rs rs;

	public double mass;

	public double charge;

	public ArrayList<Pt> fs = new ArrayList<Pt>();

	public IVertexPositionConstraint positionConstraint;
	
	public Object identity;
	
	public Vertex spanningTreeParent;
	
	public ArrayList<Vertex> spanningTreeChildren;

	/**
	 * Level in spanning tree.
	 */
	public int rank;

	/**
	 * Center coordinate relative to parent.
	 */
	public Pt starTreeCenter;
	
	/**
	 * Subtree enclosing circle center.
	 */
	public Pt starTreeCircleCenter;
	
	/**
	 * Subtree enclosing circle radius.
	 */
	double starTreeRadius;

	/**
	 * Subtree width in strip layout.
	 */
	public double subTreeWidth;

	/**
	 * Anchor vertex to which the position of this vertex is relative.
	 */
	public Vertex anchor;

	public Vertex(Pt pt, Rs rc, double mass, double charge) {
		this(pt, rc, mass, charge, null, null);
	}

	public Vertex(Pt pt, Rs rc, double mass, double charge, IVertexPositionConstraint positionConstraint) {
		this(pt, rc, mass, charge, positionConstraint, null);
	}

	public Vertex(Pt pt, Rs rc, double mass, double charge, IVertexPositionConstraint positionConstraint, Object identity) {
		this.pt = pt;
		this.rs = rc;
		this.mass = mass;
		this.charge = charge;
		this.positionConstraint = positionConstraint;
		this.identity = identity;
	}

	public boolean isFixed() {
		return positionConstraint instanceof PointPositionConstraint;
	}

	public Pt getCenter() {
		return new Pt(pt.x + rs.width / 2, pt.y + rs.height / 2);
	}
}

class Edge {
	public Vertex source;

	public Vertex target;
	
	public double springCoefficient;
	
	public Object identity;

	public Edge(Vertex source, Vertex target) {
		this(source, target, null, 1);
	}

	public Edge(Vertex source, Vertex target, Object identity, double springCoefficient) {
		this.source = source;
		this.target = target;
		this.identity = identity;
		this.springCoefficient = springCoefficient;
	}
}

interface IVertexPositionConstraint {
	Pt getForce(Pt pt);

	Pt getFinalPosition(Pt pt);
}

class PointPositionConstraint implements IVertexPositionConstraint {
	public Pt pt;

	public PointPositionConstraint(Pt pt) {
		this.pt = pt;
	}

	public Pt getForce(Pt pt) {
		Pt d = this.pt.copy().subtract(pt);

		return d.multiply(d.getLength());
	}

	public Pt getFinalPosition(Pt pt) {
		return this.pt.copy();
	}
}

class LinePositionConstraint implements IVertexPositionConstraint {
	public Ln ln;

	public LinePositionConstraint(Ln ln) {
		this.ln = ln;
	}

	public Pt getForce(Pt pt) {
		Pt d = ln.getClosestPoint(pt).copy().subtract(pt);

		return d;
	}

	public Pt getFinalPosition(Pt pt) {
		return ln.getClosestPoint(pt);
	}
}
