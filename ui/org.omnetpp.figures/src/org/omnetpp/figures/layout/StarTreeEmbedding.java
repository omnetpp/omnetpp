package org.omnetpp.figures.layout;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;

public class StarTreeEmbedding extends RankedTreeEmbedding
{
	public double leafSpacing;

	public StarTreeEmbedding(GraphComponent graphComponent, double leafSpacing)
	{
		super(graphComponent);
		this.leafSpacing = leafSpacing;
	}

	public static void embed(GraphComponent graphComponent)
	{
		new StarTreeEmbedding(graphComponent, 0).embed();
	}

	public void embed()
	{
		calculateCenter();
		// TODO: resurrect
		//rotateCenter();
		calculatePosition();
	}

	private void calculateCenter()
	{
		calculateCenterRecursive(graphComponent.spanningTreeRoot);
	}

	@SuppressWarnings("unchecked")
	private void calculateCenterRecursive(Vertex vertex)
	{
		if (vertex.spanningTreeChildren.size() == 0)
		{
			vertex.starTreeRadius = vertex.rs.getDiagonalLength() / 2;
			vertex.starTreeCenter = new Pt(0, 0);
			vertex.starTreeCircleCenter = new Pt(0, 0);
		}
		else
		{
			vertex.starTreeCenter = new Pt(0, 0);

			for (Vertex child : vertex.spanningTreeChildren)
				calculateCenterRecursive(child);

			ArrayList<Pt> pts = new ArrayList<Pt>();
			ArrayList<Cc> circles = new ArrayList<Cc>();

			for (Vertex vertexChild : vertex.spanningTreeChildren)
			{
				// Initialize
				pts.clear();
				circles.clear();

				circles.add(new Cc(new Pt(0, 0), vertex.rs.getDiagonalLength() / 2 + leafSpacing + vertexChild.starTreeRadius));

				for (Vertex vertexPositioned : vertex.spanningTreeChildren)
				{
					if (vertexPositioned == vertexChild)
						break;

					circles.add(new Cc(vertexPositioned.starTreeCenter.copy().add(vertexPositioned.starTreeCircleCenter), vertexPositioned.starTreeRadius + leafSpacing + vertexChild.starTreeRadius));
				}

				// Find point candidates
				for (int i = 0; i < circles.size(); i++)
					for (int j = i + 1; j < circles.size(); j++)
					{
						Cc circle1 = circles.get(i);
						Cc circle2 = circles.get(j);

						List<Pt> intersectingPts = circle1.intersect(circle2);

						if (intersectingPts != null)
							pts.addAll(intersectingPts);
					}

				// Default points
				if (circles.size() == 1)
				{
					Cc circleSelf = circles.get(0);
					pts.add(circleSelf.getCenterTop());
					pts.add(circleSelf.getCenterBottom());
					pts.add(circleSelf.getLeftCenter());
					pts.add(circleSelf.getRightCenter());
				}

				for (Pt pt : (ArrayList<Pt>)pts.clone())
					for (Cc circle : circles)
						if (pt.getDistance(circle.origin) < circle.radius - 1)
						{
							pts.remove(pt);
							break;
						}

				// Optimize cost function
				double costMin = Double.POSITIVE_INFINITY;

				Assert.isTrue(pts.size() != 0);

				for (Pt pt : pts)
				{
					double cost = vertex.starTreeCenter.getDistance(pt);

					if (cost < costMin)
					{
						costMin = cost;
						vertexChild.starTreeCenter = pt.copy().subtract(vertexChild.starTreeCircleCenter);
					}
				}
			}

			// Find minimum covering circle
			circles.clear();
			circles.add(new Cc(new Pt(0, 0), vertex.rs.getDiagonalLength() / 2));

			for (Vertex vertexChild : vertex.spanningTreeChildren)
				circles.add(new Cc(vertexChild.starTreeCenter.copy().add(vertexChild.starTreeCircleCenter), vertexChild.starTreeRadius));

			Cc circleEncolsing = Cc.getEnclosingCircle(circles);

			vertex.starTreeRadius = circleEncolsing.radius;
			vertex.starTreeCircleCenter = circleEncolsing.origin;
		}
	}

	private void rotateCenter()
	{
		rotateCenterRecursive(graphComponent.spanningTreeRoot);
	}

	private void rotateCenterRecursive(Vertex vertex)
	{
		if (vertex.spanningTreeChildren.size() != 0)
		{
			if (vertex.spanningTreeParent != null)
			{
				Pt pt = new Pt(0, 0);
				double angle = (vertex.starTreeCenter.copy().add(vertex.starTreeCircleCenter)).getAngle();

				// Find weight point
				Pt weightPoint = new Pt(0, 0);
				double area = 0;

				for (Vertex vertexChild : vertex.spanningTreeChildren)
				{
					area += vertexChild.rs.getArea();
					weightPoint.add(vertexChild.starTreeCenter.copy().add(vertex.starTreeCircleCenter).multiply(vertexChild.rs.getArea()));
				}

				weightPoint.divide(area);
				double angleWeight = weightPoint.getAngle();
				double angleRotate = angle - angleWeight;

				if (!Double.isNaN(angleRotate))
				{
					// Rotate children around circle center
					for (Vertex vertexChild : vertex.spanningTreeChildren)
					{
						pt = vertexChild.starTreeCenter.copy().add(vertexChild.starTreeCircleCenter);
						pt.add(vertex.starTreeCircleCenter);
						pt.rotate(angleRotate);
						vertexChild.starTreeCenter = pt.subtract(vertexChild.starTreeCircleCenter);
					}

					// Rotate parent around circle center
					pt.subtract(vertex.starTreeCircleCenter);
					pt.rotate(angleRotate);
					vertex.starTreeCenter = vertex.starTreeCenter.copy().add(vertex.starTreeCircleCenter).add(pt);
					vertex.starTreeCircleCenter.subtract(pt);

					// Correct children position
					for (Vertex vertexChild : vertex.spanningTreeChildren)
						vertexChild.starTreeCenter.subtract(vertex.starTreeCircleCenter);
				}
			}

			for (Vertex vertexChild : vertex.spanningTreeChildren)
				rotateCenterRecursive(vertexChild);
		}
	}

	private void calculatePosition()
	{
		calculatePositionRecursive(graphComponent.spanningTreeRoot, new Pt(0, 0));
	}

	private void calculatePositionRecursive(Vertex vertex, Pt pt)
	{
		vertex.pt = new Pt(pt.x - vertex.rs.width / 2, pt.y  - vertex.rs.height / 2);
		
		if (vertex.spanningTreeParent != null)
			vertex.starTreeCircleCenter.add(pt);

		for (Vertex child : vertex.spanningTreeChildren)
			calculatePositionRecursive(child, pt.copy().add(child.starTreeCenter));
	}
}
