package org.omnetpp.figures.layout;

public class ForceDirectedGraphLayoutAlgorithm extends AbstractGraphLayoutAlgorithm {
	private ForceDirectedEmbedding embedding;

	private GraphComponent graphComponent = new GraphComponent();

	@Override
	public void addAnchoredNode(Object module, String anchor, int offx, int offy, int width, int height) {
	}

	@Override
	public void addEdge(Object from, Object to, int len) {
		graphComponent.addEdge(new Edge(getVertex(from), getVertex(to)));
	}

	@Override
	public void addEdgeToBorder(Object from, int len) {
	}

	@Override
	public void addFixedNode(Object module, int x, int y, int width, int height) {
		Vertex vertex = new Vertex(new Pt(x, y), new Rs(width, height), 10, 1, new PointPositionConstraint(new Pt(x, y)), module);
		graphComponent.addVertex(vertex);
	}

	@Override
	public void addMovableNode(Object module, int x, int y, int width, int height) {
		Vertex vertex = new Vertex(new Pt(x, y), new Rs(width, height), 10, 1, null, module);
		graphComponent.addVertex(vertex);
	}

	@Override
	public void execute() {
		if (!graphComponent.isEmpty() && !graphComponent.isFixed()) {
			if (hasInvalidPositions()) {
				graphComponent.calculateSpanningTree();
				new RandomEmbedding(graphComponent, new Rc(0, 0, width, height)).embed();
				new StarTreeEmbedding(graphComponent, 40).embed();
			}

			embedding = new ForceDirectedEmbedding(graphComponent, null);

			//new TestFrame(embedding, width, height);

			embedding.embed();
		}
	}

	@Override
	public org.eclipse.draw2d.geometry.Point getAnchorPosition(String anchor) {
		return null;
	}

	@Override
	public org.eclipse.draw2d.geometry.Point getNodePosition(Object module) {
		Vertex vertex = getVertex(module);
		return new org.eclipse.draw2d.geometry.Point(vertex.pt.x, vertex.pt.y);
	}

	@Override
	public void setAnchorPosition(String anchor, int x, int y) {
	}
	
	protected boolean hasInvalidPositions() {
		for (Vertex vertex : graphComponent.getVertices()) {
			double x = vertex.pt.x;
			double y = vertex.pt.y;

			if (Double.isNaN(x) || Double.isNaN(y) ||
				x == Integer.MIN_VALUE || x == Integer.MAX_VALUE ||
				y == Integer.MIN_VALUE || y == Integer.MAX_VALUE ||
				x == 0 || y == 0) 
				return true;
		}
		
		return false;
	}
	
	protected Vertex getVertex(Object module) {
		for (int i = 0; i < graphComponent.getVertexCount(); i++) {
			Vertex vertex = graphComponent.getVertex(i);

			if (module == vertex.identity)
				return vertex;
		}

		throw new RuntimeException("Vertex not found");
	}
}