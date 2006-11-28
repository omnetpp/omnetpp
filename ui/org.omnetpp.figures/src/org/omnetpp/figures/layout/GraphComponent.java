package org.omnetpp.figures.layout;

import java.util.ArrayList;

class GraphComponent {
	public Rc rc;
	
	public Vertex spanningTreeRoot;

	private ArrayList<Vertex> vertices = new ArrayList<Vertex>();

	private ArrayList<Edge> edges = new ArrayList<Edge>();
	
	public GraphComponent() {
	}

	public GraphComponent(Rc rc) {
		this.rc = rc;
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
		
		calculateSpanningTreeRecursive(null, rootVertex);
	}

	protected void calculateSpanningTreeRecursive(Vertex parentVertex, Vertex vertex) {
		vertex.spanningTreeParent = parentVertex;
		
		if (vertex.spanningTreeChildren == null)
			vertex.spanningTreeChildren = new ArrayList<Vertex>();
		
		if (parentVertex != null)
			parentVertex.spanningTreeChildren.add(vertex);
		
		for (Edge edge : edges) {
			if (edge.source == vertex && edge.target.spanningTreeChildren == null)
				calculateSpanningTreeRecursive(vertex, edge.target);

			if (edge.target == vertex && edge.source.spanningTreeChildren == null)
				calculateSpanningTreeRecursive(vertex, edge.source);
		}
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

	// level in spanning tree
	public int rank;

	// center coordinate relative to parent
	public Pt starTreeCenter;
	
	// subtree enclosing circle center
	public Pt starTreeCircleCenter;
	
	// subtree enclosing circle radius
	double starTreeRadius;

	// subtree width in strip layout
	public double subTreeWidth;

	public Vertex() {
	}

	public boolean isFixed() {
		return positionConstraint instanceof FixPositionConstraint;
	}

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

	public Pt getCenter() {
		return new Pt(pt.x + rs.width / 2, pt.y + rs.height / 2);
	}
}

class Edge {
	public Vertex source;

	public Vertex target;
	
	public Object identity;

	public Edge() {
	}

	public Edge(Vertex source, Vertex target) {
		this(source, target, null);
	}

	public Edge(Vertex source, Vertex target, Object identity) {
		this.source = source;
		this.target = target;
		this.identity = identity;
	}
}

interface IVertexPositionConstraint {
	Pt getForce(Pt pt);

	Pt getFinalPosition(Pt pt);
}

class FixPositionConstraint implements IVertexPositionConstraint {
	public Pt pt;

	public FixPositionConstraint(Pt pt) {
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
