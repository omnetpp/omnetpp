package org.omnetpp.figures.layout;

import java.util.ArrayList;
import java.util.List;

public abstract class RankedTreeEmbedding extends GraphEmbedding
{
	public RankedTreeEmbedding(GraphComponent graphComponent)
	{
		super(graphComponent);
	}

	protected void assignRanks()
	{
		for (Vertex vertex : graphComponent.getVertices())
			vertex.rank = -1;

		graphComponent.spanningTreeRoot.rank = 0;
		assignRanksRecursive(graphComponent.spanningTreeRoot, 0);
	}

	protected void assignRanksRecursive(Vertex vertex, int rank)
	{
		for (Vertex child : vertex.spanningTreeChildren)
			if (child.rank == -1)
				child.rank = rank + 1;

		for (Vertex child : vertex.spanningTreeChildren)
			assignRanksRecursive(child, rank + 1);
	}

	protected List<Vertex> getVerticesSortedByRank() {
		ArrayList<Vertex> vertices = new ArrayList<Vertex>();
		
		addVerticesSortedByRank(graphComponent.spanningTreeRoot, vertices);

		return vertices;
	}

	private void addVerticesSortedByRank(Vertex vertex, ArrayList<Vertex> vertices) {
		vertices.add(vertex);
		
		for (Vertex child : vertex.spanningTreeChildren)
			addVerticesSortedByRank(child, vertices);
	}
}
