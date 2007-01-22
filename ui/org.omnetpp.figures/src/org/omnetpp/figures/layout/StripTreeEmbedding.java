package org.omnetpp.figures.layout;

public class StripTreeEmbedding extends RankedTreeEmbedding
{
	public double levelDistance;

	public double minLevelSpacing;
	
	public double leafSpacing;
	
	public boolean levelDistanceRankProportional;

	public StripTreeEmbedding(GraphComponent graphComponent, double levelDistance, double leafSpacing, double minLevelSpacing, boolean levelDistanceRankProportional)
	{
		super(graphComponent);
		this.levelDistance = levelDistance;
		this.leafSpacing = leafSpacing;
		this.minLevelSpacing = minLevelSpacing;
		this.levelDistanceRankProportional = levelDistanceRankProportional;
	}

	public static void embed(GraphComponent graphComponent)
	{
		new StripTreeEmbedding(graphComponent, 0, 0, 0, false).embed();
	}

	public void embed()
	{
		assignRanks();
		calculateRankPosition();
		calculateSubTreeSize();
		calculateNonRankPosition();
	}

	protected void calculateSubTreeSize()
	{
		calculateSubTreeSizeRecursive(graphComponent.spanningTreeRoot);
	}

	protected double calculateSubTreeSizeRecursive(Vertex vertex)
	{
		double size = 0;
		double childrenSize = (vertex.spanningTreeChildren.size() - 1) * leafSpacing;
		double vertexSize = 0;
		
		vertexSize = vertex.rs.width;

		for (Vertex child : vertex.spanningTreeChildren)
			childrenSize += calculateSubTreeSizeRecursive(child);

		if (childrenSize == 0)
			size = vertexSize;
		else
			size = Math.max(childrenSize, vertexSize);

		vertex.subTreeWidth = size;

		return size;
	}

	protected void calculateRankPosition()
	{
		double rankPrev = Double.NEGATIVE_INFINITY;
		double distance = 0;
		double position = 0;

		for (Vertex vertex : getVerticesSortedByRank())
		{
			if (levelDistanceRankProportional)
				position = vertex.rank * levelDistance;
			else
			{
				if (vertex.rank > rankPrev)
				{
					position += distance;
					distance = levelDistance;
				}

				distance = Math.max(distance, vertex.rs.height + minLevelSpacing);
			}

			vertex.pt.y = position;
			rankPrev = vertex.rank;
		}
	}

	private void calculateNonRankPosition()
	{
		calculateNonRankPositionRecursive(graphComponent.spanningTreeRoot, 0);
	}

	private void calculateNonRankPositionRecursive(Vertex vertex, double positionOffset)
	{
		vertex.pt.x = positionOffset + (vertex.subTreeWidth - vertex.rs.width) / 2;

		double chidrenSubTreeSize = (vertex.spanningTreeChildren.size() - 1) * leafSpacing;

		for (Vertex child : vertex.spanningTreeChildren)
			chidrenSubTreeSize += child.subTreeWidth;

		double subTreeSize = 0;
		subTreeSize += vertex.subTreeWidth;
		double childOffset = Math.max(0, (subTreeSize - chidrenSubTreeSize) / 2);

		for (Vertex child : vertex.spanningTreeChildren)
		{
			calculateNonRankPositionRecursive(child, positionOffset + childOffset);

			childOffset += child.subTreeWidth;
			childOffset += leafSpacing;
		}
	}
}
