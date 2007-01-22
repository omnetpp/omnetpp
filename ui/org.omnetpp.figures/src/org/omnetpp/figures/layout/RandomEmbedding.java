package org.omnetpp.figures.layout;

import java.util.Random;


public class RandomEmbedding extends GraphEmbedding
{
	private Random random;
	
	private Rc rc;
	
	public RandomEmbedding(GraphComponent graphComponent, Rc rc)
	{
		super(graphComponent);
		this.rc = rc;
	}

	public static void embed(GraphComponent graphComponent)
	{
		new RandomEmbedding(graphComponent, new Rc(0, 0, 500, 500)).embed();
	}

	public void embed()
	{
		random = new Random(0);

		double width = rc.rs.width;
		double height = rc.rs.height;

		for (Vertex vertex : graphComponent.getVertices())
			if (vertex.pt.isNil()) {
				vertex.pt.x = rc.pt.x + random.nextDouble() * (width - vertex.rs.width);
				vertex.pt.y = rc.pt.y + random.nextDouble() * (height - vertex.rs.height);
			}

	}
}
