package org.omnetpp.figures.layout;

import java.awt.Color;
import java.awt.Frame;
import java.awt.Graphics;
import java.util.ArrayList;
import java.util.Random;

public class ForceDirectedEmbedding
{	
	// Algorithm parameters
	// Magnetism between vertexs having connections.
	public double connectionSpringCoefficient = 0.1;

	// Magnetism and repeal between two connections' endings.
	public double connectionMagneticFieldCoefficient = 0;// 1000;
	public Pt connectionHomogenousMagneticFieldPower = new Pt(0, 0.5);
	public double connectionMagneticDipolPower = 10;

	// Repeal between two vertexs.
	public double vertexElectricRepealCoefficient = 10000;

	// Magnetism between two vertexs.
	public double vertexSpringCoefficient = 0;// 0.001;

	// Friction reduces energy.
	public double frictionCoefficient = 0.3;

	// Simulation time step.
	public double timeStep = 0.01;

	// Maximum simulation time step allowed.
	public double minTimeStep = 0.0001;

	// Maximum simulation time step allowed.
	public double maxTimeStep = 10;

	// Limit of acceleration difference (between bs in RK-4).
	// Error has to be between Min and Max
	public double accelerationMinErrorLimit = 1;
	public double accelerationMaxErrorLimit = 5;

	// Acceleration limit during relaxing.
	public double accelerationRelaxLimit = 1;

	// Velocity limit during relaxing.
	public double velocityRelaxLimit = 1;

	// Maximim velocity;
	public double maxVelocity = 100;

	// Maximum spring force.
	public double maxSpringForce = 100;

	// Maximum electric repeal force.
	public double maxElectricForce = 100;

	// Maximum magnetic force.
	public double maxMagneticForce = 100;

	// Minimum number of cycles of relaxing.
	public int minCycle = 0;

	// Maximum number of cycles of relaxing.
	public int maxCycle = 1000;
	
	// Implementation fields
	private Random random;

	private GraphComponent graphComponent;
	
	IForceDirectedEmbeddingListener listener;

	// Constructors
	public ForceDirectedEmbedding(GraphComponent graphComponent, IForceDirectedEmbeddingListener listener) {
		this.graphComponent = graphComponent;
		this.listener = listener;
	}

