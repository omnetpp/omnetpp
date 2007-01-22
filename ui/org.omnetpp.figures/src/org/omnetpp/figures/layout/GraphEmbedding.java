package org.omnetpp.figures.layout;

public abstract class GraphEmbedding
{
	protected GraphComponent graphComponent;

	public GraphEmbedding(GraphComponent graphComponent)
	{
		this.graphComponent = graphComponent;
	}

	public GraphComponent getGraphComponent() {
		return graphComponent;
	}
	
	public abstract void embed();
}