	// Find the solution for the differential equation using the
	// Runge-Kutta-4 velocity dependent forces algorithm.
	//
	// b1 = b[yn, yn']
	// b2 = b[yn + h / 2 * yn' + h * h / 8 * b1, yn' + h / 2 * b1]
	// b3 = b[yn + h / 2 * yn' + h * h / 8 * b1, yn' + h / 2 * b2]
	// b4 = b[yn + h * yn' + h * h / 2 * b3, yn' + h * b3]
	//
	// yn+1 = yn + h * yn' + h * h / 6 * [b1 + b2 + b3]
	// yn+1' = yn' + h / 6 * (b1 + 2 * b2 + 2 * b3 + b4)
	public void embed() {
		int cycle = 0;
		int probCycle = 0;
		boolean relaxed = false;
		// h (Low and High for log h search)
		double h = timeStep;
		double hLow;
		double hHigh;
		// yn : positions
		Pt[] ps = createPtArray();
		getCenters(ps);
		// accelerations
		Pt[] acs = createPtArray();
		// yn' : velocities
		Pt[] vs = createPtArray();
		// bs
		Pt[] b1 = createPtArray();
		Pt[] b2 = createPtArray();
		Pt[] b3 = createPtArray();
		Pt[] b4 = createPtArray();
		// velocity deltas
		Pt[] dvs = createPtArray();
		Pt[] tvs = createPtArray();
		// position deltas
		Pt[] dps = createPtArray();
		Pt[] tps = createPtArray();
		// initialize random for no distance repealing
		// make it stable and always use the same seed
		random = new Random(0);
		boolean fFirstRun = true;

		for (Vertex vertex : graphComponent.getVertices()) {
			if ((!vertex.hasFixedXPosition && vertex.pt.x != 0) ||
				(!vertex.hasFixedYPosition && vertex.pt.y != 0))
			{
				fFirstRun = false;
				break;
			}
		}

		if (fFirstRun) {
			double width = 0;
			double height = 0;
			
			for (Vertex vertex : graphComponent.getVertices()) {
				width += vertex.rc.width;
				height += vertex.rc.height;
			}

			for (Vertex vertex : graphComponent.getVertices()) {
				vertex.pt.x = random.nextDouble() * width;
				vertex.pt.y = random.nextDouble() * height;
			}
		}

		while (!relaxed) {
			// Runge Kutta 4th order
			hLow = 0;
			hHigh = 0;
			boolean hFound = false;

			while (!hFound) {
				probCycle++;

				// b1 = b[yn, yn']
				b(b1, ps, vs);

				// b2 = b[yn + h / 2 * yn' + h * h / 8 * b1, yn' + h / 2 * b1]
				addMul(tps, ps, h / 2, vs);
				addMul(tps, tps, h * h / 8, b1);
				addMul(tvs, vs, h / 2, b1);
				b(b2, tps, tvs);
			
				// b3 = b[yn + h / 2 * yn' + h * h / 8 * b1, yn' + h / 2 * b2]
				addMul(tps, ps, h / 2, vs);
				addMul(tps, tps, h * h / 8, b2);// ???? b1
				addMul(tvs, vs, h / 2, b2);
				b(b3, tps, tvs);
			
				// b4 = b[yn + h * yn' + h * h / 2 * b3, yn' + h * b3]
				addMul(tps, ps, h, vs);
				addMul(tps, tps, h * h / 2, b3);
				addMul(tvs, vs, h, b3);
				b(b4, tps, tvs);

				// Adjust time step (h)
				double diff = diff(b1, b2, b3, b4);

				// Console.WriteLine("**** " + diff + " ***** " + h);

				if (diff < accelerationMinErrorLimit) {
					hLow = h;

					if (hHigh != 0)
						h = (hLow + hHigh) / 2;
					else
						h *= 2;
				}
				else if (diff > accelerationMaxErrorLimit) {
					hHigh = h;

					if (hLow != 0)
						h = (hLow + hHigh) / 2;
					else
						h /= 2;
				}
				else
					hFound = true;

				if (h > maxTimeStep || h < minTimeStep || h == hHigh || h == hLow) {
					hFound = true;
				}
			}

			// yn+1 = yn + h * yn' + h * h / 6 * [b1 + b2 + b3]
			add(dps, b1, b2);
			add(dps, dps, b3);
			mul(dps, h * h / 6, dps);
			addMul(dps, dps, h, vs);
			add(ps, ps, dps);

			// yn+1' = yn' + h / 6 * (b1 + 2 * b2 + 2 * b3 + b4)
			addMul(dvs, b1, 2, b2);
			addMul(dvs, dvs, 2, b3);
			add(dvs, dvs, b4);
			mul(dvs, h / 6, dvs);
			add(vs, vs, dvs);
			
			// Maximize velocity
			for (int i = 0; i < vs.length; i++) {
				if (vs[i].getLength() > maxVelocity) {
					vs[i].Normalize();
					vs[i].multiply(maxVelocity);
				}
			}
				
			// Check if relaxed
			cycle++;

			if (cycle > minCycle)
				relaxed = true;

			if (relaxed && cycle < maxCycle)
				for (int i = 0; i < vs.length; i++) {
					acs[i].assign(b1[i]).add(b2[i]).add(b3[i]).add(b4[i]).divide(4);

					if (vs[i].getLength() > velocityRelaxLimit || acs[i].getLength() > accelerationRelaxLimit) {
						relaxed = false;
						break;
					}
				}

			addToVertexPositions(dps);
			
			if (listener != null)
				listener.positionsChanged();
		}

		// Console.WriteLine("Runge-Kutta-4 number of cycles to relax: " + cycle + "
		// Prob cycle: " + probCycle);
	}

	private Pt[] createPtArray() {
		Pt[] result = new Pt[graphComponent.getVertexCount()];
		
		for (int i = 0; i < result.length; i++)
			result[i] = new Pt(0, 0);
		
		return result;
	}

	// Calculation
	private double diff(Pt[] b1, Pt[] b2, Pt[] b3, Pt[] b4)
	{
		double max = 0;
		assert (b1.length == b2.length);
		assert (b2.length == b3.length);
		assert (b3.length == b4.length);

		for (int i = 0; i < b1.length; i++) {
			max = Math.max(max, b1[i].getDistance(b2[i]));
			max = Math.max(max, b2[i].getDistance(b3[i]));
			max = Math.max(max, b3[i].getDistance(b4[i]));
		}

		return max;
	}

	private void getCenters(Pt[] ps)
	{
		for (int i = 0; i < ps.length; i++)
			ps[i] = graphComponent.getVertex(i).getCenter();
	}

	private void addToVertexPositions(Pt[] dps)
	{
		assert (dps.length == graphComponent.getVertexCount());

		for (int i = 0; i < dps.length; i++) {
			Vertex vertex = graphComponent.getVertex(i);
			vertex.pt.x += dps[i].x;
			vertex.pt.y += dps[i].y;
		}
	}
	
	// result = a + b * c
	private void addMul(Pt[] result, Pt[] a, double b, Pt[] c)
	{
		assert (a.length == c.length);

		for (int i = 0; i < result.length; i++)
			result[i].assign(c[i]).multiply(b).add(a[i]);
	}

	// result = a + b
	private void add(Pt[] result, Pt[] a, Pt[] b)
	{
		assert (a.length == b.length);

		for (int i = 0; i < result.length; i++)
			result[i].assign(a[i]).add(b[i]);
	}

	// result = a * b
	private void mul(Pt[] result, double a, Pt[] b)
	{
		for (int i = 0; i < result.length; i++)
			result[i].assign(b[i]).multiply(a);
	}

	// fs = b[ps, vs]
	private void b(Pt[] fs, Pt[] ps, Pt[] vs)
	{
		// Reset fs
		for (int i = 0; i < fs.length; i++)
			fs[i].setZero();
		
		// Apply connection spring force
		if (connectionSpringCoefficient != 0) {
			for (int i = 0; i < graphComponent.getEdgeCount(); i++) {
				Edge edge = graphComponent.getEdge(i);
				Vertex vertex1 = edge.source;
				Vertex vertex2 = edge.target;

				if (vertex1 != null && vertex2 != null && vertex1 != vertex2) {
					int i1 = graphComponent.IndexOfVertex(edge.source);
					int i2 = graphComponent.IndexOfVertex(edge.target);

					Pt d = new Pt(ps[i2]).subtract(ps[i1]);
					double distance = d.getLength();

					if (distance != 0) {
						double springForce = connectionSpringCoefficient * distance;

						if (springForce > maxSpringForce)
							springForce = maxSpringForce;

						addForcePair(vertex1, vertex2, fs[i1], fs[i2], springForce, d, distance);
					}
				}
			}
		}

		// Apply magnetic force
		if (connectionMagneticFieldCoefficient != 0) {
			for (int i = 0; i < graphComponent.getEdgeCount(); i++) {
				Edge edge1 = graphComponent.getEdge(i);
				int iSource = graphComponent.IndexOfVertex(edge1.source);
				int iTarget = graphComponent.IndexOfVertex(edge1.target);
				Pt pSource1 = ps[iSource];
				Pt pTarget1 = ps[iTarget];
				Vertex sourceVertex1 = edge1.source;
				Vertex targetVertex1 = edge1.target;
				Pt connectionCenter1 = new Pt(pSource1).add(pTarget1).divide(2);
				Pt magneticFieldPower = connectionHomogenousMagneticFieldPower;
				Pt m1 = new Pt(pSource1).subtract(pTarget1);
				double connectionLength = m1.getLength();

				if (connectionLength != 0) {
					m1.Normalize();
					m1.multiply(connectionMagneticDipolPower);

					for (int j = i + 1; j < graphComponent.getEdgeCount(); j++) {
						Edge edge2 = graphComponent.getEdge(j);
						Pt pSource2 = ps[graphComponent.IndexOfVertex(edge2.source)];
						Pt pTarget2 = ps[graphComponent.IndexOfVertex(edge2.target)];
						Pt connectionCenter2 = new Pt(pSource2).add(pTarget2).divide(2);
						Pt m2 = new Pt(pTarget2).subtract(pSource2);
						m2.Normalize();
						m2.multiply(connectionMagneticDipolPower);

						// magnetic dipol field power
						// B = (3(m*r)r - mr^2) / r^5
						Pt r = new Pt(connectionCenter1).subtract(connectionCenter2);
						double rLength = r.getLength();
						Pt dipolFieldPower = (r.copy().multiply(3 * m2.crossProduct(r)).subtract(m2.copy().multiply(Math.pow(rLength, 2)))).divide(Math.pow(rLength, 5)).multiply(connectionMagneticDipolPower);

						magneticFieldPower.add(dipolFieldPower);
					}

					double magneticFieldMoment = magneticFieldPower.x * m1.y - magneticFieldPower.y * m1.x;
					double magneticFieldForce = connectionMagneticFieldCoefficient * magneticFieldMoment / (connectionLength / 2);

					if (Math.abs(magneticFieldForce) > maxMagneticForce)
						magneticFieldForce = Math.signum(magneticFieldForce) * maxMagneticForce;
					else if (Double.isNaN(magneticFieldForce))
						magneticFieldForce = maxMagneticForce;

					if (!sourceVertex1.hasFixedXPosition)
						fs[iSource].x -= magneticFieldForce * m1.y / connectionLength;

					if (!sourceVertex1.hasFixedYPosition)
						fs[iSource].y += magneticFieldForce * m1.x / connectionLength;

					if (!targetVertex1.hasFixedXPosition)
						fs[iTarget].x += magneticFieldForce * m1.y / connectionLength;

					if (!targetVertex1.hasFixedYPosition)
						fs[iTarget].y -= magneticFieldForce * m1.x / connectionLength;
				}
			}
		}

		// Apply electric force
		if (vertexElectricRepealCoefficient != 0) {
			for (int i = 0; i < graphComponent.getVertexCount(); i++) {
				Vertex vertex1 = graphComponent.getVertex(i);

				for (int j = i + 1; j < graphComponent.getVertexCount(); j++) {
					Vertex vertex2 = graphComponent.getVertex(j);

					Pt d = new Pt(ps[i]).subtract(ps[j]);
					double distance = d.getLength();
					double modifiedDistance = distance - Math.sqrt(vertex1.rc.width * vertex1.rc.width + vertex1.rc.height * vertex1.rc.height) / 2 - Math.sqrt(vertex2.rc.width * vertex2.rc.width + vertex2.rc.height * vertex2.rc.height) / 2;
					double electricForce = vertexElectricRepealCoefficient * vertex1.charge * vertex2.charge / modifiedDistance / modifiedDistance;

					if (modifiedDistance <= 0 || electricForce > maxElectricForce)
						electricForce = maxElectricForce * vertex1.charge * vertex2.charge;

					addForcePair(vertex1, vertex2, fs[i], fs[j], electricForce, d, distance);
				}
			}
		}

		// Apply vertex spring force
		if (vertexSpringCoefficient != 0) {
			for (int i = 0; i < graphComponent.getVertexCount(); i++) {
				Vertex vertex1 = graphComponent.getVertex(i);

				for (int j = i + 1; j < graphComponent.getVertexCount(); j++) {
					Vertex vertex2 = graphComponent.getVertex(j);
					Pt d = new Pt(ps[j]).subtract(ps[i]);
					double distance = d.getLength();

					if (distance != 0) {
						double springForce = vertexSpringCoefficient * distance;

						if (springForce > maxSpringForce)
							springForce = maxSpringForce;

						addForcePair(vertex1, vertex2, fs[i], fs[j], springForce, d, distance);
					}
				}
			}
		}
		
		// Apply friction force
		if (frictionCoefficient != 0) {
			for (int i = 0; i < vs.length; i++) {
				double vlen = vs[i].getLength();
				Vertex vertex = graphComponent.getVertex(i);

				if (!vertex.hasFixedXPosition)
					fs[i].x -= frictionCoefficient * vlen * vs[i].x;

				if (!vertex.hasFixedYPosition)
					fs[i].y -= frictionCoefficient * vlen * vs[i].y;
			}
		}
		
		// Convert forces to acceleration
		for (int i = 0; i < graphComponent.getVertexCount(); i++)
			fs[i].divide(graphComponent.getVertex(i).mass);		
	}

	private void addForcePair(Vertex vertex1, Vertex vertex2, Pt f1, Pt f2, double force, Pt d, double distance)
	{
		double forceX;
		double forceY;

		if (distance != 0) {
			forceX = force * d.x / distance;
			forceY = force * d.y / distance;
		}
		else {
			forceX = random.nextDouble() - 0.5;
			forceY = random.nextDouble() - 0.5;
		}

		assert (!Double.isNaN(forceX));
		assert (!Double.isNaN(forceY));

		if (!vertex1.hasFixedXPosition)
			f1.x += forceX;

		if (!vertex1.hasFixedYPosition)
			f1.y += forceY;

		if (!vertex2.hasFixedXPosition)
			f2.x -= forceX;

		if (!vertex2.hasFixedYPosition)
			f2.y -= forceY;
	}

	public interface IForceDirectedEmbeddingListener {
		public void positionsChanged();
	}

	/**
	 * Test.
	 */
	public static void main(String[] args) {
		GraphComponent graphComponent = new GraphComponent();
		Vertex vertex1 = new Vertex(new Pt(0, 0), new Rc(10, 10), false, false, 1, 1);
		Vertex vertex2 = new Vertex(new Pt(10, 0), new Rc(10, 10), false, false, 1, 1);
		Vertex vertex3 = new Vertex(new Pt(0, 10), new Rc(10, 10), false, false, 1, 1);
		Vertex vertex4 = new Vertex(new Pt(10, 10), new Rc(10, 10), false, false, 1, 1);
		graphComponent.addVertex(vertex1);
		graphComponent.addVertex(vertex2);
		graphComponent.addVertex(vertex3);
		graphComponent.addVertex(vertex4);
		graphComponent.addEdge(new Edge(vertex1, vertex2));
		graphComponent.addEdge(new Edge(vertex1, vertex3));
		graphComponent.addEdge(new Edge(vertex1, vertex4));
		graphComponent.addEdge(new Edge(vertex2, vertex3));
		graphComponent.addEdge(new Edge(vertex2, vertex4));
		graphComponent.addEdge(new Edge(vertex3, vertex4));

		TestFrame testFrame = new TestFrame(graphComponent);
		testFrame.setSize(500, 500);
		testFrame.show();
		ForceDirectedEmbedding embedding = new ForceDirectedEmbedding(graphComponent, testFrame);
		embedding.embed();
		
		for (Vertex vertex : graphComponent.getVertices())
			System.out.println(vertex.pt);
	}
}

class TestFrame extends Frame implements ForceDirectedEmbedding.IForceDirectedEmbeddingListener {
	private GraphComponent graphComponent;

	public TestFrame(GraphComponent graphComponent) {
		this.graphComponent = graphComponent;
	}
	
	public void positionsChanged() {
		repaint();
	}
	
	public void paint(Graphics g)
	{
		g.setColor(new Color(0, 0, 0));

		for (Vertex vertex : graphComponent.getVertices()) {
			int x = (int)vertex.pt.x;
			int y = (int)vertex.pt.y;
			int width = (int)vertex.rc.width;
			int height = (int)vertex.rc.height;
			g.fillRect(x, y, width, height);
		}
	}
}

class Pt {
	public double x;

	public double y;

	public Pt(double x, double y) {
		this.x = x;
		this.y = y;
	}
	
	public Pt(Pt pt) {
		x = pt.x;
		y = pt.y;
	}

	public Pt copy() {
		return new Pt(x, y);
	}
	
	public Pt assign(Pt pt) {
		x = pt.x;
		y = pt.y;
		
		return this;
	}
	
	public void setZero() {
		x = 0;
		y = 0;
	}

	public double getLength() {
		return Math.sqrt(x * x + y * y);
	}
	
	public double getDistance(Pt other) {
		double dx = x - other.x;
		double dy = y - other.y;
		return Math.sqrt(dx * dx + dy * dy);
	}
	
	public Pt Normalize() {
		double length = getLength();
		x /= length;
		y /= length;

		return this;
	}
	
	public Pt add(Pt pt) {
		x += pt.x;
		y += pt.y;

		return this;
	}

	public Pt subtract(Pt pt) {
		x -= pt.x;
		y -= pt.y;

		return this;
	}

	public Pt multiply(double d) {
		x *= d;
		y *= d;

		return this;
	}
	
	public Pt divide(double d) {
		x /= d;
		y /= d;

		return this;
	}

	public double crossProduct(Pt pt) {
		return x * pt.y - y * pt.x;
	}
	
	@Override
	public String toString() {
		return "x: " + x + " y: " + y;
	}
}

class Rc {
	public double width;

	public double height;

	public Rc(double width, double height) {
		this.width = width;
		this.height = height;
	}

	@Override
	public String toString() {
		return "width: " + width + " height: " + height;
	}
}

class GraphComponent {
	private ArrayList<Vertex> vertices = new ArrayList<Vertex>();
	
	private ArrayList<Edge> edges = new ArrayList<Edge>();
	
	public void addVertex(Vertex vertex) {
		vertices.add(vertex);
	}
	
	public void addEdge(Edge edge) {
		edges.add(edge);
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
}

class Vertex {
	public Pt pt;

	public Rc rc;
	
	public boolean hasFixedXPosition;
	
	public boolean hasFixedYPosition;
	
	public double mass;
	
	public double charge;

	public Vertex() {
	}
	
	public Vertex(Pt pt, Rc rc, boolean hasFixedXPosition, boolean hasFixedYPosition, double mass, double charge) {
		this.pt = pt;
		this.rc = rc;
		this.hasFixedXPosition = hasFixedXPosition;
		this.hasFixedYPosition = hasFixedYPosition;
		this.mass = mass;
		this.charge = charge;
	}

	public Pt getCenter() {
		return new Pt(pt.x + rc.width / 2, pt.y + rc.height / 2);
	}
}

class Edge {
	public Vertex source;
	
	public Vertex target;
	
	public Edge() {
	}

	public Edge(Vertex source, Vertex target) {
		this.source = source;
		this.target = target;
	}
}